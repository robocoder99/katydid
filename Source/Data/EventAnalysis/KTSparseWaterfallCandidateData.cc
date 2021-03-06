/*
 * KTSparseWaterfallCandidateData.cc
 *
 *  Created on: Feb 7, 2013
 *      Author: nsoblath
 */

#include "KTSparseWaterfallCandidateData.hh"

namespace Katydid
{
    const std::string KTSparseWaterfallCandidateData::sName("sparse-wf-candidate");

    KTSparseWaterfallCandidateData::KTSparseWaterfallCandidateData() :
            KTExtensibleData< KTSparseWaterfallCandidateData >(),
            fPoints(),
            fComponent(0),
            fAcquisitionID(0),
            fCandidateID(0),
            //fTimeBinWidth(0.),
            //fFreqBinWidth(0.),
            fTimeInRunC(0.),
            fTimeLength(0.),
            fTimeInAcq(0.),
            //fFirstSliceNumber(0),
            //fLastSliceNumber(0),
            fMinFrequency(0.),
            fMaxFrequency(0.),
            //fMeanStartFrequency(0.),
            //fMeanEndFrequency(0.),
            fFrequencyWidth(0.)
            //fStartRecordNumber(0),
            //fStartSampleNumber(0),
            //fEndRecordNumber(0),
            //fEndSampleNumber(0)
    {
    }

    KTSparseWaterfallCandidateData::~KTSparseWaterfallCandidateData()
    {
    }

    void KTSparseWaterfallCandidateData::AddPoint(const KTDiscriminatedPoint& point)
    {
        fPoints.insert(point);
        return;
    }

} /* namespace Katydid */
