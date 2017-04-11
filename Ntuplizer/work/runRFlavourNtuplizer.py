import FWCore.ParameterSet.Config as cms

from FWCore.ParameterSet.VarParsing import VarParsing

process = cms.Process("Analysis")

options = VarParsing ('analysis')

options.register('runOnData', False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "Run this on real data"
)
options.register('mcGlobalTag', '80X_mcRun2_asymptotic_2016_TrancheIV_v6', 
    VarParsing.multiplicity.singleton,
    VarParsing.varType.string,
    "MC global tag"
)
options.register('dataGlobalTag', '80X_dataRun2_2016SeptRepro_v4',
    VarParsing.multiplicity.singleton,
    VarParsing.varType.string,
    "Data global tag"
)
options.register('wantSummary', False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "Print out trigger and timing summary"
)

jetCorrectionsAK4 = ('AK4PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None')

postfix = "PFlow"

pvSource = 'offlineSlimmedPrimaryVertices'

genParticles = 'prunedGenParticles'
jetSource = 'slimmedJets'
genJetCollection = 'slimmedGenJets'
pfCandidates = 'packedPFCandidates'
pvSource = 'offlineSlimmedPrimaryVertices'
svSource = 'slimmedSecondaryVertices'
trackSource = 'unpackedTracksAndVertices'
muSource = 'slimmedMuons'
elSource = 'slimmedElectrons'
patMuons = 'slimmedMuons'

globalTag = options.dataGlobalTag if options.runOnData else options.mcGlobalTag 


## bTagInfos = [
##    ##  'pfImpactParameterTagInfos'
##    ## ,'pfSecondaryVertexTagInfos'
##    ## ,'pfInclusiveSecondaryVertexFinderTagInfos'
##    ## ,'pfSecondaryVertexNegativeTagInfos'
##    ## ,'pfInclusiveSecondaryVertexFinderNegativeTagInfos'
##    ## ,'softPFMuonsTagInfos'
##    ## ,'softPFElectronsTagInfos'
##    ## ,'pfInclusiveSecondaryVertexFinderCvsLTagInfos'
##    ## ,'pfInclusiveSecondaryVertexFinderNegativeCvsLTagInfos'
## ]
bTagInfos = None
bTagDiscriminators = [
    'softPFMuonBJetTags'
    ,'softPFElectronBJetTags'
    ,'pfCombinedMVAV2BJetTags'
    ,'pfCombinedCvsBJetTags'
    ,'pfCombinedCvsLJetTags'
    ,'deepFlavourJetTags:probudsg'         
    ,'deepFlavourJetTags:probb'            
    ,'deepFlavourJetTags:probc'            
    ,'deepFlavourJetTags:probbb'           
    ,'deepFlavourJetTags:probcc'           
    ]


from PhysicsTools.PatAlgos.tools.jetTools import *
## Updated the default jet collection
updateJetCollection(
    process,
    jetSource = cms.InputTag(jetSource),
    jetCorrections = jetCorrectionsAK4,
    pfCandidates = cms.InputTag(pfCandidates),
    pvSource = cms.InputTag(pvSource),
    svSource = cms.InputTag(svSource),
    muSource = cms.InputTag(muSource),
    elSource = cms.InputTag(elSource),
    btagInfos = bTagInfos,
    btagDiscriminators = bTagDiscriminators,
    explicitJTA = False,
    postfix = postfix
    )

## Input files
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        '/store/mc/RunIISummer16MiniAODv2/QCD_Pt_80to120_TuneCUETP8M1_13TeV_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/06D23EE0-0EB7-E611-9676-A0369F3102B6.root'
    )
)

## Output file
process.TFileService = cms.Service("TFileService",
   fileName = cms.string(options.outputFile)
)

## Events to process
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )

## Options and Output Report
process.options   = cms.untracked.PSet(
    ## reportEvery = cms.untracked.int32(100),
    wantSummary = cms.untracked.bool(options.wantSummary),
    allowUnscheduled = cms.untracked.bool(True)
)


process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
##Set GT by hand:
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag.globaltag = globalTag

## Filter for removing scraping events
process.noscraping = cms.EDFilter("FilterOutScraping",
    applyfilter = cms.untracked.bool(True),
    debugOn = cms.untracked.bool(False),
    numtrack = cms.untracked.uint32(10),
    thresh = cms.untracked.double(0.25)
)

## Filter for good primary vertex
process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
    vertexCollection = cms.InputTag(pvSource),
    minimumNDOF = cms.uint32(4) ,
    maxAbsZ = cms.double(24),
    maxd0 = cms.double(2)
)
#-------------------------------------

# process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")
process.load("RFlavour.Ntuplizer.rflavourNtuplizerSequence_cff")
process.jetsWithClusters.jets = "selectedUpdatedPatJetsPFlow"

process.filter = cms.Sequence(
    ## process.noscraping*
    process.primaryVertexFilter
    )

# process.load('PhysicsTools.PatAlgos.slimming.unpackedTracksAndVertices_cfi')

process.p = cms.Path(
    process.filter*process.rflavourNtuplizerSequence
    )

open('pydump.py','w').write(process.dumpPython())
