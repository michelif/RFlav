#!/bin/bash

set -x 

version=$1

taskFolder=phase1_${version}

outFolder=//pnfs/psi.ch/cms/trivcat/store/user/$USER/btag

stageTo=$outFolder/$taskFolder

xrdfs t3dcachedb03.psi.ch mkdir $stageTo

fggRunJobs.py --load rFlavPhaseI.json -x cmsRun runRFlavourNtuplizer.py --stage-to root://t3dcachedb03.psi.ch:$stageTo \
    -d $taskFolder useAAA=1 maxEvents=-1 -q all.q \
    -n 5
