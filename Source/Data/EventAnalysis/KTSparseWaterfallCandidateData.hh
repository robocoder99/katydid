/*
 * KTSparseWaterfallCandidateData.hh
 *
 *  Created on: Feb 7, 2013
 *      Author: nsoblath
 */

#ifndef KTSPARSEWATERFALLCANDIDATEDATA_HH_
#define KTSPARSEWATERFALLCANDIDATEDATA_HH_

#include "KTData.hh"
#include "KTDiscriminatedPoint.hh"

#include "KTMemberVariable.hh"

#include <set>

namespace Katydid
{
    class KTSparseWaterfallCandidateData : public Nymph::KTExtensibleData< KTSparseWaterfallCandidateData >
    {
        public:
            // struct Point
            // {
            //     double fTimeInRunC;
            //     double fFrequency;
            //     double fAmplitude;
            //     double fTimeInAcq;
            //     double fMean;
            //     double fVariance;
            //     double fNeighborhoodAmplitude;
            //     Point(double tirc, double freq, double amp, double tiacq, double mean, double variance, double neighborhoodAmplitude) : fTimeInRunC(tirc), fFrequency(freq), fAmplitude(amp), fTimeInAcq(tiacq), fMean(mean), fVariance(variance), fNeighborhoodAmplitude(neighborhoodAmplitude) {}
            // };

            // struct PointCompare
            // {
            //     bool operator() (const Point& lhs, const Point& rhs) const
            //     {
            //         return lhs.fTimeInRunC < rhs.fTimeInRunC || (lhs.fTimeInRunC == rhs.fTimeInRunC && lhs.fFrequency < rhs.fFrequency);
            //     }
            // };

            // typedef std::set< Point, PointCompare > Points;

        public:
            KTSparseWaterfallCandidateData();
            virtual ~KTSparseWaterfallCandidateData();

            const KTDiscriminatedPoints& GetPoints() const;
            KTDiscriminatedPoints& GetPoints();

            MEMBERVARIABLE(unsigned, Component);
            MEMBERVARIABLE(uint64_t, AcquisitionID);
            MEMBERVARIABLE(unsigned, CandidateID);

            //MEMBERVARIABLE(unsigned, NTimeBins);
            //MEMBERVARIABLE(double, TimeBinWidth);

            //MEMBERVARIABLE(unsigned, NFreqBins);
            //MEMBERVARIABLE(double, FreqBinWidth);

            MEMBERVARIABLE(double, TimeInRunC);
            MEMBERVARIABLE(double, TimeLength);
            MEMBERVARIABLE(double, TimeInAcq);
            //MEMBERVARIABLE(uint64_t, FirstSliceNumber);
            //MEMBERVARIABLE(uint64_t, LastSliceNumber);
            MEMBERVARIABLE(double, MinimumFrequency);
            MEMBERVARIABLE(double, MaximumFrequency);
            //MEMBERVARIABLE(double, MeanStartFrequency);
            //MEMBERVARIABLE(double, MeanEndFrequency);
            MEMBERVARIABLE(double, FrequencyWidth);

            //MEMBERVARIABLE(unsigned, StartRecordNumber);
            //MEMBERVARIABLE(unsigned, StartSampleNumber);
            //MEMBERVARIABLE(unsigned, EndRecordNumber);
            //MEMBERVARIABLE(unsigned, EndSampleNumber);

            void AddPoint(const KTDiscriminatedPoint& point);

        private:
            KTDiscriminatedPoints fPoints;

        public:
            static const std::string sName;

    };

    inline const KTDiscriminatedPoints& KTSparseWaterfallCandidateData::GetPoints() const
    {
        return fPoints;
    }

    inline KTDiscriminatedPoints& KTSparseWaterfallCandidateData::GetPoints()
    {
        return fPoints;
    }

} /* namespace Katydid */
#endif /* KTWATERFALLCANDIDATEDATA_HH_ */
