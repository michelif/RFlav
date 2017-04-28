#ifndef JetNtuplizer_h 
#define JetNtuplizer_h

#include "BaseNtuplizer.h"

#include "CandidateNtuplizer.h"
#include "ClusterNtuplizer.h"

#include "DataFormats/PatCandidates/interface/Jet.h"

class JetNtuplizer : public BaseNtuplizer 
{
public:
	JetNtuplizer(const edm::ParameterSet &cfg, edm::ConsumesCollector &&cc, 
		     CandidateNtuplizer * candidateNtuplizer, ClusterNtuplizer * clusterNtuplizer  );
	
	virtual void newEvent(const edm::Event & event, const edm::EventSetup& eventSetup);
	
	void addJet(const pat::Jet & jet);
	
private:
	CandidateNtuplizer * candidateNtuplizer_;
	ClusterNtuplizer * clusterNtuplizer_;
	size_t ptb_, etab_, phib_, massb_, areab_;
	size_t genPtb_, genPartonidb_, genFlavourb_, genGlavourCleanedb_, genPartonFlavourb_, genHadronFlavourb_, genNbHadronsb_, genNcHadronsb_;
	size_t cCSVb_, DeepCSVbb_, DeepCSVcb_, DeepCSVlb_, DeepCSVbbb_, DeepCSVccb_, cMVAv2b_;
	
};


#endif // JetNtuplizer_h
