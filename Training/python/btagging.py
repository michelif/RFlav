import root_pandas as rpd

from glob import glob
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import json
import os

import ROOT as RT

import random as rnd

from gzip import open as gopen
import cPickle as pickle

from sklearn.utils.extmath import cartesian
import scipy.stats as stats

import random

# ---------------------------------------------------------------------------------------------------
class IO:

    ldata = os.path.expanduser("~/store/t2/btag")
    xdata = "root://cms02.lcg.cscs.ch//store/user/musella/btag/"

    catalog = None
    cross_sections = None
    
    @staticmethod
    def list_files(folder,pattern):

        for x in glob(os.path.join(IO.ldata,folder,pattern)):
            yield os.path.join(IO.xdata,os.path.relpath(x,IO.ldata))

    @staticmethod
    def get_num_events(ifil):
        if IO.catalog == None:
            try:
                with open("catalog.json") as fin:
                    IO.catalog = json.reads(fin.read())
                    fin.close()
            except:
                IO.catalog = {}
        if not ifil in IO.catalog:
            fin = RT.TFile.Open(ifil)
            nevts = fin.Get("allEvents/hEventCount").GetEntries()
            fin.Close()
            IO.catalog[ ifil ] = nevts
            with open("catalog.json","w+") as fout:
                fout.write(json.dumps(IO.catalog))
                fout.close()
                
        return IO.catalog[ifil]
    
    @staticmethod
    def get_cross_section(folder):
        if IO.cross_sections == None:
            with open("cross_sections.json") as fin:
                IO.cross_sections = json.loads(fin.read())
                fin.close()
                
        return IO.cross_sections.get(os.path.basename(folder),-1.)
        
    @staticmethod
    def load_input_ntuples(folder,key="rflavourNtuplizer/tree",nfiles=None,nevents=None,shuffle=None,**kwargs):

        files = list( IO.list_files(folder,"*/*/*/*.root") )
        if shuffle:
            files = rnd.shuffle(files)
        if nfiles != None:
            files = files[:nfiles]

        
        # num_events = reduce(lambda x,y: x+y, map(lambda z: IO.get_num_events(z), files))
        num_events = 0.
        for ifil,fil in enumerate(files):
            num_events += IO.get_num_events(fil)
            if nevents and num_events >= nevents:
                files = files[:ifil+1]
                break
            
        xs = IO.get_cross_section(folder)["xs"]
        print(num_events, xs)
        weight = xs/float(num_events)

        df = None
        for ifil,fil in enumerate(files):            
            try:
                fdf = rpd.read_root(fil,key,**kwargs)
                df = fdf if ifil == 0 else df.append(fdf)
            except Exception, e:
                print("Failed to read file %s: %s" % (fil, e) )
        
        return weight,df
    

    @staticmethod
    def to_pickle(fname,obj):
        with gopen(fname,'w+') as fout:
            pickle.dump(obj,fout)
            fout.close()

    @staticmethod
    def read_pickle(fname):
        obj = None
        with gopen(fname,'r') as fin:
            obj = pickle.load(fin)
            fin.close()
        return obj
    
    
# ---------------------------------------------------------------------------------------------------
class preprocessing:

    @staticmethod
    def define_process_weight(df,proc,weight):
        df['proc'] = ( np.ones_like(df.index)*proc ).astype(np.int8)
        df['weight'] = ( np.ones_like(df.index)*weight ).astype(np.float32)
        df['Jet_label'] = df[ ['Jet_genFlavourCleaned','Jet_genPartonFlavour'] ].apply(preprocessing.jet_label, axis=1, raw=True ).astype(np.int8)

        

    @staticmethod
    def jet_label(x):
        if x[0] == 0: return 0
        flav = min(max(1,np.abs(x[0]) - 2),4)
        if x[0] != 21 and x[1] == 21: flav += 5
        return flav


    @staticmethod
    def adjust_and_compress(df,doNhits=True,doFloat16=True):
        for col in df.columns:
            # print col,df[col].dtype,len(np.unique(df[col].dropna().values))
            if doNhits and col.startswith('Track_nHit'):
                df[col] = df[col].fillna(0.).astype(np.int8)
            elif doFloat16 and df[col].dtype == np.float32:
                df[col] = df[col].astype(np.float16)
            elif "index" in col:
                df[col] = df[col].fillna(-1).astype(np.int8)
        return df

    @staticmethod
    def sample(ns,sampler,pt_min,pt_switch,pt_slope,eta_max,flav):
        flav = flav / flav.sum()
        eta_pdf = stats.uniform(-eta_max,eta_max)
        pt_pdf1 = stats.uniform(pt_min,pt_switch)
        frac2 = 0.
        if pt_slope:
            pt_pdf2 = stats.expon(pt_switch,pt_slope)
            int1 = 1.
            int2 = pt_pdf2.cdf(1000.)*pt_pdf1.pdf(pt_switch)/pt_pdf2.pdf(pt_switch)
            frac2 = int2 / ( int1 + int2 )

        npt = ns
        n1 = npt
        if frac2 > 0.:
            n2 = int(np.round(npt*frac2))
            n1 -= npt
            pts = [ pt_pdf1.rvs(size=n1) ]
        if n2 > 0:
            pts.append( pt_pdf2.rvs(size=n2) )
        pts = np.hstack( pts )
        # pts = pts[ pts < 600. ]
            
        # pts = random.shuffle(pts)[:ns]
        
        etas = eta_pdf.rvs( size=ns )
        flavours = np.apply_along_axis(lambda x: np.argmax(x), 1, np.random.multinomial(1,flav,size=ns) )

        ## print(pts.shape,etas.shape,flavours.shape)
        X = np.vstack( [pts,etas,flavours] ).transpose()
        ## print(X.shape)
        sampler.steps[0][1].set_params(sparse=False)
        y = sampler.predict_proba(X)

        where = X[:,0] > 65.
        y[where,0] = 0.
        norm = y[where,:].sum(axis=1)
        for col in range(y.shape[1]):
            y[where,col] = y[where,col] / norm

        ## print(X[:10],y[:10])
        index = np.apply_along_axis(lambda x: np.argmax(np.random.multinomial(1,x)), 1, y)
        return X,y,index
    
