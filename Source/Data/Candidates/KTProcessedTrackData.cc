/*
 * KTProcessedTrackData.cc
 *
 *  Created on: July 22, 2014
 *      Author: nsoblath
 */

#include "KTProcessedTrackData.hh"

namespace Katydid
{
    KTProcessedTrackData::KTProcessedTrackData() :
            KTExtensibleData< KTProcessedTrackData >(),
            fComponent(0),
            fIsCut(false),
            fStartTimeInRunC(0.),
            fEndTimeInRunC(0.),
            fTimeLength(0.),
            fStartFrequency(0.),
            fEndFrequency(0.),
            fFrequencyWidth(0.),
            fSlope(0.),
            fIntercept(0.),
            fTotalPower(0.),
            fStartTimeInRunCSigma(0.),
            fEndTimeInRunCSigma(0.),
            fTimeLengthSigma(0.),
            fStartFrequencySigma(0.),
            fEndFrequencySigma(0.),
            fFrequencyWidthSigma(0.),
            fSlopeSigma(0.),
            fInterceptSigma(0.),
            fTotalPowerSigma(0.)
    {
    }

    KTProcessedTrackData::~KTProcessedTrackData()
    {
    }

} /* namespace Katydid */
