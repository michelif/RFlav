#ifndef EventNtuplizer_h 
#define EventNtuplizer_h

#include "BaseNtuplizer.h"

#include "JetNtuplizer.h"

class EventNtuplizer : public BaseNtuplizer 
{
public:
	EventNtuplizer(const edm::ParameterSet &cfg, edm::ConsumesCollector &&cc, JetNtuplizer * jetNtuplizer );
		
	virtual void newEvent(const edm::Event & event, const edm::EventSetup& eventSetup);

	const reco::Vertex & primaryVertex() const { return pv_; };
private:
	JetNtuplizer * jetNtuplizer_;
	
	int bnvtx_;
        edm::EDGetTokenT<reco::VertexCollection> vertexes_;
	reco::Vertex pv_;

	int brho_;
	edm::EDGetTokenT<double> rho_;
	
};


#endif // EventNtuplizer_h
