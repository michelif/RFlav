import FWCore.ParameterSet.Config as cms

jetsWithClusters = cms.EDProducer('JetClusterMatcher',
                                  jets = cms.InputTag('slimmedJets'),
                                  clusters = cms.InputTag('clusterCandidates'),
                                  maxClusterDeltaR = cms.double(0.4),
                                  maxSeedDeltaR = cms.double(0.4),
                                  )
