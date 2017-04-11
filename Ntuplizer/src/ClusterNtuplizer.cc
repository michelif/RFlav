#include "../interface/ClusterNtuplizer.h"

#include "TrackingTools/IPTools/interface/IPTools.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/PatternTools/interface/TwoTrackMinimumDistance.h"

ClusterNtuplizer::ClusterNtuplizer(const edm::ParameterSet &cfg, edm::ConsumesCollector &&cc ) :
	BaseNtuplizer(cfg,std::forward<edm::ConsumesCollector>(cc))
{
	bookIndex("Clus_index");
	ijetb_ = bookSharedIndex("Jet_index");
	icandb_ = bookSharedIndex("Cand_index");
	seedPVDistb_ = bookFloat("Clus_seedPVDist");
	seedingPointxb_ = bookFloat("Clus_seedingPointx");
	seedingPointyb_ = bookFloat("Clus_seedingPointy");
	seedingPointzb_ = bookFloat("Clus_seedingPointz");
	trackPointxb_ = bookFloat("ClusTrack_trackPointx");
	trackPointyb_ = bookFloat("ClusTrack_trackPointy");
	trackPointzb_ = bookFloat("ClusTrack_trackPointz");
	trackPointCovxxb_ = bookFloat("ClusTrack_trackPointCovxx");
	trackPointCovyyb_ = bookFloat("ClusTrack_trackPointCovyy");
	trackPointCovzzb_ = bookFloat("ClusTrack_trackPointCovzz");
	trackPointCovyxb_ = bookFloat("ClusTrack_trackPointCovyx");
	trackPointCovzxb_ = bookFloat("ClusTrack_trackPointCovzx");
	trackPointCovzyb_ = bookFloat("ClusTrack_trackPointCovzy");
	distToSeedb_ = bookFloat("ClusTrack_distToSeed");
	distToSeedSigb_ = bookFloat("ClusTrack_distToSeedSig");
	crossingPointResidxb_ = bookFloat("ClusTrack_crossingPointResidx");
	crossingPointResidyb_ = bookFloat("ClusTrack_crossingPointResidy");
	crossingPointResidzb_ = bookFloat("ClusTrack_crossingPointResidz");
	crossingPointPVDistb_ = bookFloat("ClusTrack_crossingPointPVDist");
	distClApproachb_ = bookFloat("ClusTrack_distClApproach");
	trackRelDirxb_ = bookFloat("ClusTrack_trackRelDirx");
	trackRelDiryb_ = bookFloat("ClusTrack_trackRelDiry");
	trackRelDirzb_ = bookFloat("ClusTrack_trackRelDirz");
	trackRelDirPhib_ = bookFloat("ClusTrack_trackRelDirPhi");
	trackRelDirEtab_ = bookFloat("ClusTrack_trackRelDirEta");
}
	
void ClusterNtuplizer::newEvent(const edm::Event & event, const edm::EventSetup& eventSetup)
{
	BaseNtuplizer::newEvent(event,eventSetup);
	
	resetIndex();
	setDefaults();
}

void ClusterNtuplizer::newJet(const pat::Jet & jet, index_t ijet)
{
	resetIndex(false);
	setDefaults();
	fillSharedIndex(ijetb_,ijet);
}

void ClusterNtuplizer::newCluster(const reco::CompositePtrCandidate & clus, reco::TransientTrack & seedtt, const reco::Vertex & pv)
{
	incrementIndex();
	setDefaults();
	centroid_ = GlobalVector(clus.vertex().x(),clus.vertex().y(),clus.vertex().z());
	seedtt_ = &seedtt;
	ipSeed_ = IPTools::absoluteImpactParameter3D(seedtt,pv);
	pv_ = GlobalPoint(pv.position().x(),pv.position().y(),pv.position().z());
}

void ClusterNtuplizer::addTrack(const reco::CandidatePtr & cand, index_t icand, reco::TransientTrack & candtt)
{
	using namespace reco;
	VertexDistance3D distanceComputer;
	TwoTrackMinimumDistance dist;
	setDefaults();
	fillSharedIndex(icandb_, icand);
	
	float       pvDistance       = ipSeed_.second.value();
	fillFloat(seedPVDistb_,pvDistance);
	fillFloat(seedingPointxb_,centroid_.x());
	fillFloat(seedingPointyb_,centroid_.y());
	fillFloat(seedingPointzb_,centroid_.z());
	
	if( candtt == *seedtt_ ) {
		fillFloat(distToSeedb_,0.);
		fillFloat(distToSeedSigb_,0.);
	} else if(dist.calculate(candtt.impactPointState(),seedtt_->impactPointState())) {
		GlobalPoint seedPosition     = dist.points().second;
		GlobalError seedPositionErr  = seedtt_->impactPointState().cartesianError().position();
		GlobalPoint ttPoint          = dist.points().first;
		GlobalError ttPointErr       = candtt.impactPointState().cartesianError().position();
		Measurement1D m              = distanceComputer.distance(VertexState(seedPosition,seedPositionErr), VertexState(ttPoint, ttPointErr));
		GlobalPoint  cpResid         = dist.crossingPoint() - centroid_; 
		
		float distanceFromPV =  (dist.points().second-pv_).mag();
		float distance = dist.distance();
		GlobalVector seedDir = GlobalVector(seedtt_->impactPointState().globalDirection().x(),seedtt_->impactPointState().globalDirection().y(),seedtt_->impactPointState().globalDirection().z()); 
		GlobalVector trackDir = GlobalVector(candtt.impactPointState().globalDirection().x(),candtt.impactPointState().globalDirection().y(),candtt.impactPointState().globalDirection().z()) - seedDir; 
		//// float dotprodTrackSeed2D = trackDir2D.unit().dot(seedDir2D.unit());
		//// 
		//// float dotprodTrack = (dist.points().first-pv_).unit().dot(candtt.impactPointState().globalDirection().unit());
		//// float dotprodSeed = (dist.points().second-pv_).unit().dot(seedtt_->impactPointState().globalDirection().unit());
		//// 
		//// float weight = distanceFromPV*distanceFromPV/(pvDistance*distance);
		

		fillFloat(distToSeedb_,m.value());
		fillFloat(distToSeedSigb_,m.significance());
		
		fillFloat(trackPointxb_,ttPoint.x());
		fillFloat(trackPointyb_,ttPoint.y());
		fillFloat(trackPointzb_,ttPoint.z());
		fillFloat(trackPointCovxxb_,ttPointErr.cxx());
		fillFloat(trackPointCovyyb_,ttPointErr.cyy());
		fillFloat(trackPointCovzzb_,ttPointErr.czz());
		fillFloat(trackPointCovyxb_,ttPointErr.cyx());
		fillFloat(trackPointCovzxb_,ttPointErr.czx());
		fillFloat(trackPointCovzyb_,ttPointErr.czy());

		fillFloat(crossingPointResidxb_,cpResid.x());
		fillFloat(crossingPointResidyb_,cpResid.y());
		fillFloat(crossingPointResidzb_,cpResid.z());
		fillFloat(crossingPointPVDistb_,distanceFromPV);
		
		fillFloat(distClApproachb_,distance);
		fillFloat(trackRelDirxb_,trackDir.x());
		fillFloat(trackRelDiryb_,trackDir.y());
		fillFloat(trackRelDirzb_,trackDir.z());
		fillFloat(trackRelDirPhib_,trackDir.phi());
		fillFloat(trackRelDirEtab_,trackDir.eta());
	}
	fillTree();
}
