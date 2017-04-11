#include "../interface/BaseNtuplizer.h"
#include <iostream>

BaseNtuplizer::BaseNtuplizer(const edm::ParameterSet &cfg, edm::ConsumesCollector &&cc ) :
	shared_(true), tree_(0), bookIndex_(false)
{
	treeName_ = cfg.getParameter<std::string>("treeName");
}


TTree * BaseNtuplizer::bookTree(TFileDirectory & dir)
{
	tree_ = dir.make<TTree>( treeName_.c_str(), treeName_.c_str() );
	shared_ = false;
	bookBranches(tree_);
	return tree_;
}

void BaseNtuplizer::bookBranches(TTree * tree) 
{
	tree_ = tree;
	bookBranches();
}

void BaseNtuplizer::bookBranches() 
{
	using namespace std;
	if( ! shared_ ) {
		tree_->Branch( "Ev_event", &event_, "Ev_event/l" );
		tree_->Branch( "Ev_lumi", &lumi_, "Ev_lumi/i" );
		tree_->Branch( "Ev_run", &run_, "Ev_run/i" );
	}
	if( bookIndex_ ) {
		tree_->Branch(indexName_.c_str(),&index_);
	}
	if( ! shared_ ) {
		for(size_t ivar=0; ivar<sharedIndexesNames_.size(); ++ivar) {
			auto name = sharedIndexesNames_[ivar];
			auto * var = &(sharedIndexes_[ivar]);
			tree_->Branch(name.c_str(),var);
		}
	}
	for(size_t ivar=0; ivar<intNames_.size(); ++ivar) {
		auto name = intNames_[ivar];
		auto * var = &(ints_[ivar]);
		tree_->Branch(name.c_str(),var,(name+"/I").c_str());
	}
	for(size_t ivar=0; ivar<byteNames_.size(); ++ivar) {
		auto name = byteNames_[ivar];
		auto * var = &(bytes_[ivar]);
		tree_->Branch(name.c_str(),var,(name+"/B").c_str());
	}
	for(size_t ivar=0; ivar<floatNames_.size(); ++ivar) {
		auto name = floatNames_[ivar];
		auto * var = &(floats_[ivar]);
		tree_->Branch(name.c_str(),var,(name+"/F").c_str());
	}
	for(size_t ivar=0; ivar<vintNames_.size(); ++ivar) {
		auto name = vintNames_[ivar];
		auto * var = &(vints_[ivar]);
		tree_->Branch(name.c_str(),var);
	}
	for(size_t ivar=0; ivar<vfloatNames_.size(); ++ivar) {
		auto name = vfloatNames_[ivar];
		auto * var = &(vfloats_[ivar]);
		tree_->Branch(name.c_str(),var);
	}
}
	
void BaseNtuplizer::newEvent(const edm::Event & event, const edm::EventSetup& eventSetup)
{
	if( ! shared_ ) { // fillEventInfo
		event_ = event.id().event();
		lumi_ = event.id().luminosityBlock();
		run_  = event.id().run();
	}
	isData_ = event.isRealData();
}

void BaseNtuplizer::setDefaults()
{
	ints_ = defaultInts_;
	bytes_ = defaultBytes_;
	floats_ = defaultFloats_;
	for(size_t ivint=0; ivint<vints_.size(); ++ivint) {
		vints_[ivint] = defaultVints_[ivint];
	}
	for(size_t ivfloat=0; ivfloat<vfloats_.size(); ++ivfloat) {
		vfloats_[ivfloat] = defaultVfloats_[ivfloat];
	}
}

void BaseNtuplizer::resetIndex(bool sharedToo)
{ 
	index_ = -1; 
	maxindex_ = 0;
	if( sharedToo ) { 
		for(auto & shIdx : sharedIndexes_) { shIdx = -1; }
	}
}

size_t BaseNtuplizer::bookSharedIndex(const std::string & name)
{
	sharedIndexesNames_.push_back(name);
	sharedIndexes_.push_back(0);
	return sharedIndexes_.size()-1;
}

size_t BaseNtuplizer::bookInt(const std::string & name, int defaultVal)
{
	intNames_.push_back(name);
	ints_.push_back(defaultVal);
	defaultInts_.push_back(defaultVal);
	return ints_.size()-1;
}

size_t BaseNtuplizer::bookByte(const std::string & name, char defaultVal)
{
	byteNames_.push_back(name);
	bytes_.push_back(defaultVal);
	defaultBytes_.push_back(defaultVal);
	return bytes_.size()-1;
}

size_t BaseNtuplizer::bookFloat(const std::string & name, float defaultVal)
{
	floatNames_.push_back(name);
	floats_.push_back(defaultVal);
	defaultFloats_.push_back(defaultVal);
	return floats_.size()-1;
}

size_t BaseNtuplizer::bookVInt(const std::string & name, std::vector<int> defaultVal)
{
	vintNames_.push_back(name);
	vints_.push_back(defaultVal);
	defaultVints_.push_back(defaultVal);
	return vints_.size()-1;
}

size_t BaseNtuplizer::bookVFloat(const std::string & name, std::vector<float> defaultVal)
{
	vfloatNames_.push_back(name);
	vfloats_.push_back(defaultVal);
	defaultVfloats_.push_back(defaultVal);
	return vfloats_.size()-1;
}
