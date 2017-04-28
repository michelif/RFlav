#include "../interface/JetNtuplizer.h"


JetNtuplizer::JetNtuplizer(const edm::ParameterSet &cfg, edm::ConsumesCollector &&cc, 
			   CandidateNtuplizer * candidateNtuplizer, ClusterNtuplizer * clusterNtuplizer  ) :
	BaseNtuplizer(cfg,std::forward<edm::ConsumesCollector>(cc)),
	candidateNtuplizer_(candidateNtuplizer),
	clusterNtuplizer_(clusterNtuplizer)
{
	bookIndex("Jet_index");
	ptb_  = bookFloat("Jet_pt");
	etab_ = bookFloat("Jet_eta");
	phib_ = bookFloat("Jet_phi");
	areab_ = bookFloat("Jet_area");
	massb_ = bookFloat("Jet_mass");
	genPtb_  = bookFloat("Jet_genPt");
	genPartonidb_ = bookByte("Jet_genPartonid");
	genFlavourb_ = bookByte("Jet_genFlavour");
	genGlavourCleanedb_ = bookByte("Jet_genFlavourCleaned");
	genPartonFlavourb_ = bookByte("Jet_genPartonFlavour");
	genHadronFlavourb_ = bookByte("Jet_genHadronFlavour");
	genNbHadronsb_ = bookByte("Jet_genNbHadrons");
	genNcHadronsb_ = bookByte("Jet_genNcHadrons");
	cCSVb_ = bookFloat("Jet_cCSV");
	DeepCSVbb_ = bookFloat("Jet_DeepCSVb");
	DeepCSVcb_ = bookFloat("Jet_DeepCSVc");
	DeepCSVlb_ = bookFloat("Jet_DeepCSVl");
	DeepCSVbbb_ = bookFloat("Jet_DeepCSVbb");
	DeepCSVccb_ = bookFloat("Jet_DeepCSVcc");
	cMVAv2b_ = bookFloat("Jet_cMVAv2");
}
	
void JetNtuplizer::newEvent(const edm::Event & event, const edm::EventSetup& eventSetup)
{
	BaseNtuplizer::newEvent(event,eventSetup);	

	resetIndex();
	setDefaults();

	candidateNtuplizer_->newEvent(event,eventSetup);
	clusterNtuplizer_->newEvent(event,eventSetup);
}

void JetNtuplizer::addJet(const pat::Jet & jet)
{
	incrementIndex();
	
	// from BTagAnalyzer
	int flavour  =-1  ;
	if ( !isData_ ) {
		flavour = abs( jet.partonFlavour() );
		if ( flavour >= 1 && flavour <= 3 ) flavour = 1;
	}
	
	int hflav = jet.hadronFlavour();		
	int pflav = jet.partonFlavour();
	int cflav = 0; //~correct flavour definition
	if( !isData_ ) {
		if( hflav != 0 ) {
			cflav = hflav;
		} else { //not a heavy jet
			cflav = std::abs(pflav) == 4 || std::abs(pflav) == 5 ? 0 : pflav;
		}
	}
	

	
	fillFloat(ptb_,jet.pt());
	fillFloat(etab_,jet.eta());
	fillFloat(phib_,jet.phi());
	fillFloat(areab_,jet.jetArea());
	fillFloat(massb_,jet.mass());
		
	fillFloat(genPtb_, ( jet.genJet()!=0 ? jet.genJet()->pt() : -1. ));
	fillByte(genPartonidb_, jet.genParton() ? jet.genParton()->pdgId() : 0 );
	fillByte(genFlavourb_, cflav);
	fillByte(genGlavourCleanedb_, cflav);
	fillByte(genPartonFlavourb_, jet.partonFlavour());
	fillByte(genHadronFlavourb_, jet.hadronFlavour());
	fillByte(genNbHadronsb_, jet.jetFlavourInfo().getbHadrons().size());
	fillByte(genNcHadronsb_, jet.jetFlavourInfo().getcHadrons().size());

	fillFloat(cCSVb_,jet.bDiscriminator("pfCombinedCvsBJetTags"));
	fillFloat(DeepCSVbb_,jet.bDiscriminator("deepFlavourJetTags:probb"   ));
	fillFloat(DeepCSVcb_,jet.bDiscriminator("deepFlavourJetTags:probc"   ));
	fillFloat(DeepCSVlb_,jet.bDiscriminator("deepFlavourJetTags:probudsg"));
	fillFloat(DeepCSVbbb_,jet.bDiscriminator("deepFlavourJetTags:probbb"  ));
	fillFloat(DeepCSVccb_,jet.bDiscriminator("deepFlavourJetTags:probcc"  ));
	fillFloat(cMVAv2b_,jet.bDiscriminator("pfCombinedMVAV2BJetTags"));
	
	candidateNtuplizer_->newJet(jet,getIndex());
	clusterNtuplizer_->newJet(jet,getIndex());
	
	fillTree();
}
