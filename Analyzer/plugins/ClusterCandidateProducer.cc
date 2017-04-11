#ifndef ClusterCandidateProducer_h
#define ClusterCandidateProducer_h

#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"

#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
// #include "TrackingTools/TransientTrack/interface/CandidatePtrTransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "RecoVertex/AdaptiveVertexFinder/interface/TracksClusteringFromDisplacedSeed.h"

#include "CommonTools/CandUtils/interface/AddFourMomenta.h"
#include "DataFormats/Candidate/interface/CompositePtrCandidate.h"

// -------------------------------------------------------------------------------------------------------------------
class ClusterCandidateProducer : public edm::global::EDProducer<> {
    
public:
    
	explicit ClusterCandidateProducer(const edm::ParameterSet & iConfig);
	~ClusterCandidateProducer();
    
	virtual void produce(edm::StreamID, edm::Event & iEvent, const edm::EventSetup& iSetup) const override;
    
private:
	// configurables
	const edm::EDGetTokenT<edm::View<reco::Candidate> >    Cands_;
	const edm::EDGetTokenT<reco::VertexCollection>         PVs_;
	const edm::EDGetTokenT<edm::View<reco::Candidate> >    AdditionalTracks_;
	int maxNtracks_;
	mutable TracksClusteringFromDisplacedSeed clusterMaker_;
};



// -------------------------------------------------------------------------------------------------------------------
ClusterCandidateProducer::ClusterCandidateProducer(const edm::ParameterSet& iConfig) :
	Cands_(consumes< edm::View<reco::Candidate> >(iConfig.getParameter<edm::InputTag>("packedCandidates"))),
	PVs_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("slimmedVertices"))),
	AdditionalTracks_(consumes<edm::View<reco::Candidate> >(iConfig.getParameter<edm::InputTag>("additionalTracks"))),
	maxNtracks_(iConfig.getParameter<int>("maxNtracks")),
	clusterMaker_(iConfig)
{
	produces<std::vector<reco::CompositePtrCandidate>>();
}


// -------------------------------------------------------------------------------------------------------------------
ClusterCandidateProducer::~ClusterCandidateProducer() 
{

}

// -------------------------------------------------------------------------------------------------------------------
void fillFromCandidates(std::vector<reco::TransientTrack> & output, const edm::View<reco::Candidate> & input,  const TransientTrackBuilder & builder)
{
	for(size_t icand=0; icand<input.size(); ++icand) {
		auto cand = input.ptrAt(icand);
		auto packCand = dynamic_cast<const pat::PackedCandidate*>( cand.get() );
		if( cand->charge() == 0 || packCand->numberOfHits()<= 0) { continue; }			
		auto ttrack = builder.build( cand );
		output.push_back(ttrack);
	}
}

//// // -------------------------------------------------------------------------------------------------------------------
//// std::vector<float> computeTkInfo(const TransientTrack & track, const TransientTrack & seed, 
//// 				 std::pair<bool,Measurement1D> ipSeed, const reco::GlobalPoint & pv, const reco::GlobalPoint & centroid)
//// {
//// 	using namespace reco;
//// 	std::vector<float> info(ClusterCandidate::ninfo,0.);
//// 	VertexDistance3D distanceComputer;
//// 	TwoTrackMinimumDistance dist;
//// 	if(dist.calculate(track.impactPointState(),seed.impactPointState())) {
//// 		float       pvDistance       = ipSeed.second.value();
//// 		GlobalPoint ttPoint          = dist.points().first;
//// 		GlobalError ttPointErr       = tt->impactPointState().cartesianError().position();
//// 		GlobalPoint seedPosition     = dist.points().second;
//// 		GlobalError seedPositionErr  = seed.impactPointState().cartesianError().position();
//// 		Measurement1D m              = distanceComputer.distance(VertexState(seedPosition,seedPositionErr), VertexState(ttPoint, ttPointErr));
//// 		GlobalPoint  cpResid         = dist.crossingPoint() - centroid; 
//// 		
//// 		
//// 		float distanceFromPV =  (dist.points().second-pv).mag();
//// 		float distance = dist.distance();
//// 		GlobalVector trackDir2D(tt->impactPointState().globalDirection().x(),tt->impactPointState().globalDirection().y(),0.); 
//// 		GlobalVector seedDir2D(seed.impactPointState().globalDirection().x(),seed.impactPointState().globalDirection().y(),0.); 
//// 		//SK:UNUSED//    float dotprodTrackSeed2D = trackDir2D.unit().dot(seedDir2D.unit());
//// 		
//// 		float dotprodTrack = (dist.points().first-pv).unit().dot(tt->impactPointState().globalDirection().unit());
//// 		float dotprodSeed = (dist.points().second-pv).unit().dot(seed.impactPointState().globalDirection().unit());
//// 		
//// 		float weight = distanceFromPV*distanceFromPV/(pvDistance*distance);
//// 		
//// 	}
//// 
//// 	
//// }

// -------------------------------------------------------------------------------------------------------------------
void ClusterCandidateProducer::produce(edm::StreamID, edm::Event & iEvent, const edm::EventSetup & iSetup) const 
{
	using namespace edm; using namespace std; using namespace reco;
	
	// read-in PackedCandidates and primary vertexes
	Handle<edm::View<reco::Candidate> > cands;
	iEvent.getByToken(Cands_, cands);
	Handle<VertexCollection> pvs;
	iEvent.getByToken(PVs_, pvs);
	Handle<edm::View<reco::Candidate> > addTracks;
	iEvent.getByToken(AdditionalTracks_, addTracks);

	// build transient tracks from PackedCandidates
	edm::ESHandle<TransientTrackBuilder> trackBuilder;
	iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",
					       trackBuilder);
	std::vector<reco::TransientTrack> tracks;
	fillFromCandidates( tracks, *cands,     *trackBuilder );
	fillFromCandidates( tracks, *addTracks, *trackBuilder );

	// run clustering
	auto clusterInfos = clusterMaker_.clusters( (*pvs)[0], tracks );
	
	// fill cluster candidates
	std::auto_ptr< std::vector<reco::CompositePtrCandidate> > candidates( new std::vector<reco::CompositePtrCandidate>() );
	AddFourMomenta add4mom;
	for(auto & cluster : clusterInfos ) {
		// compute extra information to be stored
		// std::vector< std::pair<size_t,std::vector<float>> > tkInfos;
		std::vector< std::pair<size_t,float> > tkInfos;
		TwoTrackMinimumDistance dist;
		for(auto & track : cluster.tracks ) {
			dist.calculate(track.impactPointState(),cluster.seedingTrack.impactPointState());
			// tkInfos.push_back( std::make_pair(tkInfos.size(), computeTkInfo(track, cluster.seed, (*pv)[0] ))  );
			tkInfos.push_back( std::make_pair(tkInfos.size(), dist.distance())  );
		}
		
		// sort tracks by distance from seed
		//// std::sort(tkInfos.begin(), tkInfos.end(), 
		//// 	  [](std::pair<size_t,std::vector<float>> & info1, std::pair<size_t,std::vector<float>> &info2) 
		//// 	  { return info1.second[ClusterCandidate::dSeedxy] < info2.second[ClusterCandidate::dSeedxyz] } );
		std::sort(tkInfos.begin(), tkInfos.end(), 
			  [](std::pair<size_t,float> & info1, std::pair<size_t,float> &info2)
			  { return info1.second < info2.second; } );
		
		// fill cluster candidate
		// ClusterCandidate cand;
		CompositePtrCandidate cand;
		cand.setVertex( CompositePtrCandidate::Point(cluster.seedPoint.x(),cluster.seedPoint.y(),cluster.seedPoint.z()) );
		size_t ntracks = std::min(cluster.tracks.size(), (size_t)maxNtracks_ );
		for(size_t itk=0; itk<ntracks; ++itk) {
			auto track = cluster.tracks[ tkInfos[itk].first ].basicTransientTrack()->candidate();
			cand.addDaughter( track );
		}
		add4mom.set(cand);
		candidates->push_back(cand);
	}

	iEvent.put(candidates);
}


#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(ClusterCandidateProducer);

#endif // ClusterCandidateProducer_h
