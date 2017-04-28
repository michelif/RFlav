#ifndef CandidateNtuplizer_h 
#define CandidateNtuplizer_h

#include "BaseNtuplizer.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"

#include "DataFormats/Candidate/interface/CandidateFwd.h"

#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"


class CandidateNtuplizer : public BaseNtuplizer 
{
public:
	CandidateNtuplizer(const edm::ParameterSet &cfg, edm::ConsumesCollector &&cc );
	
	virtual void newEvent(const edm::Event & event, const edm::EventSetup& eventSetup);
	
	void newJet(const pat::Jet & jet, index_t ijet);
	
	std::pair<reco::TransientTrack,index_t> & addCandidate(const reco::CandidatePtr & cand, const reco::Vertex & pv, bool fromCluster=false);
	
private:
	const pat::Jet * currJet_;
	GlobalVector jdirection_;
	size_t ijetb_;
	size_t ptrelb_, ptb_, etab_, phib_;
	size_t typeb_;
	size_t deltaRb_, distb_, lengthb_, dxyb_, dxyErrorb_, dzErrorb_, dzb_, sign2Db_, sign3Db_;
	size_t iP2Db_, iP2Dsigb_, iPb_, iPsigb_, iP2Derrb_, iPerrb_;
	size_t chi2b_, nHitAllb_, nHitPixelb_, nHitStripb_, nHitTIBb_, nHitTIDb_, nHitTOBb_, nHitTECb_, nHitPXBb_, nHitPXFb_, isHitL1b_;
	
	std::vector<reco::CandidatePtr> cacheIndex_;
	std::vector<std::pair<reco::TransientTrack,index_t>> cache_;
	edm::ESHandle<TransientTrackBuilder> trackBuilder_;
};


#endif // CandidateNtuplizer_h
