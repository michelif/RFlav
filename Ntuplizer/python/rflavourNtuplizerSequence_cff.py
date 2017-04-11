import FWCore.ParameterSet.Config as cms

from RFlavour.Ntuplizer.rflavourNtuplizer_cfi import rflavourNtuplizer

from RFlavour.TagInfos.clusterCandidatesProducer_cfi import clusterCandidates
from RFlavour.TagInfos.jetsWithClusters_cfi import jetsWithClusters

rflavourNtuplizerSequence = cms.Sequence(clusterCandidates*jetsWithClusters*rflavourNtuplizer)
