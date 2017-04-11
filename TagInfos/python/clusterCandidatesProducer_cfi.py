import FWCore.ParameterSet.Config as cms

import RecoVertex.AdaptiveVertexFinder.inclusiveCandidateVertexFinder_cfi as ivf

import copy 


clusterCandidates = cms.EDProducer('ClusterCandidatesProducer',
                                   
                                   slimmedVertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
                                   beamSpot = cms.InputTag("offlineBeamSpot"),
                                   additionalTracks= cms.InputTag("lostTracks"),
                                   packedCandidates = cms.InputTag("packedPFCandidates"),
                                   maxNtracks = cms.int32(-1),

                                   minHits = copy.copy(ivf.inclusiveCandidateVertexFinder.minHits),
                                   maximumLongitudinalImpactParameter = copy.copy(ivf.inclusiveCandidateVertexFinder.maximumLongitudinalImpactParameter),
                                   minPt = copy.copy(ivf.inclusiveCandidateVertexFinder.minPt),
                                   clusterizer = copy.deepcopy(ivf.inclusiveCandidateVertexFinder.clusterizer),
                                   )


