#include "DataFormats/Candidate/interface/CompositePtrCandidate.h"
#include <vector>

namespace  {
    struct dictionary {
	    std::vector<reco::CompositePtrCandidate>                 vec_ptr_cand;
	    edm::Wrapper<std::vector<reco::CompositePtrCandidate>>   wrp_vec_ptr_cand;

    };
}
