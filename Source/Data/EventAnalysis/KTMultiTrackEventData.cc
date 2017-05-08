/*
 * KTMultiTrackEventData.cc
 *
 *  Created on: Aug 4, 2014
 *      Author: nsoblath
 */

#include "KTMultiTrackEventData.hh"

#include "KTLogger.hh"

#include <algorithm>
#include <cmath>

namespace Katydid
{
    KTLOGGER(evlog, "KTMultiTrackEventData")

    const std::string KTMultiTrackEventData::sName("multi-track-event");

    KTMultiTrackEventData::KTMultiTrackEventData() :
            KTExtensibleData< KTMultiTrackEventData >(),
            fComponent(0),
            fAcquisitionID(0.),
            fEventID(0),
            fTotalEventSequences(0),
            fStartTimeInAcq(0.),
            fStartTimeInRunC(0.),
            fEndTimeInRunC(0.),
            fTimeLength(0.),
            fStartFrequency(0.),
            fEndFrequency(0.),
            fMinimumFrequency(0.),
            fMaximumFrequency(0.),
            fFrequencyWidth(0.),
            fStartTimeInRunCSigma(0.),
            fEndTimeInRunCSigma(0.),
            fTimeLengthSigma(0.),
            fStartFrequencySigma(0.),
            fEndFrequencySigma(0.),
            fFrequencyWidthSigma(0.),
            fFirstTrackID(0),
            fFirstTrackTimeLength(0.),
            fFirstTrackFrequencyWidth(0.),
            fFirstTrackSlope(0.),
            fFirstTrackIntercept(0.),
            fFirstTrackTotalPower(0.),
            fUnknownEventTopology(false),
            fTracks()
    {
    }

    KTMultiTrackEventData::KTMultiTrackEventData(const KTMultiTrackEventData& orig) :
            KTExtensibleData< KTMultiTrackEventData >(orig),
            fComponent(orig.fComponent),
            fAcquisitionID(orig.fAcquisitionID),
            fEventID(orig.fEventID),
            fTotalEventSequences(orig.fTotalEventSequences),
            fStartTimeInAcq(orig.fStartTimeInAcq),
            fStartTimeInRunC(orig.fStartTimeInRunC),
            fEndTimeInRunC(orig.fEndTimeInRunC),
            fTimeLength(orig.fTimeLength),
            fStartFrequency(orig.fStartFrequency),
            fEndFrequency(orig.fEndFrequency),
            fMinimumFrequency(orig.fMinimumFrequency),
            fMaximumFrequency(orig.fMaximumFrequency),
            fFrequencyWidth(orig.fFrequencyWidth),
            fStartTimeInRunCSigma(orig.fStartTimeInRunCSigma),
            fEndTimeInRunCSigma(orig.fEndTimeInRunCSigma),
            fTimeLengthSigma(orig.fTimeLengthSigma),
            fStartFrequencySigma(orig.fStartFrequencySigma),
            fEndFrequencySigma(orig.fEndFrequencySigma),
            fFrequencyWidthSigma(orig.fFrequencyWidthSigma),
            fFirstTrackID(orig.fFirstTrackID),
            fFirstTrackTimeLength(orig.fFirstTrackTimeLength),
            fFirstTrackFrequencyWidth(orig.fFirstTrackFrequencyWidth),
            fFirstTrackSlope(orig.fFirstTrackSlope),
            fFirstTrackIntercept(orig.fFirstTrackIntercept),
            fFirstTrackTotalPower(orig.fFirstTrackTotalPower),
            fUnknownEventTopology(orig.fUnknownEventTopology),
            fTracks()
    {
        for (TrackSetCIt trackIt = orig.GetTracksBegin(); trackIt != orig.GetTracksEnd(); ++trackIt)
        {
            //fTracks.insert(Tracks::value_type(trackIt->first, trackIt->second));
            fTracks.insert(*trackIt);
        }
    }

    KTMultiTrackEventData::~KTMultiTrackEventData()
    {
    }

    KTMultiTrackEventData& KTMultiTrackEventData::operator=(const KTMultiTrackEventData& rhs)
    {
        ClearTracks();

        fComponent = rhs.fComponent;

        fAcquisitionID = rhs.fAcquisitionID;
        fEventID = rhs.fEventID;

        fTotalEventSequences = rhs.fTotalEventSequences;

        fStartTimeInAcq = rhs.fStartTimeInAcq;
        fStartTimeInRunC = rhs.fStartTimeInRunC;
        fEndTimeInRunC = rhs.fEndTimeInRunC;
        fTimeLength = rhs.fTimeLength;
        fStartFrequency = rhs.fStartFrequency;
        fEndFrequency = rhs.fEndFrequency;
        fMinimumFrequency = rhs.fMinimumFrequency;
        fMaximumFrequency = rhs.fMaximumFrequency;
        fFrequencyWidth = rhs.fFrequencyWidth;

        fStartTimeInRunCSigma = rhs.fStartTimeInRunCSigma;
        fEndTimeInRunCSigma = rhs.fEndTimeInRunCSigma;
        fTimeLengthSigma = rhs.fTimeLengthSigma;
        fStartFrequencySigma = rhs.fStartFrequencySigma;
        fEndFrequencySigma = rhs.fEndFrequencySigma;
        fFrequencyWidthSigma = rhs.fFrequencyWidthSigma;

        fFirstTrackID = rhs.fFirstTrackID;
        fFirstTrackTimeLength = rhs.fFirstTrackTimeLength;
        fFirstTrackFrequencyWidth = rhs.fFirstTrackFrequencyWidth;
        fFirstTrackSlope = rhs.fFirstTrackSlope;
        fFirstTrackIntercept = rhs.fFirstTrackIntercept;
        fFirstTrackTotalPower = rhs.fFirstTrackTotalPower;

        fUnknownEventTopology = rhs.fUnknownEventTopology;

        for (TrackSetCIt trackIt = rhs.GetTracksBegin(); trackIt != rhs.GetTracksEnd(); ++trackIt)
        {
            //fTracks.insert(Tracks::value_type(trackIt->first, trackIt->second));
            fTracks.insert(*trackIt);
        }

        return *this;
    }

    const std::string KTMultiPeakTrackData::sName("mpt-data");

    KTMultiPeakTrackData::KTMultiPeakTrackData() :
            KTExtensibleData< KTMultiPeakTrackData >(),
            fComponent(0),
            fEventSequenceID(-1),
            fMPTrack()
    {
    }

    KTMultiPeakTrackData::KTMultiPeakTrackData(const KTMultiPeakTrackData& orig) :
            KTExtensibleData< KTMultiPeakTrackData >(orig),
            
            fComponent(orig.fComponent),
            fEventSequenceID(orig.fEventSequenceID),
            fMPTrack(orig.fMPTrack)
    {
    }

    KTMultiPeakTrackData::~KTMultiPeakTrackData()
    {
    }

    KTMultiPeakTrackData& KTMultiPeakTrackData::operator=(const KTMultiPeakTrackData& rhs)
    {
        KTExtensibleData< KTMultiPeakTrackData >::operator=(rhs);
        fComponent = rhs.fComponent;
        fEventSequenceID = rhs.fEventSequenceID;
        fMPTrack = rhs.fMPTrack;
        return *this;
    }

    void KTMultiTrackEventData::AddTrack(const KTProcessedTrackData& track)
    {
        //fTracks.insert(Tracks::value_type(track.GetTrackID(), track));
        fTracks.insert(track);
        return;
    }

    void KTMultiTrackEventData::AddTracks(TrackSetCItSet tracks)
    {
        AddTracks(tracks, fTotalEventSequences);
        fTotalEventSequences++;
        return;
    }

    void KTMultiTrackEventData::AddTracks(TrackSet tracks)
    {
        AddTracks(tracks, fTotalEventSequences);
        fTotalEventSequences++;
        return;
    }

    void KTMultiTrackEventData::AddTracks(TrackSetCItSet tracks, ssize_t eventSequenceID)
    {
        for ( TrackSetCItSet::iterator aTrack=tracks.begin(); aTrack != tracks.end(); ++aTrack )
        {
            KTProcessedTrackData aProcessedTrack = KTProcessedTrackData(**aTrack);
            aProcessedTrack.SetEventSequenceID(eventSequenceID);
            AddTrack(aProcessedTrack);
        }
        return;
    }

    void KTMultiTrackEventData::AddTracks(TrackSet tracks, ssize_t eventSequenceID)
    {
        for ( TrackSet::iterator aTrack=tracks.begin(); aTrack != tracks.end(); ++aTrack )
        {
            KTProcessedTrackData aProcessedTrack = KTProcessedTrackData(*aTrack);
            aProcessedTrack.SetEventSequenceID(eventSequenceID);
            AddTrack(aProcessedTrack);
        }
        return;
    }

    void KTMultiTrackEventData::ProcessTracks()
    {
        KTDEBUG(evlog, "Processing tracks");

        TrackSetCIt trackIt = fTracks.begin();

        fStartTimeInAcq = trackIt->GetStartTimeInAcq();
        fStartTimeInRunC = trackIt->GetStartTimeInRunC();
        fEndTimeInRunC = trackIt->GetEndTimeInRunC();
        fTimeLength = trackIt->GetTimeLength();
        fStartFrequency = trackIt->GetStartFrequency();
        fEndFrequency = trackIt->GetEndFrequency();
        fMinimumFrequency = std::min(fStartFrequency, fEndFrequency);
        fMaximumFrequency = std::max(fStartFrequency, fEndFrequency);
        fFrequencyWidth = trackIt->GetFrequencyWidth();

        fStartTimeInRunCSigma = trackIt->GetStartTimeInRunCSigma();
        fEndTimeInRunCSigma = trackIt->GetEndTimeInRunCSigma();
        fTimeLengthSigma = trackIt->GetTimeLengthSigma();
        fStartFrequencySigma = trackIt->GetStartFrequencySigma();
        fEndFrequencySigma = trackIt->GetEndFrequencySigma();
        fFrequencyWidthSigma = trackIt->GetFrequencyWidthSigma();

        fFirstTrackID = trackIt->GetTrackID();
        fFirstTrackTimeLength = trackIt->GetTimeLength();
        fFirstTrackFrequencyWidth = trackIt->GetFrequencyWidth();
        fFirstTrackSlope = trackIt->GetSlope();
        fFirstTrackIntercept = trackIt->GetIntercept();
        fFirstTrackTotalPower = trackIt->GetTotalPower();

        for (++trackIt; trackIt != fTracks.end(); ++trackIt)
        {
            KTDEBUG(evlog, "Track " << trackIt->GetTrackID());

            if (trackIt->GetStartTimeInRunC() < fStartTimeInRunC)
            {
                fStartTimeInAcq = trackIt->GetStartTimeInAcq();
                fStartTimeInRunC = trackIt->GetStartTimeInRunC();
                fStartTimeInRunCSigma = trackIt->GetStartTimeInRunCSigma();
                fStartFrequency = trackIt->GetStartFrequency();
                fStartFrequencySigma = trackIt->GetStartFrequencySigma();
                KTDEBUG(evlog, "Start time (freq) is now " << fStartTimeInRunC << "(" << fStartFrequency << ")");
                fFirstTrackID = trackIt->GetTrackID();
                fFirstTrackTimeLength = trackIt->GetTimeLength();
                fFirstTrackFrequencyWidth = trackIt->GetFrequencyWidth();
                fFirstTrackSlope = trackIt->GetSlope();
                fFirstTrackIntercept = trackIt->GetIntercept();
                fFirstTrackTotalPower = trackIt->GetTotalPower();
            }

            if (trackIt->GetEndTimeInRunC() > fEndTimeInRunC)
            {
                fEndTimeInRunC = trackIt->GetEndTimeInRunC();
                fEndTimeInRunCSigma = trackIt->GetEndTimeInRunCSigma();
                fEndFrequency = trackIt->GetEndFrequency();
                fEndFrequencySigma = trackIt->GetEndFrequencySigma();
                KTDEBUG(evlog, "End time (freq) is now " << fEndTimeInRunC << "(" << fEndFrequency << ")");
            }

            double minFreq = std::min(trackIt->GetStartFrequency(), trackIt->GetEndFrequency());
            if (minFreq < fMinimumFrequency)
            {
                fMinimumFrequency = minFreq;
                KTDEBUG(evlog, "Minimum frequency is now " << fMinimumFrequency);
            }

            double maxFreq = std::max(trackIt->GetStartFrequency(), trackIt->GetEndFrequency());
            if (maxFreq > fMaximumFrequency)
            {
                fMaximumFrequency = maxFreq;
                KTDEBUG(evlog, "Maximum frequency is now " << fMaximumFrequency);
            }
        }

        fTimeLength = fEndTimeInRunC - fStartTimeInRunC;
        fTimeLengthSigma = sqrt(fEndTimeInRunCSigma * fEndTimeInRunCSigma + fStartTimeInRunCSigma * fStartTimeInRunCSigma);

        fFrequencyWidth = fMaximumFrequency - fMinimumFrequency;
        fFrequencyWidthSigma = sqrt(fMaximumFrequency * fEndFrequencySigma + fMinimumFrequency * fMinimumFrequency);

        return;
    }

    void KTMultiTrackEventData::ClearTracks()
    {
        fTracks.clear();

        fStartTimeInAcq = 0.;
        fStartTimeInRunC = 0.;
        fEndTimeInRunC = 0.;
        fTimeLength = 0.;
        fStartFrequency = 0.;
        fEndFrequency = 0.;
        fMinimumFrequency = 0.;
        fMaximumFrequency = 0.;
        fFrequencyWidth = 0.;
        fStartTimeInRunCSigma = 0.;
        fEndTimeInRunCSigma = 0.;
        fTimeLengthSigma = 0.;
        fStartFrequencySigma = 0.;
        fEndFrequencySigma = 0.;
        fFrequencyWidthSigma = 0.;
        fFirstTrackTimeLength = 0.;
        fFirstTrackFrequencyWidth = 0.;
        fFirstTrackSlope = 0.;
        fFirstTrackIntercept = 0.;
        fFirstTrackTotalPower = 0.;

        return;
    }

    MultiPeakTrackRef::MultiPeakTrackRef() :
            fTrackRefs(),
            fMeanStartTimeInRunC(0.),
            fSumStartTimeInRunC(0.),
            fMeanEndTimeInRunC(0.),
            fSumEndTimeInRunC(0.),
            fAcquisitionID(0),
            fUnknownEventTopology(false),
            fFirstStartFreq(0.),
            fLastEndFreq(0.),

            fFirstStartTimeInRunC(1.),
            fLastEndTimeInRunC(0.),
            fSumSlopes(0.),
            fMeanSlope(0.)
    {}

    /*MultiTrackTrackRef::MultiTrackTrackRef() :
            fTrackRefs(),
            fFirstStartTimeInRunC(0.),
            fLastEndTimeInRunC(0.),
            fMeanStartTimeInRunC(0.),
            fMeanEndTimeInRunC(0.),
            fFirstStartFreq(0.),
            fLastEndFreq(0.),
            fSumSlopes(0.),
            fMeanSlope(0.),
            fAcquisitionID(0),
            fUnknownEventTopology(false)
    {}*/

    bool MultiPeakTrackRef::InsertTrack(const TrackSetCIt& trackRef)
    {
        if (fTrackRefs.find(trackRef) != fTrackRefs.end())  return false;

        fTrackRefs.insert(trackRef);
        fSumStartTimeInRunC += trackRef->GetStartTimeInRunC();
        fSumEndTimeInRunC += trackRef->GetEndTimeInRunC();
        double currentSize = (double)fTrackRefs.size();
        fMeanStartTimeInRunC = fSumStartTimeInRunC / currentSize;
        fMeanEndTimeInRunC = fSumEndTimeInRunC / currentSize;
        return true;
    }

    bool MultiPeakTrackRef::AttachTrack(const TrackSetCIt& trackRef)
    {
        if (fTrackRefs.find(trackRef) != fTrackRefs.end())  return false;

        fTrackRefs.insert(trackRef);
        double trackStart = trackRef->GetStartTimeInRunC();
        double trackEnd = trackRef->GetEndTimeInRunC();
        double trackSlope = trackRef->GetSlope();

        if (trackStart < fFirstStartTimeInRunC)
        {
            fFirstStartTimeInRunC = trackStart;
            fFirstStartFreq = trackRef->GetStartFrequency();
        }
        if (trackEnd > fLastEndTimeInRunC)
        {
            fLastEndTimeInRunC = trackEnd;
            fLastEndFreq = trackRef->GetEndFrequency();
        }
        fSumSlopes += trackSlope;
        double currentSize = (double)fTrackRefs.size();
        fMeanSlope = fSumSlopes / currentSize;

        fMeanStartTimeInRunC = fFirstStartTimeInRunC;
        fMeanEndTimeInRunC = fLastEndTimeInRunC;

        return true;
    }

} /* namespace Katydid */
