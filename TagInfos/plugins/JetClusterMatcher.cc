#ifndef JetClusterMatcher_h
#define JetClusterMatcher_h

#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"

#include "DataFormats/Candidate/interface/Candidate.h"

#include "DataFormats/Math/interface/deltaR.h"

#include "TString.h"

// -------------------------------------------------------------------------------------------------------------------
class JetClusterMatcher : public edm::global::EDProducer<> {
    
public:
    
	explicit JetClusterMatcher(const edm::ParameterSet & iConfig);
	~JetClusterMatcher();
    
	virtual void produce(edm::StreamID, edm::Event & iEvent, const edm::EventSetup& iSetup) const override;
    
private:
	const edm::EDGetTokenT<edm::View<reco::Candidate> >  clusters_;
	const edm::EDGetTokenT<edm::View<pat::Jet> > jets_;
	double maxClusterDeltaR_, maxSeedDeltaR_;
};



// -------------------------------------------------------------------------------------------------------------------
JetClusterMatcher::JetClusterMatcher(const edm::ParameterSet& iConfig) :
	clusters_(consumes< edm::View<reco::Candidate> >(iConfig.getParameter<edm::InputTag>("clusters"))),
	jets_(consumes< edm::View<pat::Jet> >(iConfig.getParameter<edm::InputTag>("jets"))),
	maxClusterDeltaR_(iConfig.getParameter<double>("maxClusterDeltaR")),
	maxSeedDeltaR_(iConfig.getParameter<double>("maxSeedDeltaR"))
{
	produces<std::vector<pat::Jet>>();
}


// -------------------------------------------------------------------------------------------------------------------
JetClusterMatcher::~JetClusterMatcher() 
{

}

// -------------------------------------------------------------------------------------------------------------------
void JetClusterMatcher::produce(edm::StreamID, edm::Event & iEvent, const edm::EventSetup & iSetup) const 
{
	using namespace edm; using namespace std; using namespace reco;
	
	// read-in PackedCandidates and primary vertexes
	Handle<edm::View<reco::Candidate> > clusters;
	iEvent.getByToken(clusters_, clusters);
	Handle<edm::View<pat::Jet> > jets;
	iEvent.getByToken(jets_, jets);
	
	// fill cluster candidates
	std::auto_ptr< std::vector<pat::Jet> > outJets( new std::vector<pat::Jet>() );
	outJets->reserve(jets->size());
	for(auto & ijet : *jets) {
		auto jet = ijet;
		int nclust = 0;
		for(size_t iclus=0; iclus<clusters->size(); ++iclus) {
			auto cluster = clusters->ptrAt(iclus);
			if( deltaR(jet,*cluster) < maxClusterDeltaR_ || deltaR(jet,*(cluster->daughter(0))) < maxSeedDeltaR_ ) {
				jet.addUserCand( Form("cluster%d",nclust), cluster  );
				++nclust;
			}
		}
		jet.addUserInt("nclusters",nclust);
		outJets->push_back(jet);
	}

	iEvent.put(outJets);
}


#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(JetClusterMatcher);

#endif // JetClusterMatcher_h
