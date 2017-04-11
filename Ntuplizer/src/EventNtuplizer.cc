#include "../interface/EventNtuplizer.h"


EventNtuplizer::EventNtuplizer(const edm::ParameterSet &cfg, edm::ConsumesCollector &&cc, JetNtuplizer * jetNtuplizer ) :
	BaseNtuplizer(cfg,std::forward<edm::ConsumesCollector>(cc)),
	jetNtuplizer_(jetNtuplizer),
	vertexes_(cc.consumes<reco::VertexCollection>(cfg.getParameter<edm::InputTag>("vertexes"))),
	rho_(cc.consumes<double>(cfg.getParameter<edm::InputTag>("rho")))
{
	bnvtx_ = bookInt("Ev_nvtx");
	brho_ = bookFloat("Ev_rho");
}
	
void EventNtuplizer::newEvent(const edm::Event & event, const edm::EventSetup& eventSetup)
{
	BaseNtuplizer::newEvent(event,eventSetup);
	
	edm::Handle<double> rho;
	event.getByToken(rho_,rho);
	fillFloat(brho_,*rho);
	
        edm::Handle<reco::VertexCollection> vertexes;
	event.getByToken(vertexes_,vertexes);	
	fillInt(bnvtx_,vertexes->size());
	
	if ( vertexes->size() > 0 ) {
		pv_ = (*vertexes)[0];
	} else {
		reco::Vertex::Error e;
		e(0,0)=0.0015*0.0015;
		e(1,1)=0.0015*0.0015;
		e(2,2)=15.*15.;
		reco::Vertex::Point p(0,0,0);
		pv_=reco::Vertex(p,e,1,1,1);
	}
	
	
	jetNtuplizer_->newEvent(event,eventSetup);
	
	fillTree();
}
