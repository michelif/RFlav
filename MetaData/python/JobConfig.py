from flashgg.MetaData.JobConfig import JobConfig
## from flashgg.MicroAOD.MicroAODCustomize import MicroAODCustomize as JobConfig
import FWCore.ParameterSet.Config as cms


class MyJobConfig(JobConfig):
    
    def __init__(self,*args,**kwargs):
        
        super(MyJobConfig,self).__init__(*args,**kwargs)
        
        
    def customize(self,process):
        
        if not hasattr(process,"TFileService"):
            process.TFileService = cms.Service("TFileService",
                                               fileName = cms.string("test.root")
                                               )        
        JobConfig.customize(self,process)
        
## customize = MyJobConfig(metaDataSrc="diphotons",
##                        crossSections=["$CMSSW_BASE/src/flashgg/MetaData/data/cross_sections.json",
##                                       "$CMSSW_BASE/src/diphotons/MetaData/data/cross_sections.json"])

customize = MyJobConfig(metaDataSrc="RFlavour",
                        cross_sections=["$CMSSW_BASE/src/RFlavour/MetaData/data/cross_sections.json"])
