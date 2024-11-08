'''Author: g. karathanasis. georgios.karathanasis@cern.ch
cfg to run tag and probe ntuple for muon POG. It runs both on AOD and miniAOD
Modified by Andre Frankenthal (a.franken@cern.ch) -- September 2020
usage: cmsRun run_muonAnalyzer_cfg.py option1=value1 option2=value2
'''

from FWCore.ParameterSet.VarParsing import VarParsing
import FWCore.ParameterSet.Config as cms

options = VarParsing('python')

#options.register('resonance', 'Z',
options.register('resonance', 'JPsi',
    VarParsing.multiplicity.singleton,
    VarParsing.varType.string,
    "Set resonance ('Z'/'JPsi')"
)

options.register('isFullAOD', False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "Set to False for MiniAOD datatier"
)

options.register('isMC',True,
#options.register('isMC',False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "Set to True for MC"
)

options.register('isOnlySeeded', False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "Set to True for Tracker Only Seeded Tracks"   
)

options.register('globalTag', '',
    VarParsing.multiplicity.singleton,
    VarParsing.varType.string,
    "Set global tag"
)

options.register('reportEvery', 1000,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.int,
    "Report frequency"
)

options.register('numThreads', 1,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.int,
    "Number of CMSSW threads" 
)

# this parameter is added for Jet Branches (ID varies for different era)
options.register('era', 'Run2023',
    VarParsing.multiplicity.singleton,
    VarParsing.varType.string,
    "era"
)

options.register('includeJets', False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "Set to False to exclude jets information in output ntuples"
)

options.register('fromCRAB', False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "Is config run from CRAB"
)

options.register('isStandAlone', True,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "run StandAlone Muon Analyzers"
)
options.parseArguments()

# defaults

if options._beenSet['globalTag'] and options.globalTag != '':
    globaltag = options.globalTag
else:
    globaltag = '130X_dataRun3_PromptAnalysis_v1' if not options.isMC else '130X_mcRun3_2023_realistic_v14'

# Run local test if no input files provided
if len(options.inputFiles) == 0:
    if options.resonance == 'Z':
        if options.isFullAOD:
            if options.isMC:
                options.inputFiles.append('/store/mc/Run3Summer23BPixDRPremix/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/AODSIM/130X_mcRun3_2023_realistic_postBPix_v2-v3/50000/004b3105-d392-4fdb-b0a5-b355c71db0ac.root')
                options.inputFiles.append('/store/mc/Run3Summer23BPixDRPremix/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/AODSIM/130X_mcRun3_2023_realistic_postBPix_v2-v3/50000/0037e224-e1e0-48a5-b307-0c27bbe20aa7.root')
                options.inputFiles.append('/store/mc/Run3Summer23BPixDRPremix/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/AODSIM/130X_mcRun3_2023_realistic_postBPix_v2-v3/50000/005f14db-fe48-40df-be32-107b56f43bc4.root')
                options.inputFiles.append('/store/mc/Run3Summer23BPixDRPremix/DYto2L-4Jets_MLL-50_TuneCP5_13p6TeV_madgraphMLM-pythia8/AODSIM/130X_mcRun3_2023_realistic_postBPix_v2-v3/50000/0bbf16f4-31ca-43e9-abc9-9fc58abf8c3d.root')
            else:
                options.inputFiles.append('/store/data/Run2023C/Muon1/AOD/PromptReco-v2/000/367/516/00000/0ece5656-fa0c-49d2-a335-cc48c8de4d25.root')
        else:
            if options.isMC:
                options.inputFiles.append('/store/mc/Run3Summer23MiniAODv4/DYto2L-2Jets_MLL-50_TuneCP5_13p6TeV_amcatnloFXFX-pythia8/MINIAODSIM/130X_mcRun3_2023_realistic_v14-v1/2550000/00070982-2c6a-4df6-9af7-530f9155c758.root')

            else:
                options.inputFiles.append('/store/data/Run2022C/SingleMuon/MINIAOD/PromptReco-v1/000/356/383/00000/8014e371-898b-470c-8adc-91d9dc37acfe.root')
                options.inputFiles.append('/store/data/Run2022C/SingleMuon/MINIAOD/PromptReco-v1/000/356/384/00000/84eb615d-43ff-4d46-95de-b0d146147560.root')
                options.inputFiles.append('/store/data/Run2022C/SingleMuon/MINIAOD/PromptReco-v1/000/356/385/00000/6386c4ca-c997-4bde-bae7-9af8a4a508b2.root')
                options.inputFiles.append('/store/data/Run2022C/SingleMuon/MINIAOD/PromptReco-v1/000/356/385/00000/7298deee-c715-4ed3-bc2a-167781df4767.root')
                options.inputFiles.append('/store/data/Run2022C/SingleMuon/MINIAOD/PromptReco-v1/000/356/386/00000/4624d631-ed65-49b4-94d8-5d38a4ec2c56.root')
                options.inputFiles.append('/store/data/Run2022C/SingleMuon/MINIAOD/PromptReco-v1/000/356/386/00000/5496721b-125b-466d-8733-5fa9615994a0.root')
                options.inputFiles.append('/store/data/Run2022C/SingleMuon/MINIAOD/PromptReco-v1/000/356/386/00000/65d688cd-0a80-45fb-bf5a-76860992d44b.root')
                options.inputFiles.append('/store/data/Run2022C/SingleMuon/MINIAOD/PromptReco-v1/000/356/386/00000/6810114b-3345-4c34-9f01-479e6b06f440.root')
                options.inputFiles.append('/store/data/Run2022C/SingleMuon/MINIAOD/PromptReco-v1/000/356/386/00000/6ae34aaf-5d99-4010-9c66-7f22f0b6fe5b.root')
    elif options.resonance == 'JPsi':
        if options.isFullAOD:
            if options.isMC:
                options.inputFiles.append('/store/mc/RunIIAutumn18DRPremix/JpsiToMuMu_JpsiPt8_TuneCP5_13TeV-pythia8/AODSIM/102X_upgrade2018_realistic_v15-v1/270001/FFF2FC1D-18CB-7244-9663-4E36963494B7.root')
            else:
                options.inputFiles.append('/store/data/Run2018A/Charmonium/AOD/17Sep2018-v1/100001/07679496-4DEF-1B44-BA04-768765A80599.root')
        else:
            if options.isMC:
                options.inputFiles.append('/store/mc/Run3Summer23MiniAODv4/Jpsito2Mu_JpsiPT8_TuneCP5_13p6TeV_pythia8/MINIAODSIM/MUO_POG_130X_mcRun3_2023_realistic_v14-v2/2520000/035eee41-121c-44ed-ab78-be79f2d66adc.root')
            else:
                options.inputFiles.append('/store/data/Run2023B/Muon0/MINIAOD/22Sep2023-v1/25520000/d1db3d01-4968-424d-a6e1-62e5d7cde754.root')



if options.outputFile=="":
    options.outputFile      = "output"
    if options.isMC:
        options.outputFile  += "_mc"
    else:
        options.outputFile  += "_data" 
    if options.isFullAOD:
        options.outputFile  += "_full"
    else:
        options.outputFile  += "_mini"
    options.outputFile      += ".root"


                      #default = 'MuonAnalysis/MuonAnalyzer/data/samples/muon/Z/Run2023/database.json',
process = cms.Process("MuonAnalysis")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("TrackingTools/TransientTrack/TransientTrackBuilder_cfi")
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load('Configuration.StandardSequences.EndOfProcess_cff')

process.MessageLogger.cerr.FwkReport.reportEvery = options.reportEvery

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag   = GlobalTag(process.GlobalTag,globaltag, '')

#process.maxEvents  = cms.untracked.PSet(input = cms.untracked.int32(1000))
process.maxEvents   = cms.untracked.PSet(input = cms.untracked.int32(options.maxEvents))

process.source      = cms.Source("PoolSource",
                            fileNames           = cms.untracked.vstring(options.inputFiles),
                            secondaryFileNames  = cms.untracked.vstring(),
                            inputCommands       = cms.untracked.vstring(
                                'keep *',
                                'drop *_ctppsPixelClusters_*_*'
                      )
)

if options.includeJets:
    # for b-tagging
    process.load("RecoBTag.ImpactParameter.impactParameter_cff")
    process.load("RecoBTag.SecondaryVertex.secondaryVertex_cff")
    process.load("RecoBTag.SoftLepton.softLepton_cff")
    process.load("RecoBTag.Combined.combinedMVA_cff")
    process.load("RecoBTag.CTagging.cTagging_cff")
    process.load("RecoBTag.Combined.deepFlavour_cff")
    process.load("JetMETCorrections.Configuration.JetCorrectors_cff")

# Include pat:packedCandidateCollection in AOD for miniPFIsolation
if options.isFullAOD:   
    process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")
    #process.load("CommonTools.RecoAlgos.primaryVertexAssociation_cfi")
    process.load("PhysicsTools.PatAlgos.slimming.primaryVertexAssociation_cfi")
    process.load("PhysicsTools.PatAlgos.slimming.offlineSlimmedPrimaryVertices_cfi")
    process.load("PhysicsTools.PatAlgos.slimming.packedPFCandidates_cfi")
    from PhysicsTools.PatAlgos.slimming.packedPFCandidates_cfi import packedPFCandidates
    process.packedCandsForMuons                 = packedPFCandidates.clone()
    process.packedCandsForMuons.PuppiSrc        = cms.InputTag("")
    process.packedCandsForMuons.PuppiNoLepSrc   = cms.InputTag("")

process.options = cms.untracked.PSet(   wantSummary     = cms.untracked.bool(True),
                                        numberOfThreads = cms.untracked.uint32(options.numThreads)
)

from MuonAnalysis.MuonAnalyzer.tools.ntuple_tools import *
if options.isStandAlone:
    if options.isFullAOD:
        if options.resonance == 'Z':
            process = muonAnalysis_customizeStandAloneFullAOD_Z(process)
        else:
            process = muonAnalysis_customizeStandAloneFullAOD_JPsi(process)
        if not options.isMC:
            process.muon.jetCorrector = cms.InputTag(
                "ak4PFCHSL1FastL2L3ResidualCorrector")
    else:
        if options.resonance == 'Z':
            process = muonAnalysis_customizeStandAloneMiniAOD_Z(process)
        else:
            process = muonAnalysis_customizeStandAloneMiniAOD_JPsi(process)

else:
    if options.isFullAOD:
        if options.resonance == 'Z':
            process = muonAnalysis_customizeFullAOD_Z(process)
        else:
            process = muonAnalysis_customizeFullAOD_JPsi(process)
        if not options.isMC:
            process.muon.jetCorrector = cms.InputTag(
                "ak4PFCHSL1FastL2L3ResidualCorrector")
    else:
        if options.resonance == 'Z':
            process = muonAnalysis_customizeMiniAOD_Z(process)
        else:
            process = muonAnalysis_customizeMiniAOD_JPsi(process)

process.muon.isMC           = options.isMC
process.muon.includeJets    = options.includeJets
process.muon.era            = options.era
process.muon.isOnlySeeded   = options.isOnlySeeded

# Trigger matching
muonSrc = "muons" if options.isFullAOD else "slimmedMuons"
from MuonAnalysis.MuonAssociators.muonL1Match_cfi import muonL1Match as _muonL1Match
process.muonL1Info = _muonL1Match.clone(
    src                         = cms.InputTag(muonSrc),
    useMB2InOverlap             = cms.bool(True),
    useStage2L1                 = cms.bool(True),
    preselection                = cms.string(""),
    matched                     = cms.InputTag("gmtStage2Digis:Muon:"),
    
    useStation2                 = cms.bool(True),
    cosmicPropagationHypothesis = cms.bool(False),
    propagatorAlong             = cms.ESInputTag("", "SteppingHelixPropagatorAlong"),
    propagatorAny               = cms.ESInputTag("", "SteppingHelixPropagatorAny"),
    propagatorOpposite          = cms.ESInputTag("", "SteppingHelixPropagatorOpposite"),
    fallbackToME1               = cms.bool(False)
)
process.muonL1InfoByQ = process.muonL1Info.clone(
    sortBy                      = cms.string("quality"),
    sortByQuality               = cms.bool(True),
    sortByDeltaPhi              = cms.bool(False),
    sortByDeltaEta              = cms.bool(False),
    sortByPt                    = cms.bool(False)
)

process.muon.fallbackToME1                  = cms.bool(False)
process.muon.cosmicPropagationHypothesis    = cms.bool(False)
process.muon.useMB2InOverlap                = cms.bool(True)
process.muon.propagatorAlong                = cms.ESInputTag("", "SteppingHelixPropagatorAlong")
process.muon.propagatorAny                  = cms.ESInputTag("", "SteppingHelixPropagatorAny")
process.muon.propagatorOpposite             = cms.ESInputTag("", "SteppingHelixPropagatorOpposite")

from MuonAnalysis.MuonAnalyzer.hltInfo_cff import getHLTInfo, selectTriggers
hltInfo = getHLTInfo(options.resonance, options.era)
#excludeDSA = (not options.isFullAOD)
excludeDSA = True
process.muon.triggerPaths       = cms.vstring(selectTriggers(hltInfo['triggerPaths'],   True,                   False,  excludeDSA))
process.muon.tagFilters         = cms.vstring(selectTriggers(hltInfo['tagFilters'],     not options.isFullAOD,  True,   excludeDSA))
process.muon.probeFilters       = cms.vstring(selectTriggers(hltInfo['probeFilters'],   not options.isFullAOD,  True,   excludeDSA))
#process.muon.tagFilters        = cms.vstring(selectTriggers(hltInfo['tagFilters'],     True,                   True,   excludeDSA))
#process.muon.probeFilters      = cms.vstring(selectTriggers(hltInfo['probeFilters'],   True,                   True,   excludeDSA))

# Standard selectors
from MuonAnalysis.MuonAnalyzer.selectorInfo_cff import getSelectorNamesAndBits
selectorNames, selectorBits     = getSelectorNamesAndBits(options.era, options.isFullAOD)
process.muon.probeSelectorNames = cms.vstring(selectorNames)
process.muon.probeSelectorBits  = cms.vuint32(selectorBits)
#if not options.isMC:
#    process.LumiInfo = cms.EDProducer('LumiProducerFromBrilcalc',
#                                      lumiFile          = cms.string("lumiData.csv"),
#                                      throwIfNotFound   = cms.bool(False),
#                                      doBunchByBunch    = cms.bool(False)
#)
#else:
#    process.LumiInfo = cms.EDProducer('LumiProducerFromBrilcalc',
#                                      lumiFile          = cms.string("lumiMC.csv"),
#                                      throwIfNotFound   = cms.bool(False),
#                                      doBunchByBunch    = cms.bool(False)
#                                      )
if options.isFullAOD:
    if options.includeJets:
        if not options.isMC:
	        process.analysis_step = cms.Path(
#                process.LumiInfo +
                process.primaryVertexAssociation +
                process.offlineSlimmedPrimaryVertices +
                process.packedCandsForMuons +
                process.muonL1Info +
                process.muonL1InfoByQ +
                process.ak4PFCHSL1FastL2L3ResidualCorrectorChain +
                process.muSequence
            )
        else:
            process.analysis_step = cms.Path(
#                process.LumiInfo +
                process.primaryVertexAssociation +
                process.offlineSlimmedPrimaryVertices +
                process.packedCandsForMuons +
                process.muonL1Info +
                process.muonL1InfoByQ +
                process.ak4PFCHSL1FastL2L3CorrectorChain +
                process.muSequence
	    )
    else:
        process.analysis_step = cms.Path(
#            process.LumiInfo +
            process.primaryVertexAssociation +
            process.offlineSlimmedPrimaryVertices +
            process.packedCandsForMuons +
            process.muonL1Info +
	        process.muonL1InfoByQ +
            process.muSequence
        )
else:
    if options.includeJets:
        if not options.isMC:
            process.analysis_step = cms.Path(
#                process.LumiInfo+
                process.muonL1Info +
                process.muonL1InfoByQ +
                process.ak4PFCHSL1FastL2L3ResidualCorrectorChain +
                process.muSequence
            )
        else:
            process.analysis_step = cms.Path(
#                process.LumiInfo+
                process.muonL1Info +
                process.muonL1InfoByQ +
                process.ak4PFCHSL1FastL2L3CorrectorChain +
                process.muSequence
            )
    else:
        process.analysis_step = cms.Path(
#            process.LumiInfo+
            process.muonL1Info +
            process.muonL1InfoByQ +
            process.muSequence
        )



process.TFileService    = cms.Service("TFileService",
                                        fileName = cms.string(options.outputFile)
                            )
process.endjob_step     = cms.EndPath(process.endOfProcess)

# process.fevt = cms.OutputModule("PoolOutputModule",
#     outputCommands = cms.untracked.vstring(),
#     fileName = cms.untracked.string("edm_output.root")
# )
process.schedule        = cms.Schedule(process.analysis_step, process.endjob_step)

from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)

