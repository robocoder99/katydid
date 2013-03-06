/*
 * KTSliceHeader.cc
 *
 *  Created on: Feb 14, 2013
 *      Author: nsoblath
 */

#include "KTSliceHeader.hh"

namespace Katydid
{
    KTSliceHeader::KTSliceHeader() :
            KTExtensibleData< KTSliceHeader >(),
            fTimeInRun(0.),
            fSliceNumber(0),
            fSliceSize(1),
            fSliceLength(0.),
            fSampleRate(0.),
            fBinWidth(1.),
            fComponentData(1)
    {
    }

    KTSliceHeader::KTSliceHeader(const KTSliceHeader& orig) :
            KTExtensibleData< KTSliceHeader >(orig),
            fTimeInRun(orig.fTimeInRun),
            fSliceNumber(orig.fSliceNumber),
            fSliceSize(orig.fSliceSize),
            fSliceLength(orig.fSliceLength),
            fSampleRate(orig.fSampleRate),
            fBinWidth(orig.fBinWidth),
            fComponentData(orig.fComponentData)
    {
    }

    KTSliceHeader::~KTSliceHeader()
    {
    }

    KTSliceHeader& KTSliceHeader::operator=(const KTSliceHeader& rhs)
    {
        KTExtensibleData< KTSliceHeader >::operator=(rhs);
        fTimeInRun = rhs.fTimeInRun;
        fSliceNumber = rhs.fSliceNumber;
        fSliceSize = rhs.fSliceSize;
        fSliceLength = rhs.fSliceLength;
        fSampleRate = rhs.fSampleRate;
        fBinWidth = rhs.fBinWidth;
        fComponentData = rhs.fComponentData;
        return *this;
    }

} /* namespace Katydid */
