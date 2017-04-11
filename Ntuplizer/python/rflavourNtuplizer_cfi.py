import FWCore.ParameterSet.Config as cms

rflavourNtuplizer = cms.EDAnalyzer('RFlavourNtuplesMaker',
                                   
                                   jets = cms.InputTag("jetsWithClusters"),
                                   eventCfg = cms.PSet( treeName = cms.string("tree"),  
                                                        rho = cms.InputTag("fixedGridRhoAll"),
                                                        vertexes = cms.InputTag("offlineSlimmedPrimaryVertices"),
                                                        ),
                                   jetCfg = cms.PSet( treeName = cms.string("tree") ),
                                   candidateCfg = cms.PSet( treeName = cms.string("tree") ),
                                   clusterCfg = cms.PSet( treeName = cms.string("tree") ),
                                   shareTree= cms.bool(True),
                                   )
