#include "../interface/CandidateNtuplizer.h"

// reconstruct IP
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "RecoVertex/VertexPrimitives/interface/ConvertToFromReco.h"
#include "TrackingTools/GeomPropagators/interface/AnalyticalImpactPointExtrapolator.h"

CandidateNtuplizer::CandidateNtuplizer(const edm::ParameterSet &cfg, edm::ConsumesCollector &&cc ) :
	BaseNtuplizer(cfg,std::forward<edm::ConsumesCollector>(cc))
{
	bookIndex("Cand_index");
	ijetb_ = bookSharedIndex("Jet_index");
	
	ptb_  = bookFloat("Cand_pt");
	etab_ = bookFloat("Cand_eta");
	phib_ = bookFloat("Cand_phi");
	typeb_ = bookByte("Cand_type");
	ptrelb_  = bookFloat("Cand_ptRel");
	deltaRb_ = bookFloat("Cand_deltaR");
	
	distb_ = bookFloat("Track_dist");
	lengthb_ = bookFloat("Track_length");
	dxyb_ = bookFloat("Track_dxy");
	dxyErrorb_ = bookFloat("Track_dxyError");
	dzb_ = bookFloat("Track_dz");
	dzErrorb_ = bookFloat("Track_dzError");
	sign2Db_ = bookByte("Track_sign2D");
	sign3Db_ = bookByte("Track_sign3D");
	
	iP2Db_ = bookFloat("Track_IP2Db");
	iP2Dsigb_ = bookFloat("Track_IP2Dsigb");
	iPb_ = bookFloat("Track_IPb");
	iPsigb_ = bookFloat("Track_IPsigb");
	iP2Derrb_ = bookFloat("Track_IP2Derrb");
	iPerrb_ = bookFloat("Track_IPerrb");

	chi2b_ = bookFloat("Track_chi2");
	nHitAllb_ = bookFloat("Track_nHitAllb");
	nHitPixelb_ = bookFloat("Track_nHitPixelb");
	nHitStripb_ = bookFloat("Track_nHitStripb");
	nHitTIBb_ = bookFloat("Track_nHitTIBb");
	nHitTIDb_ = bookFloat("Track_nHitTIDb");
	nHitTOBb_ = bookFloat("Track_nHitTOBb");
	nHitTECb_ = bookFloat("Track_nHitTECb");
	nHitPXBb_ = bookFloat("Track_nHitPXBb");
	nHitPXFb_ = bookFloat("Track_nHitPXFb");
	isHitL1b_ = bookFloat("Track_isHitL1b");
	
}
	
void CandidateNtuplizer::newEvent(const edm::Event & event, const edm::EventSetup& eventSetup)
{
	BaseNtuplizer::newEvent(event,eventSetup);
	
	resetIndex();
	setDefaults();
	cache_.clear();
	cacheIndex_.clear();
	
	// build transient tracks from PackedCandidates
	eventSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",trackBuilder_);

}

void CandidateNtuplizer::newJet(const pat::Jet & jet, index_t ijet)
{
	resetIndex(false);
	setDefaults();
	fillSharedIndex(ijetb_,ijet);
	currJet_ = &jet;
	jdirection_ = GlobalVector(jet.px(), jet.py(), jet.pz());
	cache_.clear();
	cacheIndex_.clear();
}

std::pair<reco::TransientTrack,BaseNtuplizer::index_t> & CandidateNtuplizer::addCandidate(const reco::CandidatePtr & cand, const reco::Vertex & pv, bool fromCluster)
{
	using namespace edm;
	using namespace reco;
	
	bool isnew = false;
	size_t loc  = 0;
	auto itr = find(cacheIndex_.begin(), cacheIndex_.end(), cand);
	auto packCand = dynamic_cast<const pat::PackedCandidate*>( cand.get() );
	if( itr == cacheIndex_.end() ) {
		isnew = true;
		incrementIndex();
		if( cand->charge() == 0 || packCand->numberOfHits()<= 0 ) { 
		 	cache_.push_back( std::make_pair(TransientTrack(),getIndex())); 
		} else { 
			cache_.push_back( std::make_pair(trackBuilder_->build( cand ),getIndex())); 
		}
		loc = cache_.size()-1;
		cacheIndex_.push_back(cand);
	} else {
		loc = itr - cacheIndex_.begin();
		if( shared_ ) { setIndex( cache_[loc].second ); }
	}

	if( isnew || shared_ ) {
		setDefaults();
		fillFloat(ptb_,cand->pt());
		fillFloat(etab_,cand->eta());
		fillFloat(phib_,cand->phi());
		fillByte(typeb_,cand->pdgId());
		
		fillFloat(ptrelb_,cand->pt()/currJet_->pt());
		float deltaR = reco::deltaR( *cand, *currJet_ );
		fillFloat(deltaRb_,deltaR);
		
		auto & ttrack = cache_[loc].first;
		if( ttrack.isValid() ) {
			float decayLength=-1.;
			TrajectoryStateOnSurface closest = IPTools::closestApproachToJet(ttrack.impactPointState(), pv, jdirection_, ttrack.field());
			if (closest.isValid()) {
				decayLength =  (closest.globalPosition() - RecoVertex::convertPos(pv.position())).mag();
			} else {
				decayLength = -1;
			}
			
			float distJetAxis =  IPTools::jetTrackDistance(ttrack, jdirection_, pv).second.value();
			auto ptrack = packCand->pseudoTrack();
			fillFloat(distb_,distJetAxis);
			fillFloat(lengthb_,decayLength);
			fillFloat(dxyb_,ptrack.dxy(pv.position()));
			fillFloat(dzb_,ptrack.dz(pv.position()));
			fillFloat(dxyErrorb_,ptrack.dxyError());
			fillFloat(dzErrorb_,ptrack.dzError());
			
			
			
			{
				TransverseImpactPointExtrapolator extrapolator(ttrack.field());
				TrajectoryStateOnSurface closestOnTransversePlaneState =
					extrapolator.extrapolate(ttrack.impactPointState(),RecoVertex::convertPos(pv.position()));
				GlobalPoint impactPoint    = closestOnTransversePlaneState.globalPosition();
				GlobalVector IPVec(impactPoint.x()-pv.x(),impactPoint.y()-pv.y(),0.);
				double prod = IPVec.dot(jdirection_);
				char sign = (prod>=0) ? 1 : -1;
				fillByte(sign2Db_,sign);
			}
			{
				AnalyticalImpactPointExtrapolator extrapolator(ttrack.field());
				TrajectoryStateOnSurface closestIn3DSpaceState =
					extrapolator.extrapolate(ttrack.impactPointState(),RecoVertex::convertPos(pv.position()));
				GlobalPoint impactPoint = closestIn3DSpaceState.globalPosition();
				GlobalVector IPVec(impactPoint.x()-pv.x(),impactPoint.y()-pv.y(),impactPoint.z()-pv.z());
				double prod = IPVec.dot(jdirection_);
				char sign = (prod>=0) ? 1 : -1;
				fillByte(sign3Db_,sign);
			}
			
			Measurement1D ip2d    = IPTools::signedTransverseImpactParameter(ttrack, jdirection_, pv).second;
			Measurement1D ip3d    = IPTools::signedImpactParameter3D(ttrack, jdirection_, pv).second;
			
			fillFloat(iP2Db_,(ip2d.value()));
			fillFloat(iP2Dsigb_,(ip2d.significance()));
			fillFloat(iPb_,(ip3d.value()));
			fillFloat(iPsigb_,(ip3d.significance()));
			fillFloat(iP2Derrb_,(ip2d.error()));
			fillFloat(iPerrb_,(ip3d.error()));
			
			fillFloat(chi2b_,ptrack.normalizedChi2());
			fillFloat(nHitAllb_,ptrack.numberOfValidHits());
			fillFloat(nHitPixelb_,ptrack.hitPattern().numberOfValidPixelHits());
			fillFloat(nHitStripb_,ptrack.hitPattern().numberOfValidStripHits());
			fillFloat(nHitTIBb_,ptrack.hitPattern().numberOfValidStripTIBHits());
			fillFloat(nHitTIDb_,ptrack.hitPattern().numberOfValidStripTIDHits());
			fillFloat(nHitTOBb_,ptrack.hitPattern().numberOfValidStripTOBHits());
			fillFloat(nHitTECb_,ptrack.hitPattern().numberOfValidStripTECHits());
			fillFloat(nHitPXBb_,ptrack.hitPattern().numberOfValidPixelBarrelHits());
			fillFloat(nHitPXFb_,ptrack.hitPattern().numberOfValidPixelEndcapHits());
			fillFloat(isHitL1b_,ptrack.hitPattern().hasValidHitInFirstPixelBarrel());
			
		}
		// Add leptons info
		
	}
	
	if( isnew && ! ( fromCluster && shared_ ) ) { fillTree(); }
	
	return cache_[loc];
}
