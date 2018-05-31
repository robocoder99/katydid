/**
 @file KTDiscriminatedPoint.hh
 @brief Defines the point structure
 @details Defines the basic point structure used for any discriminated point: used in KTSparseWaterfallCandidateData, KTTrackProcessing
 @author: M. Guigue
 @date: May 31, 2018
 */

#ifndef KTDISCRIMINATEDPOINT_HH_
#define KTDISCRIMINATEDPOINT_HH_

#include <set>

namespace Katydid
{
    struct KTDiscriminatedPoint
    {
        double fTimeInRunC;
        double fFrequency;
        double fAmplitude;
        double fTimeInAcq;
        double fMean;
        double fVariance;
        double fNeighborhoodAmplitude;
        // The SequentialTrackFinder Processor uses BinInSlice when it sums over adjacent bins in the power spectrum slice
        // Once that slow is deleted we no longer need this variable
        unsigned fBinInSlice;
        KTDiscriminatedPoint(double tirc, double freq, double amp, double tiacq, double mean, double variance, double neighborhoodAmplitude) : fTimeInRunC(tirc), fFrequency(freq), fAmplitude(amp), fTimeInAcq(tiacq), fMean(mean), fVariance(variance), fNeighborhoodAmplitude(neighborhoodAmplitude) {}

    };
    struct KTDiscriminatedPointCompare
            {
                bool operator() (const KTDiscriminatedPoint& lhs, const KTDiscriminatedPoint& rhs) const
                {
                    return lhs.fTimeInRunC < rhs.fTimeInRunC || (lhs.fTimeInRunC == rhs.fTimeInRunC && lhs.fFrequency < rhs.fFrequency);
                }
            };

    typedef std::set< KTDiscriminatedPoint, KTDiscriminatedPointCompare > KTDiscriminatedPoints;
}
#endif
