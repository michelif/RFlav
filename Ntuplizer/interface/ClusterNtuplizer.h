#ifndef ClusterNtuplizer_h 
#define ClusterNtuplizer_h

#include "BaseNtuplizer.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"

#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

#include "DataFormats/GeometryCommonDetAlgo/interface/Measurement1D.h"

class ClusterNtuplizer : public BaseNtuplizer 
{
public:
	ClusterNtuplizer(const edm::ParameterSet &cfg, edm::ConsumesCollector &&cc );
	
	virtual void newEvent(const edm::Event & event, const edm::EventSetup& eventSetup);

	void newJet(const pat::Jet & jet, index_t ijet);
	void addTrack(const reco::CandidatePtr & cand, index_t icand, reco::TransientTrack & candtt);
	void newCluster(const reco::CompositePtrCandidate & clus, reco::TransientTrack & seedtt, const reco::Vertex & pv);
	
private:
	const pat::Jet * currJet_;
	GlobalVector centroid_;
	GlobalPoint pv_;
	const reco::TransientTrack * seedtt_;
	std::pair<bool,Measurement1D> ipSeed_;
	
	size_t ijetb_, icandb_;
	size_t seedPVDistb_, seedingPointxb_, seedingPointyb_, seedingPointzb_;
	size_t trackPointxb_, trackPointyb_, trackPointzb_;
	size_t trackPointCovxxb_, trackPointCovyyb_, trackPointCovzzb_, trackPointCovyxb_, trackPointCovzxb_, trackPointCovzyb_;
	
	size_t distToSeedb_, distToSeedSigb_, crossingPointResidxb_, crossingPointResidyb_, crossingPointResidzb_, crossingPointPVDistb_, distClApproachb_;
	size_t trackRelDirxb_, trackRelDiryb_, trackRelDirzb_, trackRelDirPhib_, trackRelDirEtab_;

	        
};


#endif // ClusterNtuplizer_h
