/*
 * KTFrequencyCandidateData.cc
 *
 *  Created on: Dec 18, 2012
 *      Author: nsoblath
 */

#include "KTFrequencyCandidateData.hh"

namespace Katydid
{

    KTFrequencyCandidateData::KTFrequencyCandidateData()
    {
    }

    KTFrequencyCandidateData::~KTFrequencyCandidateData()
    {
    }

    void KTFrequencyCandidateData::Accept(KTWriter* writer) const
    {
        writer->Write(this);
        return;
    }


} /* namespace Katydid */