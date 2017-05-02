#!/bin/env python

import ROOT as RT

import sys,os


tot_events = 0
tot_kb = 0

for fil in sys.argv[1:]:
    tf = RT.TFile.Open(fil)
    tree=tf.Get("rflavourNtuplizer/tree")
    if tree == None: continue
    tree.Draw("1>>count","Jet_index==-1","goff")
    events = RT.gDirectory.Get("count").GetEntries()
    kb = float(os.path.getsize(fil))/1024.
    tot_events += events
    tot_kb += kb
    print("%s: %d %1.1f" % (fil, events, kb/events ))
    
print("total: %d %1.1f" % (tot_events, tot_kb/tot_events))
