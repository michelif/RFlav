#ifndef BaseNtuplizer_h
#define BaseNtuplizer_h

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"
#include "TTree.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Framework/interface/ConsumesCollector.h"

class BaseNtuplizer 
{
public:	
	typedef int index_t;

	BaseNtuplizer(const edm::ParameterSet &cfg, edm::ConsumesCollector &&cc );

	virtual TTree * bookTree(TFileDirectory & dir);
	virtual void bookBranches(TTree * tree);
	
	virtual void newEvent(const edm::Event & event, const edm::EventSetup& eventSetup);
	
protected:
	size_t bookByte(const std::string & name, char defaultVal=0);
	void fillByte(size_t id, char val) { bytes_[id] = val; };

	size_t bookInt(const std::string & name, int defaultVal=0);
	void fillInt(size_t id, int val) { ints_[id] = val; };
	
	size_t bookFloat(const std::string & name, float defaultVal=0.);
	void fillFloat(size_t id, float val) { floats_[id] = val; };
	
	size_t bookVInt(const std::string & name, std::vector<int> defaultVal=std::vector<int>(0));
	void fillVInt(size_t id, std::vector<int> val) { vints_[id] = val; };
	
	size_t bookVFloat(const std::string & name, std::vector<float> defaultVal=std::vector<float>(0));
	void fillVFloat(size_t id, std::vector<float> val) { vfloats_[id] = val; };

	void setDefaults();
	void fillTree() { tree_->Fill(); }

	void setTree(TTree * tree) { tree_ = tree; };
	void bookBranches();
	void bookIndex(const std::string & name) { indexName_ = name; bookIndex_ = true; };
	void resetIndex(bool sharedToo=true);
	void incrementIndex() { index_ = maxindex_; ++maxindex_; }
	index_t getIndex() { return index_; }
	void setIndex(index_t val) { index_ = val; }

	size_t bookSharedIndex(const std::string & name);
	void fillSharedIndex(size_t id, index_t val) { sharedIndexes_[id] = val; };
	
	bool shared_, isData_;

private:
	std::string treeName_;
	TTree * tree_;
	std::string indexName_;
	bool bookIndex_;
	index_t index_;
	index_t maxindex_;
	
	unsigned long long event_;
	unsigned int lumi_;
	unsigned int run_;

	std::vector<int> ints_;
	std::vector<int> defaultInts_;
	std::vector<std::string> intNames_;

	std::vector<char> bytes_;
	std::vector<char> defaultBytes_;
	std::vector<std::string> byteNames_;

	std::vector<float> floats_;
	std::vector<float> defaultFloats_;
	std::vector<std::string> floatNames_;
	
	std::vector<std::vector<int>> vints_;
	std::vector<std::vector<int>> defaultVints_;
	std::vector<std::string> vintNames_;

	std::vector<std::vector<float>> vfloats_;
	std::vector<std::vector<float>> defaultVfloats_;
	std::vector<std::string> vfloatNames_;

	std::vector<std::string> sharedIndexesNames_;
	std::vector<index_t> sharedIndexes_;
};


#endif // BaseNtuplizer_h
