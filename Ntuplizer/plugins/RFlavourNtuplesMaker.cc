#ifndef RFlavourNtuplesMaker_cc
#define RFlavourNtuplesMaker_cc


#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"
#include "TTree.h"

#include "../interface/EventNtuplizer.h"
#include "../interface/JetNtuplizer.h"
#include "../interface/CandidateNtuplizer.h"
#include "../interface/ClusterNtuplizer.h"

#include <iostream>

class RFlavourNtuplesMaker : public edm::EDAnalyzer 
{
public:
	RFlavourNtuplesMaker( const edm::ParameterSet &cfg ); 

	void analyze(edm::Event const & event, const edm::EventSetup& eventSetup);

private:
	const edm::EDGetTokenT<edm::View<pat::Jet> > jets_;
	
	CandidateNtuplizer candidateNtuplizer_;
	ClusterNtuplizer clusterNtuplizer_;
	JetNtuplizer jetNtuplizer_;
	EventNtuplizer eventNtuplizer_;
	bool shareTree_;
};

RFlavourNtuplesMaker::RFlavourNtuplesMaker( const edm::ParameterSet &cfg ) :
	jets_(consumes< edm::View<pat::Jet> >(cfg.getParameter<edm::InputTag>("jets"))),
	candidateNtuplizer_(cfg.getParameter<edm::ParameterSet>("candidateCfg"),consumesCollector()),
	clusterNtuplizer_(cfg.getParameter<edm::ParameterSet>("clusterCfg"),consumesCollector()),
	jetNtuplizer_(cfg.getParameter<edm::ParameterSet>("jetCfg"),consumesCollector(),&candidateNtuplizer_,&clusterNtuplizer_),
	eventNtuplizer_(cfg.getParameter<edm::ParameterSet>("eventCfg"),consumesCollector(),&jetNtuplizer_),
	shareTree_(cfg.getParameter<bool>("shareTree"))
{
	edm::Service<TFileService> fileService;
	auto fdir = fileService->tFileDirectory();
	
	auto evTree = eventNtuplizer_.bookTree(fdir);
	if( shareTree_ ) {
		jetNtuplizer_.bookBranches(evTree);
		candidateNtuplizer_.bookBranches(evTree);
		clusterNtuplizer_.bookBranches(evTree);
	} else {
		jetNtuplizer_.bookTree(fdir);
		candidateNtuplizer_.bookTree(fdir);
		clusterNtuplizer_.bookTree(fdir);
	}
}

void RFlavourNtuplesMaker::analyze(const edm::Event & event, const edm::EventSetup& eventSetup)
{
	eventNtuplizer_.newEvent(event,eventSetup);
	
	edm::Handle<edm::View<pat::Jet> > jets;
	event.getByToken(jets_,jets);
	auto pv = eventNtuplizer_.primaryVertex();
	
	/// std::cout << "Number of jets " << jets->size() << std::endl;
	for(auto & jet : *jets) {
		jetNtuplizer_.addJet(jet);
		
		for(size_t idaug=0; idaug<jet.numberOfDaughters(); ++idaug) {
			auto daug = jet.daughterPtr(idaug);
			candidateNtuplizer_.addCandidate(daug,pv);
		}
		
		int nclus = jet.userInt("nclusters");
		for(int iclus=0; iclus<nclus; ++iclus) {
			auto & clus = dynamic_cast<const reco::CompositePtrCandidate &>(*(jet.userCand(Form("cluster%d",iclus))));
			auto seed = candidateNtuplizer_.addCandidate(clus.daughterPtr(0),pv,true);
			clusterNtuplizer_.newCluster(clus,seed.first,pv);
			
			for(size_t itk=0; itk<clus.numberOfDaughters(); ++itk) {
				auto track = clus.daughterPtr(itk);
				auto ttrack = candidateNtuplizer_.addCandidate(track,pv,true);
				clusterNtuplizer_.addTrack(track,ttrack.second,ttrack.first);
			}
		}		
	}
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(RFlavourNtuplesMaker);


#endif // RFlavourNtuplesMaker_cc
