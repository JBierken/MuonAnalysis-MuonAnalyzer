// Package:    HLTAnalysis/TriggerAnalyzer
// Class:      TriggerAnalyzer
//
/**\class TriggerAnalyzer TriggerAnalyzer.cc
  HLTAnalysis/TriggerAnalyzer/plugins/TriggerAnalyzer.cc

Description: Ntuplizer for miniAOD files
*/
//
// Original Author:
//                george karathanasis
//         Created:  Thu, 23 Mar 2017 17:40:23 GMT
//
// Modified:
//                Minseok Oh (Feb. 2021)
//
//

// system include files
#include <iostream>
#include <memory>
#include <random>
// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/Association.h"
#include "DataFormats/Common/interface/AssociationMap.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "JetMETCorrections/Modules/interface/JetResolution.h"
#include "CondFormats/DataRecord/interface/JetResolutionRcd.h"
#include "CondFormats/DataRecord/interface/JetResolutionScaleFactorRcd.h"
#include "DataFormats/JetReco/interface/GenJet.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/L1Trigger/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "HLTrigger/HLTcore/interface/defaultModuleLabel.h"

#include <iostream>
#include <string>
#include <vector>
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidateIsolation.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "KlFitter.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/ParametrizedEngine/src/OAEParametrizedMagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/MuonReco/interface/MuonSimInfo.h"
 
#include "JetMETCorrections/Modules/interface/JetResolution.h"
#include "JetMETCorrections/JetCorrector/interface/JetCorrector.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include "TrackingTools/PatternTools/interface/ClosestApproachInRPhi.h"
#include "TrackingTools/PatternTools/interface/TwoTrackMinimumDistance.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "DataFormats/Luminosity/interface/LumiInfo.h"
#include "FWCore/Utilities/interface/InputTag.h"

//#include "MuonBranches.h"
#include "MuonGenAnalyzer.h"
#include "NtupleContent.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "helper.h"
#include "MuonMiniIsolation.h"
#include "JetsBranches.h"

#include "StandAloneMuonBranches.h"
#include "StandAloneNtupleContent.h"

using namespace std;
// using namespace edm;

//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.
class StandAloneMuonMiniAODAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
    public:
        typedef   std::vector<std::pair<pat::Muon, reco::TransientTrack>> RecoTrkAndTransientTrkCollection;
        explicit  StandAloneMuonMiniAODAnalyzer(const edm::ParameterSet&);
        ~StandAloneMuonMiniAODAnalyzer()                                  override;

        static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

    private:
        void beginJob()                                                   override;
        bool HLTaccept(const edm::Event&, 
                NtupleContent&, 
                std::vector<std::string>&);
        void embedTriggerMatching(const edm::Event&,
                edm::Handle<edm::TriggerResults>&,
                const pat::Muon&,
                NtupleContent&,
                std::vector<std::string>&,
                bool,
                const int&);

        void StandAlone_embedTriggerMatching(const edm::Event&,
                edm::Handle<edm::TriggerResults>&,
                const pat::Muon&,
                StandAloneNtupleContent&,
                std::vector<std::string>&,
                bool,
                const int&);

        void analyze(const edm::Event&, const edm::EventSetup&)           override;
        void endJob()                                                     override;

        edm::InputTag                                                     inputTag_;
        edm::EDGetTokenT<LumiInfo>                                        lumiToken_;
        edm::EDGetTokenT<GenEventInfoProduct>                             genEventInfoToken_;
        edm::EDGetTokenT<double>                                          rhoToken_;
        edm::EDGetTokenT<std::vector<PileupSummaryInfo>>                  pileupSummaryToken_;
        edm::EDGetTokenT<reco::BeamSpot>                                  beamSpotToken_;
        edm::EDGetTokenT<std::vector<reco::Vertex>>                       vtxToken_;
        edm::EDGetToken                                                   muonsToken_;
        edm::EDGetTokenT<edm::View<reco::Muon>>                           muonsViewToken_;
        edm::EDGetToken                                                   PFCands_;
        edm::EDGetToken                                                   LostTracks_;
        const edm::ESGetToken<MagneticField, IdealMagneticFieldRecord>    bFieldToken_;
        edm::EDGetToken                                                   SAmuonsToken_;

        edm::EDGetTokenT<edm::TriggerResults>                             trgresultsToken_;
        edm::EDGetTokenT<pat::TriggerObjectStandAloneMatch>               l1MatchesToken_;
        edm::EDGetTokenT<edm::ValueMap<int>>                              l1MatchesQualityToken_;
        edm::EDGetTokenT<edm::ValueMap<float>>                            l1MatchesDeltaRToken_;
        edm::EDGetTokenT<pat::TriggerObjectStandAloneMatch>               l1MatchesByQToken_;
        edm::EDGetTokenT<edm::ValueMap<int>>                              l1MatchesByQQualityToken_;
        edm::EDGetTokenT<edm::ValueMap<float>>                            l1MatchesByQDeltaRToken_;
        edm::EDGetTokenT<edm::View<reco::GenParticle>>                    genToken_;
        edm::EDGetTokenT<double>                                          rhoJetsNC_;
        edm::EDGetToken                                                   jetsToken_;
        edm::EDGetToken                                                   genJetsToken_;

        std::vector<std::string>                                          HLTPaths_;                  // trigger fired
        std::vector<std::string>                                          tagFilters_;                // tag-trigger matching
        std::vector<std::string>                                          probeFilters_;              // probe-trigger matching
        std::vector<std::string>                                          probeSelectorNames_;
        std::vector<unsigned>                                             probeSelectorBits_;

        const unsigned int                                                tagQual_;
        const StringCutObjectSelector<pat::Muon>                          tagSelection_;              // kinematic cuts for tag
        const bool                                                        HighPurity_;
        const StringCutObjectSelector<pat::PackedCandidate>               probeSelection_;            // kinematic cuts for probe
        const StringCutObjectSelector<reco::Track>                        probeSelectionSA_;          // kinematic cuts for SA probe
        const bool                                                        muonOnly_;
        const StringCutObjectSelector<pat::Muon>                          probeMuonSelection_;
        const double                                                      pairMassMin_;
        const double                                                      pairMassMax_;
        const double                                                      pairDz_;
        const bool                                                        RequireVtxCreation_;        // if true skip pairs that 
        // do not create
        // that do not have a vertex
        const double                                                      minSVtxProb_;               // min probability of a vertex to be kept. If < 0 inactive
        const double                                                      maxdz_trk_mu_;
        const double                                                      maxpt_relative_dif_trk_mu_;
        const double                                                      maxdr_trk_mu_;
        const double                                                      minpt_trkSA_;
        const double                                                      maxdz_trk_SAmu_;
        const double                                                      maxpt_relative_dif_trk_SAmu_;
        const double                                                      maxdr_trk_SAmu_;
        const double                                                      maxdr_trk_dsa_;
        const unsigned                                                    momPdgId_;
        const double                                                      genRecoDrMatch_;
        const bool                                                        saveStandAloneTree_;
        const bool                                                        saveTnPTree_;
        const int                                                         debug_;
        PropagateToMuon                                                   prop1_;
        PropagateToMuonSetup                                              propSetup1_;

        edm::Service<TFileService>                                        fs;
        TTree*                                                            t1;
        NtupleContent                                                     nt;

        //SA
        TTree*                                                            StandAlone_t1;
        StandAloneNtupleContent                                           StandAlone_nt;

        std::mt19937                                                      m_random_generator = std::mt19937(37428479);
        const bool                                                        isMC_, isOnlySeeded_, includeJets_; 
        const bool                                                        Zresonance_;
        const std::string                                                 era_;

        // ----------member data ---------------------------
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//

StandAloneMuonMiniAODAnalyzer::StandAloneMuonMiniAODAnalyzer(const edm::ParameterSet& iConfig)
    : // inputs
        inputTag_(iConfig.getParameter<edm::InputTag>("inputTag")), 
        lumiToken_(consumes<LumiInfo>(inputTag_)),
        genEventInfoToken_(consumes<GenEventInfoProduct>(iConfig.getParameter<edm::InputTag>("genEventInfo"))),
        rhoToken_(consumes<double>(iConfig.getParameter<edm::InputTag>("Rho"))),
        pileupSummaryToken_(consumes<std::vector<PileupSummaryInfo>>(iConfig.getParameter<edm::InputTag>("pileupInfo"))),
        beamSpotToken_(consumes<reco::BeamSpot>(iConfig.getParameter<edm::InputTag>("beamSpot"))),
        vtxToken_(consumes<std::vector<reco::Vertex>>(iConfig.getParameter<edm::InputTag>("vertices"))),
        muonsToken_(consumes<std::vector<pat::Muon>>(iConfig.getParameter<edm::InputTag>("muons"))),
        muonsViewToken_(consumes<edm::View<reco::Muon>>(iConfig.getParameter<edm::InputTag>("muons"))),
        PFCands_(consumes<std::vector<pat::PackedCandidate>>(iConfig.getParameter<edm::InputTag>("PFCands"))),
        LostTracks_(consumes<std::vector<pat::PackedCandidate>>(iConfig.getParameter<edm::InputTag>("lostTracks"))),
        bFieldToken_(esConsumes<MagneticField, IdealMagneticFieldRecord>()),
        SAmuonsToken_(consumes<std::vector<pat::PackedCandidate>>(iConfig.getParameter<edm::InputTag>("SAmuons"))),

        trgresultsToken_(consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("triggerResults"))),
        l1MatchesToken_(consumes<pat::TriggerObjectStandAloneMatch>(iConfig.getParameter<edm::InputTag>("l1Matches"))),
        l1MatchesQualityToken_(consumes<edm::ValueMap<int>>(iConfig.getParameter<edm::InputTag>("l1MatchesQuality"))),
        l1MatchesDeltaRToken_(consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("l1MatchesDeltaR"))),
        l1MatchesByQToken_(
                consumes<pat::TriggerObjectStandAloneMatch>(iConfig.getParameter<edm::InputTag>("l1MatchesByQ"))),
        l1MatchesByQQualityToken_(
                consumes<edm::ValueMap<int>>(iConfig.getParameter<edm::InputTag>("l1MatchesByQQuality"))),
        l1MatchesByQDeltaRToken_(
                consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("l1MatchesByQDeltaR"))),
        genToken_(consumes<edm::View<reco::GenParticle>>(iConfig.getParameter<edm::InputTag>("gen"))),
        rhoJetsNC_(consumes<double>(iConfig.getParameter<edm::InputTag>("rhoJetsNC"))),
        jetsToken_(consumes<std::vector<pat::Jet>>(iConfig.getParameter<edm::InputTag>("jets"))),
        genJetsToken_(consumes<std::vector<reco::GenJet>>(iConfig.getParameter<edm::InputTag>("genJets"))),
        HLTPaths_(iConfig.getParameter<std::vector<std::string>>("triggerPaths")),
        tagFilters_(iConfig.getParameter<std::vector<std::string>>("tagFilters")),
        probeFilters_(iConfig.getParameter<std::vector<std::string>>("probeFilters")),
        probeSelectorNames_(iConfig.getParameter<std::vector<std::string>>("probeSelectorNames")),
        probeSelectorBits_(iConfig.getParameter<std::vector<unsigned>>("probeSelectorBits")),
        tagQual_(iConfig.getParameter<unsigned>("tagQuality")),
        tagSelection_(iConfig.getParameter<std::string>("tagSelection")),
        HighPurity_(iConfig.getParameter<bool>("ProbeHPurity")),
        probeSelection_(iConfig.getParameter<std::string>("probeSelection")),
        probeSelectionSA_(iConfig.getParameter<std::string>("probeSelectionSA")),
        muonOnly_(iConfig.getParameter<bool>("muonOnly")),
        probeMuonSelection_(iConfig.getParameter<std::string>("probeMuonSelection")),
        pairMassMin_(iConfig.getParameter<double>("pairMassMin")),
        pairMassMax_(iConfig.getParameter<double>("pairMassMax")),
        pairDz_(iConfig.getParameter<double>("pairDz")),
        RequireVtxCreation_(iConfig.getParameter<bool>("RequireVtxCreation")),
        minSVtxProb_(iConfig.getParameter<double>("minSVtxProb")),
        maxdz_trk_mu_(iConfig.getParameter<double>("maxDzProbeTrkMuon")),
        maxpt_relative_dif_trk_mu_(iConfig.getParameter<double>("maxRelPtProbeTrkMuon")),
        maxdr_trk_mu_(iConfig.getParameter<double>("maxDRProbeTrkMuon")),
        minpt_trkSA_(iConfig.getParameter<double>("minPtTrkSA")),
        maxdz_trk_SAmu_(iConfig.getParameter<double>("maxDzProbeTrkSAMuon")),
        maxpt_relative_dif_trk_SAmu_(iConfig.getParameter<double>("maxRelPtProbeTrkSAMuon")),
        maxdr_trk_SAmu_(iConfig.getParameter<double>("maxDRProbeTrkSAMuon")),
        maxdr_trk_dsa_(iConfig.getParameter<double>("maxDRProbeTrkDSA")),
        momPdgId_(iConfig.getParameter<unsigned>("momPdgId")),
        genRecoDrMatch_(iConfig.getParameter<double>("genRecoDrMatch")),
        saveStandAloneTree_(iConfig.getParameter<bool>("saveStandAloneTree")),
        saveTnPTree_(iConfig.getParameter<bool>("saveTnPTree")),
        //prop1_(iConfig.getParameter<edm::ParameterSet>("propM1")),
        debug_(iConfig.getParameter<int>("debug")),
        propSetup1_(iConfig, consumesCollector()),
        isMC_(iConfig.getParameter<bool>("isMC")),
        isOnlySeeded_(iConfig.getParameter<bool>("isOnlySeeded")),
        includeJets_(iConfig.getParameter<bool>("includeJets")),
        Zresonance_(iConfig.getParameter<bool>("Zresonance")),
        era_(iConfig.getParameter<std::string>("era")) 
{
    //  edm::ParameterSet
    //  runParameters=iConfig.getParameter<edm::ParameterSet>("RunParameters");

    if (probeSelectorNames_.size() != probeSelectorBits_.size()) 
    {
        throw cms::Exception("ParameterError")
            << "length of probeSelectorNames and probeSelectorBits should be identical\n";
    }
}

StandAloneMuonMiniAODAnalyzer::~StandAloneMuonMiniAODAnalyzer() 
{
    // cout << "total " << trg_counter << " fires " << fire_counter << " l3"
    // << l3_counter << endl; do anything here that needs to be done at
    // desctruction time
}

//
// member functions
bool StandAloneMuonMiniAODAnalyzer::HLTaccept(const edm::Event&         iEvent,
        NtupleContent&            nt,
        std::vector<std::string>& HLTPaths) 
{
    edm::Handle<edm::TriggerResults>                trigResults;
    iEvent.getByToken(trgresultsToken_,             trigResults);
    edm::TriggerNames                               trigName;
    trigName                                        = iEvent.triggerNames(*trigResults);
    bool EvtFire                                    = false;
    unsigned int ipath                              = 0;
    for (auto path : HLTPaths) 
    {
        bool TrgFire                                = false;
        for (unsigned int itrg = 0; itrg < trigResults->size(); ++itrg) 
        {
            TString TrigPath                        = trigName.triggerName(itrg);
            if (!trigResults->accept(itrg))         continue;
            if (!TrigPath.Contains(path))           continue;
            EvtFire                                 = true;
            TrgFire                                 = true;
        }
        nt.trigger[ipath]                           = TrgFire;
        ipath++;
    }
    return EvtFire;
}

void StandAloneMuonMiniAODAnalyzer::embedTriggerMatching(const edm::Event&                  iEvent,
        edm::Handle<edm::TriggerResults>&  trigResults,
        const pat::Muon&                   mu,
        NtupleContent&                     nt,
        std::vector<std::string>&          Triggers,
        bool                                isTag,
        const int& debug_ = 0)
{
    for (const auto& trg : Triggers) 
    {
        TString trg_tstr                            = TString(trg);
        bool matched                                = false;
        float matched_pt                            = -99;
        float matched_eta                           = -99;
        float matched_phi                           = -99;
        float matched_dr                            = 99;

        for (auto trigobj : mu.triggerObjectMatches()) 
        {
            trigobj.unpackNamesAndLabels(iEvent,    *trigResults);
            float dR_tmp                            = deltaR(mu.eta(), mu.phi(), trigobj.eta(), trigobj.phi());
            float dptrel_tmp                        = fabs(mu.pt()-trigobj.pt())/(trigobj.pt());
            // check path names
            if (trg_tstr.Contains("HLT_")) 
            {
                for (auto path : trigobj.pathNames(true, true)) 
                {
                    TString path_tstr               = TString(path);
                    //if (path_tstr.Contains(trg_tstr) && dR_tmp < matched_dr && dR_tmp < 0.5 && dptrel_tmp < 10.) 
                    if (path_tstr.Contains(trg_tstr) && dR_tmp < matched_dr) 
                    {
                        matched                     = true;
                        matched_pt                  = trigobj.pt();
                        matched_eta                 = trigobj.eta();
                        matched_phi                 = trigobj.phi();
                        matched_dr                  = dR_tmp;
                    }
                }
            }
            // check filters
            else 
            {
                for (auto filter : trigobj.filterLabels()) 
                {
                    TString filter_tstr              = TString(filter);
                    //if (filter_tstr.Contains(trg_tstr) && dR_tmp < matched_dr && dR_tmp < 0.5 && dptrel_tmp < 10.) 
                    if (filter_tstr.Contains(trg_tstr) && dR_tmp < matched_dr) 
                    {
                        matched                      = true;
                        matched_pt                   = trigobj.pt();
                        matched_eta                  = trigobj.eta();
                        matched_phi                  = trigobj.phi();
                        matched_dr                   = dR_tmp;

                        if (debug_ > 0) {
                            std::cout << "embedTriggerMatching: isTag=" << isTag 
                                << "  filter="                          << trg_tstr 
                                << "  dR="                              << dR_tmp
                                << "  matched="                         << matched 
                                << std::endl;
                        }
                    }
                }
            }
        }
        if (isTag) {
            nt.tag_trg[&trg - &Triggers[0]]         = matched;
            nt.tag_trg_pt[&trg - &Triggers[0]]      = matched_pt;
            nt.tag_trg_eta[&trg - &Triggers[0]]     = matched_eta;
            nt.tag_trg_phi[&trg - &Triggers[0]]     = matched_phi;
            nt.tag_trg_dr[&trg - &Triggers[0]]      = matched_dr;
        } else {
            nt.probe_trg[&trg - &Triggers[0]]       = matched;
            nt.probe_trg_pt[&trg - &Triggers[0]]    = matched_pt;
            nt.probe_trg_eta[&trg - &Triggers[0]]   = matched_eta;
            nt.probe_trg_phi[&trg - &Triggers[0]]   = matched_phi;
            nt.probe_trg_dr[&trg - &Triggers[0]]    = matched_dr;
        }
    }
    return;
}

void StandAloneMuonMiniAODAnalyzer::StandAlone_embedTriggerMatching(const edm::Event&                   iEvent,
        edm::Handle<edm::TriggerResults>&   trigResults,
        const pat::Muon&                    mu,
        StandAloneNtupleContent&            StandAlone_nt,
        std::vector<std::string>&           Triggers,
        bool                                isTag,
        const int& debug_ = 0)
{
    for (const auto& trg : Triggers) 
    {
        TString trg_tstr                                        = TString(trg);
        bool matched                                            = false;
        float matched_pt                                        = -99;
        float matched_eta                                       = -99;
        float matched_phi                                       = -99;
        float matched_dr                                        = 99;
        for (auto trigobj : mu.triggerObjectMatches()) 
        {
            trigobj.unpackNamesAndLabels(iEvent, *trigResults);
            float dR_tmp                                        = deltaR(mu.eta(), mu.phi(), trigobj.eta(), trigobj.phi());
            float dptrel_tmp                                    = fabs(mu.pt()-trigobj.pt())/(trigobj.pt());
            // check path names
            if (trg_tstr.Contains("HLT_")) 
            {
                for (auto path : trigobj.pathNames(true, true)) 
                {
                    TString path_tstr                           = TString(path);
                    if (path_tstr.Contains(trg_tstr) && dR_tmp < matched_dr && dR_tmp < 0.5 && dptrel_tmp < 10.) 
                    {
                        matched                                 = true;
                        matched_pt                              = trigobj.pt();
                        matched_eta                             = trigobj.eta();
                        matched_phi                             = trigobj.phi();
                        matched_dr                              = dR_tmp;
                    }
                }
            }
            // check filters
            else 
            {
                for (auto filter : trigobj.filterLabels()) 
                {
                    TString filter_tstr = TString(filter);
                    if (filter_tstr.Contains(trg_tstr) && dR_tmp < matched_dr && dR_tmp < 0.5 && dptrel_tmp < 10.) 
                    {
                        matched                                  = true;
                        matched_pt                               = trigobj.pt();
                        matched_eta                              = trigobj.eta();
                        matched_phi                              = trigobj.phi();
                        matched_dr                               = dR_tmp;
                        if (debug_ > 0) 
                        {
                            std::cout   << "StandAlone_embedTriggerMatching: isTag="    << isTag 
                                << "  filter="                                  << trg_tstr
                                << "  dR="                                      << dR_tmp 
                                << "  matched="                                 << matched 
                                << std::endl;
                        }
                    }
                }
            }
        }
        if (isTag) 
        {
            StandAlone_nt.tag_trg[&trg - &Triggers[0]]          = matched;
            StandAlone_nt.tag_trg_pt[&trg - &Triggers[0]]       = matched_pt;
            StandAlone_nt.tag_trg_eta[&trg - &Triggers[0]]      = matched_eta;
            StandAlone_nt.tag_trg_phi[&trg - &Triggers[0]]      = matched_phi;
            StandAlone_nt.tag_trg_dr[&trg - &Triggers[0]]       = matched_dr;
        }
        else 
        {
            StandAlone_nt.probe_trg[&trg - &Triggers[0]]        = matched;
            StandAlone_nt.probe_trg_pt[&trg - &Triggers[0]]     = matched_pt;
            StandAlone_nt.probe_trg_eta[&trg - &Triggers[0]]    = matched_eta;
            StandAlone_nt.probe_trg_phi[&trg - &Triggers[0]]    = matched_phi;
            StandAlone_nt.probe_trg_dr[&trg - &Triggers[0]]     = matched_dr;
        }
    }
    return;
}
// ------------ method called for each event  ------------

void StandAloneMuonMiniAODAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) 
{
    using namespace std;
    using namespace edm;
    using namespace reco;
    using namespace trigger;

    prop1_ = propSetup1_.init(iSetup);

    // Get Data
    edm::Handle<reco::BeamSpot>                         theBeamSpot;
    iEvent.getByToken(beamSpotToken_, theBeamSpot);
    edm::Handle<reco::VertexCollection>                 vertices;
    iEvent.getByToken(vtxToken_, vertices);

    // Skip evts if there are no vertices
    if (vertices->empty())                              return;

    edm::Handle<std::vector<pat::PackedCandidate>>      SAmuons;
    iEvent.getByToken(SAmuonsToken_, SAmuons);

    edm::Handle<std::vector<pat::Muon>>                 muons;
    iEvent.getByToken(muonsToken_, muons);
    edm::Handle<edm::View<reco::Muon>>                  muonsView;
    iEvent.getByToken(muonsViewToken_, muonsView);
    edm::Handle<std::vector<pat::PackedCandidate>>      lostTracks;
    iEvent.getByToken(LostTracks_, lostTracks);
    //edm::ESHandle<MagneticField> bField;
    //iSetup.get<IdealMagneticFieldRecord>().get(bField);
    edm::ESHandle<MagneticField> bField;
    bField = iSetup.getHandle(bFieldToken_);

    //const auto& bField = iSetup.getData(bFieldToken_);
    // mini isolation
    edm::Handle<std::vector<pat::PackedCandidate>>      pfcands;
    iEvent.getByToken(PFCands_, pfcands);
    edm::Handle<double>                                 rhoJetsNC;
    iEvent.getByToken(rhoJetsNC_, rhoJetsNC);
    // jets
    edm::Handle<std::vector<pat::Jet>>                  jets;
    iEvent.getByToken(jetsToken_, jets);
    edm::Handle<std::vector<reco::GenJet>>              genJets;
    iEvent.getByToken(genJetsToken_, genJets);
    JME::JetResolution                                  resolution;
    //resolution = JME::JetResolution::get(iSetup, "AK4PFchs_pt");
    JME::JetResolutionScaleFactor                       resolution_sf;
    //resolution_sf = JME::JetResolutionScaleFactor::get(iSetup, "AK4PFchs");

    edm::Handle<edm::TriggerResults>                    trigResults;
    iEvent.getByToken(trgresultsToken_, trigResults);
    edm::Handle<pat::TriggerObjectStandAloneMatch>      l1Matches;
    iEvent.getByToken(l1MatchesToken_, l1Matches);
    edm::Handle<edm::ValueMap<int>>                     l1Qualities;
    iEvent.getByToken(l1MatchesQualityToken_, l1Qualities);
    edm::Handle<edm::ValueMap<float>>                   l1Drs;
    iEvent.getByToken(l1MatchesDeltaRToken_, l1Drs);
    edm::Handle<pat::TriggerObjectStandAloneMatch>      l1MatchesByQ;
    iEvent.getByToken(l1MatchesByQToken_, l1MatchesByQ);
    edm::Handle<edm::ValueMap<int>>                     l1QualitiesByQ;
    iEvent.getByToken(l1MatchesByQQualityToken_, l1QualitiesByQ);
    edm::Handle<edm::ValueMap<float>>                   l1DrsByQ;
    iEvent.getByToken(l1MatchesByQDeltaRToken_, l1DrsByQ);

    //SA
    StandAlone_nt.ClearBranches();

    // Information about run
    nt.ClearBranches();
    nt.run                                              = iEvent.id().run();
    nt.ls                                               = iEvent.luminosityBlock();
    nt.event                                            = iEvent.id().event();
    nt.fromFullAOD                                      = false;
    nt.BSpot_x                                          = theBeamSpot->x0();
    nt.BSpot_y                                          = theBeamSpot->y0();
    nt.BSpot_z                                          = theBeamSpot->z0();
    nt.nvertices                                        = vertices->size();

    // Gen weights, sim info
    if (!iEvent.isRealData()) 
    {
        edm::Handle<GenEventInfoProduct>                genEventInfoHandle;
        iEvent.getByToken(genEventInfoToken_,           genEventInfoHandle);
        nt.genWeight                                    = (float) genEventInfoHandle->weight();
    } else   // data
    {
        nt.genWeight                                    = (float) 1.;
    }

    // Pileup information
    edm::Handle<double>                                 rhoHandle;
    iEvent.getByToken(rhoToken_, rhoHandle);
    nt.Rho                                              = (double) *rhoHandle;

    float trueNumInteractions                           = -1;
    int puNumInteractions                               = -1;
    if (isMC_) 
    {
        edm::Handle<std::vector<PileupSummaryInfo>>     PupInfo;
        iEvent.getByToken(pileupSummaryToken_, PupInfo);

        for (auto PVI : *PupInfo) 
        {
            int BX                                      = PVI.getBunchCrossing();
            if (BX == 0) 
            {
                trueNumInteractions                     = PVI.getTrueNumInteractions();
                puNumInteractions                       = PVI.getPU_NumInteractions();
                continue;
            }
        }
    }

    nt.trueNumInteractions                              = (float) trueNumInteractions;
    nt.puNumInteractions                                = (float) puNumInteractions;

    if (debug_ > 0)                                     std::cout << "New Evt " << nt.run << std::endl;

    reco::TrackBase::Point                              vertex_point;
    bool goodVtx                                        = false;
    reco::Vertex const*                                 pv;
    for (const reco::Vertex& vtx : *vertices) 
    {
        if ((vtx.isFake() || !vtx.isValid()) 
                || (vtx.ndof() <= 4) 
                || abs(vtx.z()) > 25 
                || ((vtx.position()).Rho() > 2 ))       continue;

        nt.pv_x                                         = vtx.x();
        nt.pv_y                                         = vtx.y();
        nt.pv_z                                         = vtx.z();

        StandAlone_nt.pv_x                              = vtx.x();
        StandAlone_nt.pv_y                              = vtx.y();
        StandAlone_nt.pv_z                              = vtx.z();
        goodVtx                                         = true;
        pv                                              = &vtx;
        break;
    }
    if (!goodVtx)                                       return;  // skipping in absence of good vertex
    
    vertex_point.SetCoordinates(nt.pv_x, nt.pv_y, nt.pv_z);

    // check if path fired, if so save hlt muon
    if (!HLTaccept(iEvent, nt, HLTPaths_))              return;

    // Gen information
    MuonGenAnalyzer                                     genmu;
    std::vector<unsigned>                               matched_muon_idx;
    if (!iEvent.isRealData()) 
    {
        genmu.SetInputs(iEvent, genToken_, momPdgId_);
        genmu.FillNtuple(nt);

        auto reco_match_genmu1                          = MatchReco<pat::Muon>(*muons, nt.genmu1_eta, nt.genmu1_phi, nt.genmu1_charge, genRecoDrMatch_);
        auto reco_match_genmu2                          = MatchReco<pat::Muon>(*muons, nt.genmu2_eta, nt.genmu2_phi, nt.genmu2_charge, genRecoDrMatch_);
      
        if (reco_match_genmu1.first)                    matched_muon_idx.push_back(reco_match_genmu1.second);
        if (reco_match_genmu2.first)                    matched_muon_idx.push_back(reco_match_genmu2.second);
    }

    // Find triggering muon
    int tag_muon_index                                  = 0;
    std::vector<unsigned>                               tag_muon_map;         // idx of tag muon in muons
    RecoTrkAndTransientTrkCollection                    tag_muon_ttrack;
    std::vector<bool>                                   genmatched_tag;
    for (const pat::Muon& mu : *muons) 
    {
        tag_muon_index++;
        if (mu.selectors() != 0)                                            // Only 9_4_X and later have selector bits
        {  
            //if (!mu.passed(pow(2, tagQual_)))           continue;
            if (!mu.passed(static_cast<uint64_t>(pow(2, tagQual_)))) continue;
        } else                                                              // For 2016, assume loose ID on the tag (can be tightened at spark level)
        {
            if (!muon::isLooseMuon(mu))                 continue;
        }
        bool fired                                      = false;
        for (const std::string& path : HLTPaths_) 
        {
            char                                        cstr[(path + "*").size() + 1];
            strcpy(                                     cstr, (path + "*").c_str());
            
            if (!mu.triggered(cstr))                    continue;
            fired                                       = true;
            break;
        }
        if (!fired)                                     continue;
        if (!tagSelection_(mu))                         continue;

        tag_muon_ttrack.emplace_back(                   std::make_pair(mu, reco::TransientTrack(*mu.bestTrack(), &(*bField))));
        tag_muon_map.push_back(&mu - &muons->at(0));
        if (std::find(matched_muon_idx.begin(), matched_muon_idx.end(), &mu - &muons->at(0)) != matched_muon_idx.end())
            genmatched_tag.push_back(   true);
        else
            genmatched_tag.push_back(   false);
    }
    if (tag_muon_ttrack.empty())                        return;

    nt.nmuons                                           = muons->size();
    nt.ntag                                             = tag_muon_ttrack.size();


    if (debug_ > 0)                                     std::cout << "Tag muons: " << tag_muon_ttrack.size() << std::endl;

    // Add Lost Tracks to Packed cands
    //      --> added nocuts tracks vector for matching to SA muons
    std::vector<reco::Track>                            tracks;
    std::vector<reco::Track>                            nocut_tracks;
    for (const auto& container : {pfcands, lostTracks}) 
    {
        for (const pat::PackedCandidate& trk : *container) 
        {
            if (!trk.hasTrackDetails())                 continue;
            nocut_tracks.push_back(*trk.bestTrack());
            if (!probeSelection_(trk))                  continue;
            if (HighPurity_ && !trk.trackHighPurity())  continue;
            tracks.emplace_back(*trk.bestTrack());
        }
    }
    
    std::vector<unsigned> matched_track_idx;
    if (!iEvent.isRealData()) {
        auto reco_match_genmu1 =                        MatchReco<reco::Track>( tracks, 
                                                                                nt.genmu1_eta, 
                                                                                nt.genmu1_phi, 
                                                                                nt.genmu1_charge, 
                                                                                genRecoDrMatch_);
        auto reco_match_genmu2 =                        MatchReco<reco::Track>( tracks, 
                                                                                nt.genmu2_eta, 
                                                                                nt.genmu2_phi, 
                                                                                nt.genmu2_charge, 
                                                                                genRecoDrMatch_);
        
        if (reco_match_genmu1.first)                    matched_track_idx.push_back(reco_match_genmu1.second);
        if (reco_match_genmu2.first)                    matched_track_idx.push_back(reco_match_genmu2.second);
    }
    // ----------------------------------------------------------------------------------------------------------------------------------------------
    // probe track mapping with muon object
    std::pair<std::vector<unsigned>, std::vector<unsigned>> trk_muon_map;
    for (const auto& mu : *muons) 
    {
        if (muonOnly_ && !probeMuonSelection_(mu))      continue;
        float minDR                                     = 1000;
        unsigned int                                    idx_trk;
        if (debug_ > 1)  
        {
            std::cout   << "New trk-muon map entry pt " << mu.pt() 
                << " eta "                      << mu.eta() 
                << " phi "                      << mu.phi() 
                << std::endl;
        }
        for (const auto& trk : tracks) 
        {
            if (mu.charge() != trk.charge())            continue;
            if ((trk.pt() <= minpt_trkSA_) 
                    && (abs(trk.eta()) <= 1. || trk.p() <= 2.))continue;
            if (fabs(mu.vz() - trk.vz()) > maxdz_trk_mu_ 
                    && maxdz_trk_mu_ > 0)               continue;
            // if (fabs(mu.eta() - trk.eta()) > 0.4)    continue;
            if (fabs(mu.eta() - trk.eta()) > 0.3)       continue;
            if (fabs(mu.pt() - trk.pt()) / mu.pt() > maxpt_relative_dif_trk_mu_ 
                    && maxpt_relative_dif_trk_mu_ > 0)  continue;

            float DR                                    = deltaR(mu.eta(), mu.phi(), trk.eta(), trk.phi());

            if (debug_ > 1)
            {
                std::cout   << "   DR "     << DR 
                    << "mu-eta "    << mu.eta() 
                    << "mu-phi "    << mu.phi() 
                    << " trk-eta "  << trk.eta() 
                    << "trk-phi  "  << trk.phi()
                    << std::endl;
            }

            if (minDR < DR)                             continue;
            minDR                                       = DR;
            idx_trk                                     = &trk - &tracks[0];
        }
        if (minDR > maxdr_trk_mu_)                      continue;
        trk_muon_map.first.push_back(idx_trk);
        trk_muon_map.second.push_back(&mu - &muons->at(0));
    }
    if (debug_ > 0)                                     std::cout << "Matched trk-mu " << trk_muon_map.first.size() << std::endl;

    //SA probe track mapping with StandAlone muon object
    std::pair<std::vector<unsigned>, std::vector<unsigned>>         SA_trk_muon_map;
    for (const auto& tmp_mu : *muons) 
    {
        // check if Muon is classified as Standalone Muon
        if (!tmp_mu.isStandAloneMuon())                             continue;
        if (!((*tmp_mu.standAloneMuon()).numberOfValidHits() > 0.)) continue;
        
        // check if basic probe muon requirements are met
        if (muonOnly_ && !probeMuonSelection_(tmp_mu))              continue;
        
        const reco::Track mu                                        = *tmp_mu.standAloneMuon();
        float minDR                                                 = 1000;
        unsigned int                                                idx_trk;
        if (debug_ > 0)
        {
            std::cout   << "Standalone New trk-muon map entry pt "  << mu.pt() 
                        << " eta "                                  << mu.eta() 
                        << " phi "                                  << mu.phi()
                        << " vz "                                   << mu.vz() 
                        << std::endl;
        }

        //for (const reco::Track& trk : *tracks) 
        for (const auto& trk : tracks) 
        {
            if (debug_ > 1)
            {
                std::cout   << "Trk New trk-muon map entry pt "     << trk.pt() 
                            << " eta "                              << trk.eta() 
                            << " phi "                              << trk.phi()
                            << " vz "                               << trk.vz() 
                            << std::endl;
            }

            if (mu.charge() != trk.charge())                        continue;
            if ((trk.pt() <= minpt_trkSA_) 
                    && (abs(trk.eta()) <= 1. || trk.p() <= 2.))     continue;
            if (fabs(mu.vz() - trk.vz()) >= maxdz_trk_SAmu_ 
                    && maxdz_trk_SAmu_ > 0)                         continue;
            if (fabs(mu.eta() - trk.eta()) > 0.3)                   continue;
            if (fabs(mu.pt() - trk.pt()) / trk.pt() > maxpt_relative_dif_trk_SAmu_ 
                    && maxpt_relative_dif_trk_SAmu_)                continue;

            float DR                                                = deltaR(mu.eta(), mu.phi(), trk.eta(), trk.phi());
            if (minDR < DR)                                         continue;
            if (debug_ > 1)
            {
                std::cout   << "   DR "                             << DR 
                            << " mu-eta "                           << mu.eta() 
                            << " mu-phi "                           << mu.phi() 
                            << " trk-eta "                          << trk.eta() 
                            << " trk-phi "                          << trk.phi()
                            << std::endl;
            }
            minDR                                                 = DR;
            //idx_trk                                               = &trk - &tracks->at(0);
            idx_trk                                               = &trk - &tracks[0];
        }

        if (minDR > maxdr_trk_SAmu_)                                continue;

        SA_trk_muon_map.first.push_back(idx_trk);
        SA_trk_muon_map.second.push_back(&tmp_mu - &muons->at(0));
    }
    if (debug_ > 0)                                                  std::cout << "Matched Standalone trk-mu " << SA_trk_muon_map.first.size() << std::endl;

    // ----------------------------------------------------------------------------------------------------------------------------------------------
    // Muon collection for jet cleaning
    /*
       std::vector<reco::Muon> muForJetCleaning;
       for (const auto& mu : *muons) {
       if (!muon::isLooseMuon(mu))
       continue;
       muForJetCleaning.push_back(mu);
       }
       */
    // ----------------------------------------------------------------------------------------------------------------------------------------------

    // MN loop to match standalone muons to closest trk in DR and associated trk if exists
    std::pair<std::vector<unsigned>, std::vector<unsigned>> trk_SAmuon_map;
    std::pair<std::vector<unsigned>, std::vector<unsigned>> associatedtrk_muon_map;

    if (debug_ > 0) 
    {
        std::cout << std::endl << "NEW EVENT"   << std::endl            << std::endl;
        std::cout << "New Evt "                 << nt.event             << std::endl;
        std::cout << "Run "                     << nt.run               << std::endl;
        std::cout << "Total of tag muons: "     << tag_muon_ttrack.size()   << std::endl;
    }

    // loop to compute numerator of tracking efficiency + fake rate
    for (const auto& mu : *muons) 
    {
        // check if Muon is classified as Standalone Muon
        if (!mu.isStandAloneMuon())                         continue;
        
        // check if basic probe muon requirements are met
        if (muonOnly_ && !probeMuonSelection_(mu))          continue;
       
        // associate tracker track to standalone muon
        const reco::Track SA_mu                             = *mu.standAloneMuon();         // for main standAloneMuon efficiency study
        const reco::Track trk_mu                            = *mu.standAloneMuon();         // for fakerate study
       
        // initiate variables for main standAlone Muon study
        float minDR                                         = 1000;
        unsigned int                                        idx_trk;
        unsigned int                                        idx_tag_temp;
        
        // initiate variables for fakerate study
        float minDR_assoc                                   = 1000;
        unsigned int                                        idx_associatedtrk;
        unsigned int                                        idx_tag_assoc = 0;
        bool hasAssociatedTrkMatch                          = false;
        bool isAssoc                                        = false;
        bool isZmass                                        = false;
        bool isJPsimass                                     = false;

        for (const auto& trk : tracks) 
        {
            isZmass                                         = false;
            isJPsimass                                      = false;
            isAssoc                                         = false;

            // calculate tag-track associations
            if((trk.pt() <= minpt_trkSA_) 
                    && (abs(trk.eta()) <= 1. || trk.p() <= 2.))     continue;       // requirement on track

            // check displacement of tag from tracks, we want to do the matching only with no displaced tracks
            for (const auto& tag : tag_muon_ttrack) 
            {
                if (fabs(tag.first.vz() - trk.vz()) < maxdz_trk_SAmu_ ) { isAssoc = true; idx_tag_assoc = &tag - &tag_muon_ttrack[0]; break; }
            } 

            for (const auto& tag : tag_muon_ttrack) 
            {
                float mass_tagtrack                                 = DimuonMass(tag.first.pt(), tag.first.eta(), tag.first.phi(), trk.pt(), trk.eta(), trk.phi());
                idx_tag_temp                                        = &tag - &tag_muon_ttrack[0];

                if (mass_tagtrack >= 40     && mass_tagtrack <= 200 && idx_tag_temp == idx_tag_assoc) { isZmass     = true; break; }
                if (mass_tagtrack >= 1.5    && mass_tagtrack <= 6.  && idx_tag_temp == idx_tag_assoc) { isJPsimass  = true; break; }
            }

            bool charge_match                                       = trk_mu.charge() == trk.charge();
            bool pt_match                                           = ((fabs(trk_mu.pt() - trk.pt())/trk.pt() < maxpt_relative_dif_trk_SAmu_)
                                                                        && (maxpt_relative_dif_trk_SAmu_ > 0));
            bool DeltaR_match                                       = deltaR(trk_mu.eta(), trk_mu.phi(), trk.eta(), trk.phi()) < maxdr_trk_SAmu_;
            bool DeltaEta_match                                     = fabs(trk_mu.eta() - trk.eta()) < 0.3;

            // Fill map for fakerate study
            if(charge_match 
                    && pt_match 
                    && DeltaR_match 
                    && isAssoc 
                    && DeltaEta_match 
                    && ((!(isZmass) && Zresonance_) || (!(isJPsimass) && !Zresonance_)))
            {

                if(minDR_assoc >= deltaR(trk_mu.eta(), trk_mu.phi(), trk.eta(), trk.phi()))
                {
                    minDR_assoc                                     = deltaR(trk_mu.eta(), trk_mu.phi(), trk.eta(), trk.phi());
                }
                
                bool isTrackerOnlyseeded                            = (trk.isAlgoInMask(trk.initialStep) 
                                                                       || trk.isAlgoInMask(trk.lowPtTripletStep) 
                                                                       || trk.isAlgoInMask(trk.pixelPairStep) 
                                                                       || trk.isAlgoInMask(trk.detachedTripletStep) 
                                                                       || trk.isAlgoInMask(trk.mixedTripletStep) 
                                                                       || trk.isAlgoInMask(trk.pixelLessStep) 
                                                                       || trk.isAlgoInMask(trk.tobTecStep) 
                                                                       || trk.isAlgoInMask(trk.jetCoreRegionalStep) 
                                                                       || trk.isAlgoInMask(trk.lowPtQuadStep) 
                                                                       || trk.isAlgoInMask(trk.highPtTripletStep) 
                                                                       || trk.isAlgoInMask(trk.detachedQuadStep)
                                                                    );
                
                if(!isTrackerOnlyseeded && isOnlySeeded_)           continue;
                
                if(minDR_assoc >= deltaR(trk_mu.eta(), trk_mu.phi(), trk.eta(), trk.phi()))
                {
                    idx_associatedtrk                               = &trk - &tracks[0];
                    hasAssociatedTrkMatch                           = true;
                    minDR_assoc                                     = deltaR(trk_mu.eta(), trk_mu.phi(), trk.eta(), trk.phi());
                }
                if(minDR_assoc < maxdr_trk_SAmu_ 
                    && hasAssociatedTrkMatch 
                    && (&trk == &tracks.back())) 
                    {
                        associatedtrk_muon_map.first.push_back(     idx_associatedtrk);                  // stora indice traccia
                        associatedtrk_muon_map.second.push_back(    &mu - &muons->at(0));                // stora indice muone
                        if (debug_ > 0) 
                        {
                            std::cout << " Saving a matching fake in the map! "     << std::endl;
                            std::cout << " probe index: "   << &mu - &muons->at(0)  << std::endl;
                            std::cout << " track index: "   << idx_associatedtrk    << std::endl;
                        }
                    }
            }

            // Fill map for main standAloneMuon study
            if (SA_mu.charge() != trk.charge())                     continue;
            
            bool isTrackeronlyseeded                                = (trk.isAlgoInMask(trk.initialStep) 
                                                                       || trk.isAlgoInMask(trk.lowPtTripletStep) 
                                                                       || trk.isAlgoInMask(trk.pixelPairStep) 
                                                                       || trk.isAlgoInMask(trk.detachedTripletStep) 
                                                                       || trk.isAlgoInMask(trk.mixedTripletStep) 
                                                                       || trk.isAlgoInMask(trk.pixelLessStep) 
                                                                       || trk.isAlgoInMask(trk.tobTecStep) 
                                                                       || trk.isAlgoInMask(trk.jetCoreRegionalStep) 
                                                                       || trk.isAlgoInMask(trk.lowPtQuadStep) 
                                                                       || trk.isAlgoInMask(trk.highPtTripletStep) 
                                                                       || trk.isAlgoInMask(trk.detachedQuadStep)
                                                                    );
            
            if(!isTrackeronlyseeded && isOnlySeeded_)               continue;
            
            if((trk.pt() <= minpt_trkSA_) 
                    && (abs(trk.eta()) <= 1. || trk.p() <= 2.))     continue;
            if(!isAssoc)                                            continue;
            if(fabs(SA_mu.eta() - trk.eta()) > 0.3)                 continue;
            if(fabs(SA_mu.pt() - trk.pt()) / trk.pt() > maxpt_relative_dif_trk_SAmu_ 
                    && maxpt_relative_dif_trk_SAmu_ > 0)            continue;
            float DR                                                = deltaR(SA_mu.eta(), SA_mu.phi(), trk.eta(), trk.phi());
            if (debug_ > 0) 
            {
                std::cout   << "   DR "         << DR 
                    << " SA_mu-eta "    << SA_mu.eta() 
                    << " SA_mu-phi "    << SA_mu.phi() 
                    << " trk-eta "      << trk.eta() 
                    << " trk-phi "      << trk.phi() 
                    << std::endl;
            }

            if(minDR < DR)                                          continue;

            minDR                                                   = DR;
            idx_trk                                                 = &trk - &tracks[0];

        }
        
        if(minDR > maxdr_trk_SAmu_)                                 continue;
        trk_SAmuon_map.first.push_back(                             idx_trk);                          //storage of track index
        
        trk_SAmuon_map.second.push_back(                            &mu - &muons->at(0));             //storage of muon index

        if (debug_ > 0) 
        {
            std::cout << " Saving a matching in the map! "          << std::endl;
            std::cout << " probe index: "   << &mu - &muons->at(0)  << std::endl;
            std::cout << " track index: "   <<  idx_trk             << std::endl;
        }
    }
    // Muon collection for jet cleaning
    std::vector<reco::Muon>                                         muForJetCleaning;
    for (const auto& mu : *muons) 
    {
        if (!muon::isLooseMuon(mu))                                 continue;
        muForJetCleaning.push_back(mu);
    }
    sort(muForJetCleaning.begin(), muForJetCleaning.end(), [](const auto& l, const auto& r) { return l.pt() > r.pt(); });

    std::vector<reco::PFJet>                                        corrJets;
    if (includeJets_) 
    {
        for (const auto& jet : *jets) 
        {
            if (CrossClean(jet, muForJetCleaning))                  continue;

            std::unique_ptr<pat::Jet>                               corrJet(jet.clone());
            double jec                                              = 1.0; 
            corrJet->scaleEnergy(jec);
            // JER
            double smearFactor                                      = 1.0;

            if (isMC_) 
            {

                for (const auto& genJet : *genJets) 
                {
                    StandAlone_nt.genJets_pt.push_back(genJet.pt());
                    StandAlone_nt.genJets_eta.push_back(genJet.eta());
                    StandAlone_nt.genJets_phi.push_back(genJet.phi());
                    StandAlone_nt.genJets_mass.push_back(genJet.mass());
                }

                double jet_resolution                               = resolution.getResolution( {{JME::Binning::JetPt, corrJet->pt()},
                                                                                                {JME::Binning::JetEta, corrJet->eta()},
                                                                                                {JME::Binning::Rho, *rhoHandle}});
                double jer_sf                                       = resolution_sf.getScaleFactor( {{JME::Binning::JetPt, corrJet->pt()},
                                                                                                    {JME::Binning::JetEta, corrJet->eta()},
                                                                                                    {JME::Binning::Rho, *rhoHandle}},
                                                                                                    Variation::NOMINAL);
                // gen matching
                double min_dR                                       = std::numeric_limits<double>::infinity();
                const reco::GenJet* matched_genJet                  = nullptr;
                for (const auto& genJet : *genJets) 
                {
                    double dR                                       = deltaR(genJet, *corrJet);

                    if (dR > min_dR)                                continue;
                    if (dR >= 0.2)                                  continue;

                    min_dR                                          = dR;
                    matched_genJet                                  = &genJet;
                }
                if (matched_genJet) 
                {
                    double dPt                                      = corrJet->pt() - matched_genJet->pt();
                    smearFactor                                     = 1 + (jer_sf - 1.) * dPt / corrJet->pt();
                } else if (jer_sf > 1) 
                {
                    double sigma                                    = jet_resolution * std::sqrt(jer_sf * jer_sf - 1);
                    std::normal_distribution<>                      d(0, sigma);
                    smearFactor                                     = 1. + d(m_random_generator);
                }
                if (corrJet->pt() * smearFactor < 0.01) 
                {
                    smearFactor                                     = 0.01 / corrJet->energy();
                }
            }
            corrJet->scaleEnergy(   smearFactor);
            //corrJets.push_back(     *corrJet);
            FillJetBranches(jet, *corrJet, StandAlone_nt, era_);
            
            float deepCSVprobb      = -9999.,                       deepCSVprobbb       = -9999.;
            float deepFlavprobb     = -9999.,                       deepFlavprobbb      = -9999.;
            float deepFlavproblepb  = -9999.;
            for (const auto& pair : jet.getPairDiscri()) 
            {
                if (pair.first == "pfDeepCSVJetTags:probb")         deepCSVprobb        = pair.second;
                if (pair.first == "pfDeepCSVJetTags:probbb")        deepCSVprobbb       = pair.second;
                if (pair.first == "pfDeepFlavourJetTags:probb")     deepFlavprobb       = pair.second;
                if (pair.first == "pfDeepFlavourJetTags:probbb")    deepFlavprobbb      = pair.second;
                if (pair.first == "pfDeepFlavourJetTags:problepb")  deepFlavproblepb    = pair.second;
            }
            if (deepCSVprobb != -9999. && deepCSVprobbb != -9999.) 
            {
                //nt.jets_bTag_deepCSV.push_back(deepCSVprobb + deepCSVprobbb);
                StandAlone_nt.jets_bTag_deepCSV.push_back(deepCSVprobb + deepCSVprobbb);
            } 
            else 
            {
                //nt.jets_bTag_deepCSV.push_back(-9999.);
                StandAlone_nt.jets_bTag_deepCSV.push_back(-9999.);
            }
            if (deepFlavprobb != -9999. && deepFlavprobbb != -9999. && deepFlavproblepb != -9999.) 
            {
                //nt.jets_bTag_deepFlav.push_back(deepFlavprobb + deepFlavprobbb + deepFlavproblepb);
                StandAlone_nt.jets_bTag_deepFlav.push_back(deepFlavprobb + deepFlavprobbb + deepFlavproblepb);
            } 
            else 
            {
                //nt.jets_bTag_deepFlav.push_back(-9999.);
                StandAlone_nt.jets_bTag_deepFlav.push_back(-9999.);
            }
        }
    }

    // -------------------------------------------------------------------------------------
    // Final pair selection
    StandAloneFillEventInfo(nt, StandAlone_nt, HLTPaths_);

    // run over tracks and probes once prior to filling tree to determine ordering of pairs
    // this is necessary to use tag-probe pair with highest "quality" later on in spark_tnp
    // Also calculate the total number of pairs per event to include in ntuple

    // assign sorted vtx indices to ranking
    using t_pair_prob                       = std::pair<float, std::pair<int, int>>;
    std::vector<t_pair_prob>                pair_vtx_probs;
    std::vector<t_pair_prob>                pair_dPhi_muons;
    std::vector<t_pair_prob>                pair_dz_PV_SV;
    std::vector<t_pair_prob>                pair_dM_Z_Mmumu;       
    std::sort(pair_dz_PV_SV.begin(),        pair_dz_PV_SV.end(),        std::greater<t_pair_prob>());           // inverse sort
    std::sort(pair_dM_Z_Mmumu.begin(),      pair_dM_Z_Mmumu.end(),      std::greater<t_pair_prob>());           // inverse sort

    std::vector<t_pair_prob>                SA_pair_vtx_probs;
    std::vector<t_pair_prob>                SA_pair_dPhi_muons;
    std::vector<t_pair_prob>                SA_pair_dz_PV_SV;
    std::vector<t_pair_prob>                SA_pair_dM_Z_Mmumu;       
    std::sort(SA_pair_dz_PV_SV.begin(),     SA_pair_dz_PV_SV.end(),     std::greater<t_pair_prob>());           // inverse sort
    std::sort(SA_pair_dM_Z_Mmumu.begin(),   SA_pair_dM_Z_Mmumu.end(),   std::greater<t_pair_prob>());           // inverse sort
    
    // loop over tags
    for (const auto& tag : tag_muon_ttrack) 
    {
        auto tag_idx                                                = &tag - &tag_muon_ttrack[0];
        
        // loop over probes
        //for (const reco::Track& probe : *tracks) 
        for (const auto& probe : tracks) 
        {
            auto probe_idx                                          = &probe - &tracks[0];
            
            // apply cuts on pairs
            if (tag.first.charge() == probe.charge())               continue;
            if (fabs(tag.first.vz() - probe.vz()) > pairDz_ 
                    && pairDz_ > 0)                                 continue;
            
            float mass                                              = DimuonMass(tag.first.pt(), tag.first.eta(), tag.first.phi(), probe.pt(), probe.eta(), probe.phi());
            if (mass < pairMassMin_ || mass > pairMassMax_)         continue;
            
            // compute vtx
            std::vector<reco::TransientTrack> trk_pair              = {tag.second, reco::TransientTrack(probe, &(*bField))};
            KlFitter vtx(trk_pair);
            
            // check if fitting is succesfull
            if (RequireVtxCreation_ && !vtx.status())               continue;
            if (minSVtxProb_ > 0 && vtx.prob() < minSVtxProb_)      continue;
           
            // calculate delta-phi between both muons
            float dPhi_muons                                        = reco::deltaPhi(tag.first.phi(), probe.phi());
            
            // calculate the mass displacement wrt the Z boson mass
            math::PtEtaPhiMLorentzVector                            mu1(tag.first.pt(), tag.first.eta(),    tag.first.phi(),    MU_MASS);
            math::PtEtaPhiMLorentzVector                            mu2(probe.pt(),     probe.eta(),        probe.phi(),        MU_MASS);
            float dM_Z_Mmumu                                        = abs(91.2 - (mu1 + mu2).mass());
            
            // save quantities to ordered heap
            auto pair_idx                                           = std::make_pair(tag_idx, probe_idx);

            
            pair_vtx_probs.emplace_back(    std::make_pair(vtx.prob(), pair_idx));
            //pair_dz_PV_SV.emplace_back(     std::make_pair(vtx.dz_PV_SV(nt.pv_z), pair_idx));
            pair_dPhi_muons.emplace_back(   std::make_pair(dPhi_muons, pair_idx));
            pair_dM_Z_Mmumu.emplace_back(   std::make_pair(dM_Z_Mmumu, pair_idx));
            
        }                   // end probe loop
        
        //for (const reco::Muon& tmp_probe : *muons) 
        for (const auto& tmp_probe : *muons) 
        {
            auto probe_idx                                          = &tmp_probe - &muons->at(0);
            
            // check if muon is a standAloneMuon
            if (!tmp_probe.isStandAloneMuon())                              continue;
            if (!((*tmp_probe.standAloneMuon()).numberOfValidHits() > 0.))  continue;
            
            // Associate a tracker track with the standalone muon object
            const reco::Track probe                                 = *tmp_probe.standAloneMuon();
            
            // apply cuts on probe
            if (!probeSelectionSA_(probe))                          continue;
            
            // apply cuts on pairs; selected will be saved
            if (tag.first.charge() == probe.charge())               continue;

            float mass                                              = DimuonMass(tag.first.pt(), tag.first.eta(), tag.first.phi(), probe.pt(), probe.eta(), probe.phi());
            if (mass < pairMassMin_ || mass > pairMassMax_)         continue;
           
            // fit vertex to tag-probe pair
            std::vector<reco::TransientTrack> trk_pair              = {tag.second, reco::TransientTrack(probe, &(*bField))};
            KlFitter vtx(trk_pair);
            
            // calculate delta-phi of tag-probe pair
            float dPhi_muons                                        = reco::deltaPhi(tag.first.phi(), probe.phi());
            
            // calculate displacement wrt the Z-mass 
            math::PtEtaPhiMLorentzVector                            mu1(tag.first.pt(), tag.first.eta(), tag.first.phi(), MU_MASS);
            math::PtEtaPhiMLorentzVector                            mu2(probe.pt(), probe.eta(), probe.phi(), MU_MASS);
            float dM_Z_Mmumu                                        = abs(91.2 - (mu1 + mu2).mass());
            
            // save quantities to ordered heap
            auto pair_idx                                           = std::make_pair(tag_idx, probe_idx);
            SA_pair_vtx_probs.emplace_back(                         std::make_pair(vtx.prob(), pair_idx));
            // SA_pair_dz_PV_SV.emplace_back(                        std::make_pair(vtx.dz_PV_SV(StandAlone_nt.pv_z), pair_idx));
            SA_pair_dPhi_muons.emplace_back(                        std::make_pair(dPhi_muons, pair_idx));
            SA_pair_dM_Z_Mmumu.emplace_back(                        std::make_pair(dM_Z_Mmumu, pair_idx));
        }                   // end probe loop
    }                       // end tag loop

    nt.npairs                                                       = pair_vtx_probs.size();
    StandAlone_nt.npairs                                            = SA_pair_vtx_probs.size();

    // Final pair selection now run again to select probes
    // loop over tags
    for (const auto& tag : tag_muon_ttrack) 
    {
        bool tagHasProbe                                            = false;

        if (debug_ > 0) 
        {
            std::cout       << "Analyzing tag num. "      << (&tag - &tag_muon_ttrack[0])+1   << std::endl;
            std::cout       << "Tag: pt "                 << tag.first.pt() 
                            << " eta "                    << tag.first.eta() 
                            << " phi "                    << tag.first.phi()
                            << std::endl;
        }
        // Loop over stand alone muons
        if (saveStandAloneTree_) 
        {
            //for (const reco::Muon& tmp_probe : *muons) 
            for (const auto& tmp_probe : *muons) 
            {
                //esco dal ciclo sui probes appena riesco ad associare un probe al tag corrente 
                //  --> Eng: I exit the probes loop as soon as I manage to associate a probe with the current tag 
                if (tagHasProbe)                                                break;

                if (debug_ > 0) 
                {
                    std::cout   << "Analyzing the candidate probe number: " 
                                << ( &tmp_probe - &muons->at(0))+1 
                                << std::endl;
                }

                // check if muon is a standAloneMuon
                if (!tmp_probe.isStandAloneMuon())                              continue;
                if (!((*tmp_probe.standAloneMuon()).numberOfValidHits() > 0.))  continue;

                // find tracker track assocoated with standAloneMuons
                const reco::Track probe                                         = *tmp_probe.standAloneMuon();

                if (debug_ > 0)
                {
                    std::cout   << "    Probe pt "  << probe.pt() 
                        << " eta "                  << probe.eta() 
                        << " phi "                  << probe.phi() 
                        << "  charge "              << probe.charge() 
                        << std::endl;
                }

                // apply cuts on probe
                if (!probeSelectionSA_(probe))                                  continue;
                if (muonOnly_ && !probeMuonSelection_(tmp_probe))               continue;

                // apply cuts on pairs; selected will be saved
                if (tag.first.charge() == probe.charge())                       continue;

                // Add pt condition to take allTracks pT instead of StandAloneMuon

                float mass                                                      = DimuonMass(   tag.first.pt(), 
                                                                                                tag.first.eta(), 
                                                                                                tag.first.phi(), 
                                                                                                probe.pt(), 
                                                                                                probe.eta(), 
                                                                                                probe.phi());


                if (mass < pairMassMin_ || mass > pairMassMax_)                 continue;

                if (debug_ > 0)                                                 std::cout << "   Just computed the mass: " << mass << std::endl;

                bool TagAndTagPair                                              = false;
                for (auto& tag : tag_muon_ttrack) 
                {
                    if (abs(probe.phi() - tag.first.phi()) < 1e-5 
                            && abs(probe.eta() - tag.first.eta()) < 1e-5 
                            && abs(probe.pt() - tag.first.pt()) < 1e-5)         TagAndTagPair = true;
                }
                if(TagAndTagPair) 
                {
                    if (debug_ > 0)                                             std::cout << "   Disregarded a tag-tag pair! " << std::endl;
                    continue;
                }

                math::PtEtaPhiMLorentzVector                                    mu2(probe.pt(), probe.eta(), probe.phi(), MU_MASS);

                //filling tag and pair standalone ntuple info
                StandAlone_embedTriggerMatching(iEvent, trigResults, tag.first, StandAlone_nt, tagFilters_, true, debug_);

                StandAlone_nt.tag_isMatchedGen                      = genmatched_tag[&tag - &tag_muon_ttrack[0]];

                if(debug_ > 0) 
                {
                    std::cout   <<  "Saving in the tree tag infos having index: " 
                                <<  &tag - &tag_muon_ttrack[0] 
                                <<  std::endl;
                }

                StandAloneFillTagBranches<reco::Muon, reco::Track>(tag.first, tracks, StandAlone_nt, *pv);

                if(debug_ > 0) 
                {

                    std::cout   << "Saving the tag-probe pairs having following infos..."     << std::endl;
                    std::cout   << "Index of the tag: "       << &tag - &tag_muon_ttrack[0]       << std::endl;
                    std::cout   << "Index of the probe: "     << &tmp_probe - &muons->at(0)   << std::endl;
                    std::cout   << "Invariant mass: "         << mass                         << std::endl;
                }

                StandAloneFillPairBranches<reco::Muon, reco::Track>(tag.first, probe, StandAlone_nt);

                // Is this needed?????
                /*
                   auto tagRef                                         = muonsView->refAt(tag_muon_map[&tag - &tag_muon_ttrack[0]]);
                   if (!simInfoIsAvailalbe)                            StandAloneFillSimMatchingBranchesDummy(StandAlone_nt, true);
                   else 
                   {
                   const auto& msi                                 = (*simInfo)[tagRef];
                   StandAloneFillSimMatchingBranchesAOD(msi, StandAlone_nt, true);
                   }
                   */

                // finding trk idx for associated trk and matched trk via dR
                auto it                                             = std::find(trk_SAmuon_map.second.begin(), 
                                                                                trk_SAmuon_map.second.end(), 
                                                                                &tmp_probe - &muons->at(0));
                auto assoc_it                                       = std::find(associatedtrk_muon_map.second.begin(), 
                                                                                associatedtrk_muon_map.second.end(), 
                                                                                &tmp_probe - &muons->at(0));

                reco::Muon                                          fakeMuon;
                fakeMuon.setP4(mu2);
                fakeMuon.setCharge(probe.charge());

                int match_trk_idx                                   = -99;
                int assoc_trk_idx                                   = -99;
                int match_muon_idx                                  = &tmp_probe - &muons->at(0);

                std::pair<std::vector<bool>, std::vector<reco::Track>> match_tracks;
                if (it != trk_SAmuon_map.second.end()) 
                {
                    unsigned idx                                    = std::distance(trk_SAmuon_map.second.begin(), it);
                    match_trk_idx                                   = trk_SAmuon_map.first[idx];

                    match_tracks.first.push_back(true);
                    match_tracks.second.push_back(nocut_tracks.at(match_trk_idx));

                    if (debug_ > 0) 
                    {
                        std::cout << "Matching found in the map"                  << std::endl;
                        std::cout << "Index of the probe: "   << match_muon_idx   << std::endl;
                        std::cout << "Index of the track: "   << match_trk_idx    << std::endl;
                    }
                }
                // se it punta alla fine del vettore (no matching), si prende la traccia di default
                //  --> Eng: if it points to the end of the vector (no matching), the default track is taken
                if (it == trk_SAmuon_map.second.end()) 
                {
                    match_tracks.first.push_back(false);
                    match_tracks.second.push_back(nocut_tracks.at(0));

                    if (debug_ > 0)                                   std::cout << "No matching found in the map" <<std::endl;
                }

                if (assoc_it != associatedtrk_muon_map.second.end()) 
                {
                    unsigned assoc_idx                                = std::distance(associatedtrk_muon_map.second.begin(), assoc_it);
                    assoc_trk_idx                                     = associatedtrk_muon_map.first[assoc_idx];

                    match_tracks.first.push_back(true);
                    match_tracks.second.push_back(nocut_tracks.at(assoc_trk_idx));

                    if (debug_ > 0) 
                    {
                        std::cout << "Matching for FAKE found in the map"           << std::endl;
                        std::cout << "Index of the probe: "     << match_muon_idx   << std::endl;
                        std::cout << "Index of the track: "     << assoc_trk_idx    << std::endl;
                    }
                }
                if (assoc_it == associatedtrk_muon_map.second.end()) 
                {
                    match_tracks.first.push_back(false);
                    match_tracks.second.push_back(nocut_tracks.at(0));

                    if (debug_ > 0)                                     std::cout << "No matching for FAKE found in the map" <<std::endl;
                }

                // filling probe standalone ntuple info
                if(debug_ > 0) 
                {
                    std::cout   << "Saving in the tree the infos of the probe with index: " 
                                <<  &tmp_probe - &muons->at(0) 
                                << std::endl;
                }

                StandAloneFillProbeBranches<reco::Muon, pat::Muon, reco::Track>(
                        fakeMuon, 
                        *muons, 
                        tracks, 
                        StandAlone_nt, 
                        match_muon_idx, 
                        *pv, 
                        match_tracks
                );

                if (includeJets_)                                       FindJetProbePair<reco::PFJet, reco::Muon>(corrJets, fakeMuon, StandAlone_nt);

                /*
                if (!simInfoIsAvailalbe or match_muon_idx < 0)          StandAloneFillSimMatchingBranchesDummy(StandAlone_nt, false);
                else 
                {
                    auto muRef                                          = muonsView->refAt(match_muon_idx);
                    const auto& msi                                     = (*simInfo)[muRef];
                    StandAloneFillSimMatchingBranchesAOD(msi, StandAlone_nt, false);
                }
                */

                StandAlone_nt.iprobe++;
                if (debug_ > 0) 
                {
                    std::cout   << "Before saving"                                          << std::endl;
                    std::cout   << "Mass saved in the tree:"    << StandAlone_nt.pair_mass  << std::endl;
                    std::cout   << "tag pt in the tree:"        << StandAlone_nt.tag_pt     << std::endl;
                    std::cout   << "probe pt in the tree:"      << StandAlone_nt.probe_pt   << std::endl;
                }

                StandAlone_t1->Fill();

                if(debug_ > 0) 
                {
                    std::cout   << "Before saving"                                          << std::endl;
                    std::cout   << "Mass saved in the tree:"    << StandAlone_nt.pair_mass  << std::endl;
                    std::cout   << "tag pt in the tree:"        << StandAlone_nt.tag_pt     << std::endl;
                    std::cout   << "probe pt in the tree:"      << StandAlone_nt.probe_pt   << std::endl;

                }

                // Imposto la variabile a true per indicare che ho associato un probe al tag
                //  --> Eng: I set the variable to true to indicate that I have associated a probe with the tag
                tagHasProbe                                             = true;

            }                                                                   // loop over probes
        }                                                                       // is save standalone
    }                                                                           // loop tags
}

// ------------ method called once each job just before starting event loop
// ------------
void StandAloneMuonMiniAODAnalyzer::beginJob() {
    t1            = fs->make<TTree>(          "Events",           "Events");
    nt.SetTree(t1);
    nt.CreateBranches(                        HLTPaths_,          probeSelectorNames_);
    //SA
    StandAlone_t1 = fs->make<TTree>(          "StandAloneEvents", "StandAloneEvents");
    StandAlone_nt.SetTree(                    StandAlone_t1);
    StandAlone_nt.CreateBranches(             HLTPaths_);
    if (!tagFilters_.empty())
    {
        nt.CreateExtraTrgBranches(              tagFilters_,        true);
        StandAlone_nt.CreateExtraTrgBranches(   tagFilters_,        true);
    }
    if (!probeFilters_.empty())
    {
        nt.CreateExtraTrgBranches(              probeFilters_,      false);
        StandAlone_nt.CreateExtraTrgBranches(   probeFilters_,      false);
    }
}

// ------------ method called once each job just after ending the event loop
// ------------
void StandAloneMuonMiniAODAnalyzer::endJob() {}

// ------------ method fills 'descriptions' with the allowed parameters for the
// module  ------------
void StandAloneMuonMiniAODAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    // The following says we do not know what parameters are allowed so do no
    // validation
    // Please change this to state exactly what you do use, even if it is no
    // parameters
    edm::ParameterSetDescription  desc;
    desc.setUnknown();
    descriptions.addDefault(      desc);
}

///////////////////////

// define this as a plug-in
DEFINE_FWK_MODULE(StandAloneMuonMiniAODAnalyzer);
