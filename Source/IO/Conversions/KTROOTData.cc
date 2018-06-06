/*
 * KTROOTData.cc
 *
 *  Created on: Aug 4, 2014
 *      Author: nsoblath
 */

#include "KTROOTData.hh"

#include "KTClassifierResultsData.hh"
#include "KTMultiTrackEventData.hh"
#include "KTProcessedMPTData.hh"
#include "KTProcessedTrackData.hh"

#include "logger.hh"

#include "TClonesArray.h"

#include <iostream>

ClassImp(Katydid::TProcessedMPTData);
ClassImp(Katydid::TProcessedTrackData);
ClassImp(Katydid::TMultiTrackEventData);
ClassImp(Katydid::TClassifiedEventData);
ClassImp(Katydid::TClassifierResultsData);
ClassImp(Katydid::TDiscriminatedPoint);
ClassImp(Katydid::TSparseWaterfallCandidateData);

namespace Katydid
{
    //***********************
    // TProcessedTrackData
    //***********************

    TProcessedTrackData::TProcessedTrackData() :
            TObject(),
            fComponent(0), fTrackID(0), fEventSequenceID(-1), fIsCut(false),
            fMVAClassifier(-999.), fMainband(true),
            fAcquisitionID(0),
            fStartTimeInRunC(0.), fStartTimeInAcq(0.), fEndTimeInRunC(0.),fTimeLength(0.),
            fStartFrequency(0.), fEndFrequency(0.), fFrequencyWidth(0.),
            fSlope(0.), fIntercept(0.), fTotalPower(0.),
            fStartTimeInRunCSigma(0.), fEndTimeInRunCSigma(0.), fTimeLengthSigma(0.),
            fStartFrequencySigma(0.), fEndFrequencySigma(0.), fFrequencyWidthSigma(0.),
            fSlopeSigma(0.), fInterceptSigma(0.), fTotalPowerSigma(0.)
    {}

    TProcessedTrackData::TProcessedTrackData(const KTProcessedTrackData& orig) :
            TObject(),
            fComponent(0), fTrackID(0), fEventSequenceID(-1), fIsCut(false),
            fMVAClassifier(-999.), fMainband(true),
            fAcquisitionID(0),
            fStartTimeInRunC(0.), fStartTimeInAcq(0.), fEndTimeInRunC(0.),fTimeLength(0.),
            fStartFrequency(0.), fEndFrequency(0.), fFrequencyWidth(0.),
            fSlope(0.), fIntercept(0.), fTotalPower(0.),
            fStartTimeInRunCSigma(0.), fEndTimeInRunCSigma(0.), fTimeLengthSigma(0.),
            fStartFrequencySigma(0.), fEndFrequencySigma(0.), fFrequencyWidthSigma(0.),
            fSlopeSigma(0.), fInterceptSigma(0.), fTotalPowerSigma(0.)
    {
        Load(orig);
    }

    TProcessedTrackData::TProcessedTrackData(const TProcessedTrackData& orig) :
            TObject(orig),
            fComponent(orig.fComponent), fTrackID(orig.fTrackID), fEventID(orig.fEventID), fEventSequenceID(orig.fEventSequenceID), fIsCut(orig.fIsCut),
            fStartTimeInAcq(orig.fStartTimeInAcq), fEndTimeInRunC(orig.fEndTimeInRunC),fTimeLength(orig.fTimeLength),
            fAcquisitionID(orig.fAcquisitionID),
            fStartTimeInRunC(orig.fStartTimeInRunC),
            fStartFrequency(orig.fStartFrequency), fEndFrequency(orig.fEndFrequency), fFrequencyWidth(orig.fFrequencyWidth),
            fSlope(orig.fSlope), fIntercept(orig.fIntercept), fTotalPower(orig.fTotalPower),
            fStartTimeInRunCSigma(orig.fStartTimeInRunCSigma), fEndTimeInRunCSigma(orig.fEndTimeInRunCSigma), fTimeLengthSigma(orig.fTimeLengthSigma),
            fStartFrequencySigma(orig.fStartFrequencySigma), fEndFrequencySigma(orig.fEndFrequencySigma), fFrequencyWidthSigma(orig.fFrequencyWidthSigma),
            fSlopeSigma(orig.fSlopeSigma), fInterceptSigma(orig.fInterceptSigma), fTotalPowerSigma(orig.fTotalPowerSigma)
    {}

    TProcessedTrackData::~TProcessedTrackData()
    {}

    TObject* TProcessedTrackData::Clone(const char* newname)
    {
        TProcessedTrackData* newData = new TProcessedTrackData(*this);
        return newData;
    }

    TProcessedTrackData& TProcessedTrackData::operator=(const TProcessedTrackData& rhs)
    {
        fComponent = rhs.fComponent; fTrackID = rhs.fTrackID; fEventSequenceID = rhs.fEventSequenceID; fIsCut = rhs.fIsCut;
        fMVAClassifier = rhs.fMVAClassifier; fMainband = rhs.fMainband;
        fAcquisitionID = rhs.fAcquisitionID;
        fStartTimeInRunC = rhs.fStartTimeInRunC; fStartTimeInAcq = rhs.fStartTimeInAcq; fEndTimeInRunC = rhs.fEndTimeInRunC; fTimeLength = rhs.fTimeLength;
        fStartFrequency = rhs.fStartFrequency; fEndFrequency = rhs.fEndFrequency; fFrequencyWidth = rhs.fFrequencyWidth;
        fSlope = rhs.fSlope; fIntercept = rhs.fIntercept; fTotalPower = rhs.fTotalPower;
        fStartTimeInRunCSigma = rhs.fStartTimeInRunCSigma; fEndTimeInRunCSigma = rhs.fEndTimeInRunCSigma; fTimeLengthSigma = rhs.fTimeLengthSigma;
        fStartFrequencySigma = rhs.fStartFrequencySigma; fEndFrequencySigma = rhs.fEndFrequencySigma; fFrequencyWidthSigma = rhs.fFrequencyWidthSigma;
        fSlopeSigma = rhs.fSlopeSigma; fInterceptSigma = rhs.fInterceptSigma; fTotalPowerSigma = rhs.fTotalPowerSigma;
        return *this;
    }

    void TProcessedTrackData::Load(const KTProcessedTrackData& data)
    {
        fComponent = data.GetComponent(); fAcquisitionID = data.GetAcquisitionID(); fTrackID = data.GetTrackID(); fEventID = data.GetEventID(); fEventSequenceID = data.GetEventSequenceID(); fIsCut = data.GetIsCut();
        fMVAClassifier = data.GetMVAClassifier(); fMainband = data.GetMainband();
        fAcquisitionID = data.GetAcquisitionID();
        fStartTimeInRunC = data.GetStartTimeInRunC(); fStartTimeInAcq = data.GetStartTimeInAcq(); fEndTimeInRunC = data.GetEndTimeInRunC();fTimeLength = data.GetTimeLength();
        fStartFrequency = data.GetStartFrequency(); fEndFrequency = data.GetEndFrequency(); fFrequencyWidth = data.GetFrequencyWidth();
        fSlope = data.GetSlope(); fIntercept = data.GetIntercept(); fTotalPower = data.GetTotalPower();
        fStartTimeInRunCSigma = data.GetStartTimeInRunCSigma(); fEndTimeInRunCSigma = data.GetEndTimeInRunCSigma(); fTimeLengthSigma = data.GetTimeLengthSigma();
        fStartFrequencySigma = data.GetStartFrequencySigma(); fEndFrequencySigma = data.GetEndFrequencySigma(); fFrequencyWidthSigma = data.GetFrequencyWidthSigma();
        fSlopeSigma = data.GetSlopeSigma(); fInterceptSigma = data.GetInterceptSigma(); fTotalPowerSigma = data.GetTotalPowerSigma();
        return;
    }
    void TProcessedTrackData::Unload(KTProcessedTrackData& data) const
    {
        data.SetComponent(fComponent); data.SetAcquisitionID(fAcquisitionID); data.SetTrackID(fTrackID); data.SetEventID(fEventID); data.SetEventSequenceID(fEventSequenceID); data.SetIsCut(fIsCut);
        data.SetStartTimeInRunC(fStartTimeInRunC); data.SetStartTimeInAcq(fStartTimeInAcq); data.SetEndTimeInRunC(fEndTimeInRunC); data.SetTimeLength(fTimeLength);
        data.SetStartFrequency(fStartFrequency); data.SetEndFrequency(fEndFrequency); data.SetFrequencyWidth(fFrequencyWidth);
        data.SetSlope(fSlope); data.SetIntercept(fIntercept); data.SetTotalPower(fTotalPower);
        data.SetStartTimeInRunCSigma(fStartTimeInRunCSigma); data.SetEndTimeInRunCSigma(fEndTimeInRunCSigma); data.SetTimeLengthSigma(fTimeLengthSigma);
        data.SetStartFrequencySigma(fStartFrequencySigma); data.SetEndFrequencySigma(fEndFrequencySigma); data.SetFrequencyWidthSigma(fFrequencyWidthSigma);
        data.SetSlopeSigma(fSlopeSigma); data.SetInterceptSigma(fInterceptSigma); data.SetTotalPowerSigma(fTotalPowerSigma);
        return;
    }

    //***********************
    // TClassifierResultsData
    //***********************

    TClassifierResultsData::TClassifierResultsData() :
            TObject(),
            fComponent(0), fMainCarrierHigh(0), fMainCarrierLow(0), fSideBand(0)
    {}

    TClassifierResultsData::TClassifierResultsData(const KTClassifierResultsData& orig) :
            TObject(),
            fComponent(0), fMainCarrierHigh(0), fMainCarrierLow(0), fSideBand(0)
    {
        Load(orig);
    }

    TClassifierResultsData::TClassifierResultsData(const TClassifierResultsData& orig) :
            TObject(orig),
            fComponent(orig.fComponent), fMainCarrierHigh(orig.fMainCarrierHigh), fMainCarrierLow(orig.fMainCarrierLow), fSideBand(orig.fSideBand)
    {}

    TClassifierResultsData::~TClassifierResultsData()
    {}

    TObject* TClassifierResultsData::Clone(const char* newname)
    {
        TClassifierResultsData* newData = new TClassifierResultsData(*this);
        return newData;
    }

    TClassifierResultsData& TClassifierResultsData::operator=(const TClassifierResultsData& rhs)
    {
        fComponent = rhs.fComponent; fMainCarrierHigh = rhs.fMainCarrierHigh; fMainCarrierLow = rhs.fMainCarrierLow; fSideBand = rhs.fSideBand;
        return *this;
    }

    void TClassifierResultsData::Load(const KTClassifierResultsData& data)
    {
        fComponent = data.GetComponent(); fMainCarrierHigh = data.GetMainCarrierHigh(); fMainCarrierLow = data.GetMainCarrierLow(); fSideBand = data.GetSideBand();
        return;
    }
    void TClassifierResultsData::Unload(KTClassifierResultsData& data) const
    {
        data.SetComponent(fComponent); data.SetMainCarrierHigh(fMainCarrierHigh); data.SetMainCarrierLow(fMainCarrierLow); data.SetSideBand(fSideBand);
        return;
    }


    //***********************
    // TProcessedMPTData
    //***********************

    TProcessedMPTData::TProcessedMPTData() :
            TObject(),
            fComponent(0), fTrackID(0), fEventSequenceID(-1), fIsCut(false),
            fMVAClassifier(-999.), fMainband(true), fAxialFrequency(0.),
            fAcquisitionID(0),
            fStartTimeInRunC(0.), fStartTimeInAcq(0.), fEndTimeInRunC(0.),fTimeLength(0.),
            fStartFrequency(0.), fEndFrequency(0.), fFrequencyWidth(0.),
            fSlope(0.), fIntercept(0.), fTotalPower(0.),
            fStartTimeInRunCSigma(0.), fEndTimeInRunCSigma(0.), fTimeLengthSigma(0.),
            fStartFrequencySigma(0.), fEndFrequencySigma(0.), fFrequencyWidthSigma(0.),
            fSlopeSigma(0.), fInterceptSigma(0.), fTotalPowerSigma(0.)
    {}

    TProcessedMPTData::TProcessedMPTData(const KTProcessedMPTData& orig) :
            TObject(),
            fComponent(0), fTrackID(0), fEventSequenceID(-1), fIsCut(false),
            fMVAClassifier(-999.), fMainband(true), fAxialFrequency(0.),
            fAcquisitionID(0),
            fStartTimeInRunC(0.), fStartTimeInAcq(0.), fEndTimeInRunC(0.),fTimeLength(0.),
            fStartFrequency(0.), fEndFrequency(0.), fFrequencyWidth(0.),
            fSlope(0.), fIntercept(0.), fTotalPower(0.),
            fStartTimeInRunCSigma(0.), fEndTimeInRunCSigma(0.), fTimeLengthSigma(0.),
            fStartFrequencySigma(0.), fEndFrequencySigma(0.), fFrequencyWidthSigma(0.),
            fSlopeSigma(0.), fInterceptSigma(0.), fTotalPowerSigma(0.)
    {
        Load(orig);
    }

    TProcessedMPTData::TProcessedMPTData(const TProcessedMPTData& orig) :
            TObject(orig),
            fComponent(orig.fComponent), fTrackID(orig.fTrackID), fEventSequenceID(orig.fEventSequenceID), fIsCut(orig.fIsCut),
            fMVAClassifier(orig.fMVAClassifier), fMainband(orig.fMainband), fAxialFrequency(orig.fAxialFrequency),
            fAcquisitionID(orig.fAcquisitionID),
            fStartTimeInRunC(orig.fStartTimeInRunC), fStartTimeInAcq(orig.fStartTimeInAcq), fEndTimeInRunC(orig.fEndTimeInRunC), fTimeLength(orig.fTimeLength),
            fStartFrequency(orig.fStartFrequency), fEndFrequency(orig.fEndFrequency), fFrequencyWidth(orig.fFrequencyWidth),
            fSlope(orig.fSlope), fIntercept(orig.fIntercept), fTotalPower(orig.fTotalPower),
            fStartTimeInRunCSigma(orig.fStartTimeInRunCSigma), fEndTimeInRunCSigma(orig.fEndTimeInRunCSigma), fTimeLengthSigma(orig.fTimeLengthSigma),
            fStartFrequencySigma(orig.fStartFrequencySigma), fEndFrequencySigma(orig.fEndFrequencySigma), fFrequencyWidthSigma(orig.fFrequencyWidthSigma),
            fSlopeSigma(orig.fSlopeSigma), fInterceptSigma(orig.fInterceptSigma), fTotalPowerSigma(orig.fTotalPowerSigma)
    {}

    TProcessedMPTData::~TProcessedMPTData()
    {}

    TObject* TProcessedMPTData::Clone(const char* newname)
    {
        TProcessedMPTData* newData = new TProcessedMPTData(*this);
        return newData;
    }

    TProcessedMPTData& TProcessedMPTData::operator=(const TProcessedMPTData& rhs)
    {
        fComponent = rhs.fComponent; fTrackID = rhs.fTrackID; fEventSequenceID = rhs.fEventSequenceID; fIsCut = rhs.fIsCut;
        fMVAClassifier = rhs.fMVAClassifier; fMainband = rhs.fMainband; fAxialFrequency = rhs.fAxialFrequency;
        fAcquisitionID = rhs.fAcquisitionID;
        fStartTimeInRunC = rhs.fStartTimeInRunC; fStartTimeInAcq = rhs.fStartTimeInAcq; fEndTimeInRunC = rhs.fEndTimeInRunC; fTimeLength = rhs.fTimeLength;
        fStartFrequency = rhs.fStartFrequency; fEndFrequency = rhs.fEndFrequency; fFrequencyWidth = rhs.fFrequencyWidth;
        fSlope = rhs.fSlope; fIntercept = rhs.fIntercept; fTotalPower = rhs.fTotalPower;
        fStartTimeInRunCSigma = rhs.fStartTimeInRunCSigma; fEndTimeInRunCSigma = rhs.fEndTimeInRunCSigma; fTimeLengthSigma = rhs.fTimeLengthSigma;
        fStartFrequencySigma = rhs.fStartFrequencySigma; fEndFrequencySigma = rhs.fEndFrequencySigma; fFrequencyWidthSigma = rhs.fFrequencyWidthSigma;
        fSlopeSigma = rhs.fSlopeSigma; fInterceptSigma = rhs.fInterceptSigma; fTotalPowerSigma = rhs.fTotalPowerSigma;
        return *this;
    }

    void TProcessedMPTData::Load(const KTProcessedMPTData& data)
    {
        fComponent = data.GetComponent(); fTrackID = data.GetMainTrack().GetTrackID(); fEventSequenceID = data.GetMainTrack().GetEventSequenceID(); fIsCut = data.GetMainTrack().GetIsCut();
        fMVAClassifier = data.GetMainTrack().GetMVAClassifier(); fMainband = data.GetMainTrack().GetMainband(); fAxialFrequency = data.GetAxialFrequency();
        fAcquisitionID = data.GetMainTrack().GetAcquisitionID();
        fStartTimeInRunC = data.GetMainTrack().GetStartTimeInRunC(); fStartTimeInAcq = data.GetMainTrack().GetStartTimeInAcq(); fEndTimeInRunC = data.GetMainTrack().GetEndTimeInRunC(); fTimeLength = data.GetMainTrack().GetTimeLength();
        fStartFrequency = data.GetMainTrack().GetStartFrequency(); fEndFrequency = data.GetMainTrack().GetEndFrequency(); fFrequencyWidth = data.GetMainTrack().GetFrequencyWidth();
        fSlope = data.GetMainTrack().GetSlope(); fIntercept = data.GetMainTrack().GetIntercept(); fTotalPower = data.GetMainTrack().GetTotalPower();
        fStartTimeInRunCSigma = data.GetMainTrack().GetStartTimeInRunCSigma(); fEndTimeInRunCSigma = data.GetMainTrack().GetEndTimeInRunCSigma(); fTimeLengthSigma = data.GetMainTrack().GetTimeLengthSigma();
        fStartFrequencySigma = data.GetMainTrack().GetStartFrequencySigma(); fEndFrequencySigma = data.GetMainTrack().GetEndFrequencySigma(); fFrequencyWidthSigma = data.GetMainTrack().GetFrequencyWidthSigma();
        fSlopeSigma = data.GetMainTrack().GetSlopeSigma(); fInterceptSigma = data.GetMainTrack().GetInterceptSigma(); fTotalPowerSigma = data.GetMainTrack().GetTotalPowerSigma();
        return;
    }
    void TProcessedMPTData::Unload(KTProcessedMPTData& mptData) const
    {
        KTProcessedTrackData* data = new KTProcessedTrackData();

        data->SetComponent(fComponent); data->SetTrackID(fTrackID); data->SetEventSequenceID(fEventSequenceID); data->SetIsCut(fIsCut);
        data->SetMVAClassifier(fMVAClassifier); data->SetMainband(fMainband);
        data->SetAcquisitionID(fAcquisitionID);
        data->SetStartTimeInRunC(fStartTimeInRunC); data->SetStartTimeInAcq(fStartTimeInAcq); data->SetEndTimeInRunC(fEndTimeInRunC); data->SetTimeLength(fTimeLength);
        data->SetStartFrequency(fStartFrequency); data->SetEndFrequency(fEndFrequency); data->SetFrequencyWidth(fFrequencyWidth);
        data->SetSlope(fSlope); data->SetIntercept(fIntercept); data->SetTotalPower(fTotalPower);
        data->SetStartTimeInRunCSigma(fStartTimeInRunCSigma); data->SetEndTimeInRunCSigma(fEndTimeInRunCSigma); data->SetTimeLengthSigma(fTimeLengthSigma);
        data->SetStartFrequencySigma(fStartFrequencySigma); data->SetEndFrequencySigma(fEndFrequencySigma); data->SetFrequencyWidthSigma(fFrequencyWidthSigma);
        data->SetSlopeSigma(fSlopeSigma); data->SetInterceptSigma(fInterceptSigma); data->SetTotalPowerSigma(fTotalPowerSigma);

        mptData.SetComponent(fComponent);
        mptData.SetMainTrack(*data);
        mptData.SetAxialFrequency(fAxialFrequency);
        return;
    }


    //************************
    // TMultiTrackEventData
    //************************

    TMultiTrackEventData::TMultiTrackEventData() :
            fComponent(0), fAcquisitionID(0), fEventID(0), fTotalEventSequences(0),
            fStartTimeInRunC(0.), fStartTimeInAcq(0.), fEndTimeInRunC(0.),fTimeLength(0.),
            fStartFrequency(0.), fEndFrequency(0.), fMinimumFrequency(0.), fMaximumFrequency(0.), fFrequencyWidth(0.),
            fStartTimeInRunCSigma(0.), fEndTimeInRunCSigma(0.), fTimeLengthSigma(0.),
            fStartFrequencySigma(0.), fEndFrequencySigma(0.), fFrequencyWidthSigma(0.),
            fFirstTrackID(0), fFirstTrackTimeLength(0.), fFirstTrackFrequencyWidth(0.), fFirstTrackSlope(0.), fFirstTrackIntercept(0.), fFirstTrackTotalPower(0.),
            fUnknownEventTopology(false)
    {
        // this cannot be initialized in the initializer list because ROOT
        fTracks = new TClonesArray("Katydid::TProcessedTrackData", 20);
    }

    TMultiTrackEventData::TMultiTrackEventData(const TMultiTrackEventData& orig) :
            fComponent(orig.fComponent), fAcquisitionID(orig.fAcquisitionID), fEventID(orig.fEventID), fTotalEventSequences(orig.fTotalEventSequences),
            fStartTimeInRunC(orig.fStartTimeInRunC), fStartTimeInAcq(orig.fStartTimeInAcq), fEndTimeInRunC(orig.fEndTimeInRunC),fTimeLength(orig.fTimeLength),
            fStartFrequency(orig.fStartFrequency), fEndFrequency(orig.fEndFrequency), fMinimumFrequency(orig.fMaximumFrequency), fMaximumFrequency(orig.fMinimumFrequency), fFrequencyWidth(orig.fFrequencyWidth),
            fStartTimeInRunCSigma(orig.fStartTimeInRunCSigma), fEndTimeInRunCSigma(orig.fEndTimeInRunCSigma), fTimeLengthSigma(orig.fTimeLengthSigma),
            fStartFrequencySigma(orig.fStartFrequencySigma), fEndFrequencySigma(orig.fEndFrequencySigma), fFrequencyWidthSigma(orig.fFrequencyWidthSigma),
            fFirstTrackID(orig.fFirstTrackID), fFirstTrackTimeLength(orig.fFirstTrackTimeLength), fFirstTrackFrequencyWidth(orig.fFirstTrackFrequencyWidth), fFirstTrackSlope(orig.fFirstTrackSlope), fFirstTrackIntercept(orig.fFirstTrackIntercept), fFirstTrackTotalPower(orig.fFirstTrackTotalPower),
            fUnknownEventTopology(orig.fUnknownEventTopology)
    {
        // this cannot be initialized in the initializer list because ROOT
        fTracks = new TClonesArray(*orig.fTracks);
    }

    TMultiTrackEventData::TMultiTrackEventData(const KTMultiTrackEventData& orig) :
            fComponent(0), fAcquisitionID(0), fEventID(0), fTotalEventSequences(0),
            fStartTimeInRunC(0.), fStartTimeInAcq(0.), fEndTimeInRunC(0.),fTimeLength(0.),
            fStartFrequency(0.), fEndFrequency(0.), fMinimumFrequency(0.), fMaximumFrequency(0.), fFrequencyWidth(0.),
            fStartTimeInRunCSigma(0.), fEndTimeInRunCSigma(0.), fTimeLengthSigma(0.),
            fStartFrequencySigma(0.), fEndFrequencySigma(0.), fFrequencyWidthSigma(0.),
            fFirstTrackID(0), fFirstTrackTimeLength(0.), fFirstTrackFrequencyWidth(0.), fFirstTrackSlope(0.), fFirstTrackIntercept(0.), fFirstTrackTotalPower(0.),
            fUnknownEventTopology(false)
    {
        // this cannot be initialized in the initializer list because ROOT
        fTracks = new TClonesArray("Katydid::TProcessedTrackData", 20);
        Load(orig);
    }

    TMultiTrackEventData::~TMultiTrackEventData()
    {
        fTracks->Clear();
    }

    TObject* TMultiTrackEventData::Clone(const char* newname)
    {
        TMultiTrackEventData* newData = new TMultiTrackEventData(*this);
        return newData;
    }

    TMultiTrackEventData& TMultiTrackEventData::operator=(const TMultiTrackEventData& rhs)
    {
        fComponent = rhs.fComponent; fAcquisitionID = rhs.fAcquisitionID; fEventID = rhs.fEventID; fTotalEventSequences = rhs.fTotalEventSequences;
        fStartTimeInRunC = rhs.fStartTimeInRunC; fStartTimeInAcq = rhs.fStartTimeInAcq; fEndTimeInRunC = rhs.fEndTimeInRunC;fTimeLength = rhs.fTimeLength;
        fStartFrequency = rhs.fStartFrequency; fEndFrequency = rhs.fEndFrequency; fMinimumFrequency = rhs.fMinimumFrequency; fMaximumFrequency = rhs.fMaximumFrequency; fFrequencyWidth = rhs.fFrequencyWidth;
        fStartTimeInRunCSigma = rhs.fStartTimeInRunCSigma; fEndTimeInRunCSigma = rhs.fEndTimeInRunCSigma; fTimeLengthSigma = rhs.fTimeLengthSigma;
        fStartFrequencySigma = rhs.fStartFrequencySigma; fEndFrequencySigma = rhs.fEndFrequencySigma; fFrequencyWidthSigma = rhs.fFrequencyWidthSigma;
        fFirstTrackID = rhs.fFirstTrackID; fFirstTrackTimeLength = rhs.fFirstTrackTimeLength; fFirstTrackFrequencyWidth = rhs.fFirstTrackFrequencyWidth; fFirstTrackSlope = rhs.fFirstTrackSlope; fFirstTrackIntercept = rhs.fFirstTrackIntercept; fFirstTrackTotalPower = rhs.fFirstTrackTotalPower;
        fUnknownEventTopology = rhs.fUnknownEventTopology;
        fTracks->Clear(); (*fTracks) = *(rhs.fTracks);
        return *this;
    }

    void TMultiTrackEventData::Load(const KTMultiTrackEventData& data)
    {
        fComponent = data.GetComponent(); fAcquisitionID = data.GetAcquisitionID(); fEventID = data.GetEventID(); fTotalEventSequences = data.GetTotalEventSequences();
        fStartTimeInRunC = data.GetStartTimeInRunC(); fStartTimeInAcq = data.GetStartTimeInAcq(); fEndTimeInRunC = data.GetEndTimeInRunC();fTimeLength = data.GetTimeLength();
        fStartFrequency = data.GetStartFrequency(); fEndFrequency = data.GetEndFrequency(); fMinimumFrequency = data.GetMinimumFrequency(); fMaximumFrequency = data.GetMaximumFrequency(); fFrequencyWidth = data.GetFrequencyWidth();
        fStartTimeInRunCSigma = data.GetStartTimeInRunCSigma(); fEndTimeInRunCSigma = data.GetEndTimeInRunCSigma(); fTimeLengthSigma = data.GetTimeLengthSigma();
        fStartFrequencySigma = data.GetStartFrequencySigma(); fEndFrequencySigma = data.GetEndFrequencySigma(); fFrequencyWidthSigma = data.GetFrequencyWidthSigma();
        fFirstTrackID = data.GetFirstTrackID(); fFirstTrackTimeLength = data.GetFirstTrackTimeLength(); fFirstTrackFrequencyWidth = data.GetFirstTrackFrequencyWidth(); fFirstTrackSlope = data.GetFirstTrackSlope(); fFirstTrackIntercept = data.GetFirstTrackIntercept(); fFirstTrackTotalPower = data.GetFirstTrackTotalPower();
        fUnknownEventTopology = data.GetUnknownEventTopology();
        Int_t nTracks = (Int_t)data.GetNTracks();
        fTracks->Clear(); fTracks->Expand(nTracks);
        Int_t iTrack = 0;
        for (TrackSetCIt trIt = data.GetTracksBegin(); trIt != data.GetTracksEnd(); ++trIt)
        {
            TProcessedTrackData* track = new((*fTracks)[iTrack]) TProcessedTrackData(trIt->fProcTrack);
            ++iTrack;
        }
        return;
    }
    void TMultiTrackEventData::Unload(KTMultiTrackEventData& data) const
    {
        data.ClearTracks(); // do this first, since it clears some of the member variables other than just fTracks
        data.SetComponent(fComponent); data.SetAcquisitionID(fAcquisitionID); data.SetEventID(fEventID); data.SetTotalEventSequences(fTotalEventSequences);
        data.SetStartTimeInRunC(fStartTimeInRunC); data.SetStartTimeInAcq(fStartTimeInAcq); data.SetEndTimeInRunC(fEndTimeInRunC); data.SetTimeLength(fTimeLength);
        data.SetStartFrequency(fStartFrequency); data.SetEndFrequency(fEndFrequency); data.SetMinimumFrequency(fMinimumFrequency); data.SetMaximumFrequency(fMaximumFrequency); data.SetFrequencyWidth(fFrequencyWidth);
        data.SetStartTimeInRunCSigma(fStartTimeInRunCSigma); data.SetEndTimeInRunCSigma(fEndTimeInRunCSigma); data.SetTimeLengthSigma(fTimeLengthSigma);
        data.SetStartFrequencySigma(fStartFrequencySigma); data.SetEndFrequencySigma(fEndFrequencySigma); data.SetFrequencyWidthSigma(fFrequencyWidthSigma);
        data.SetFirstTrackID(fFirstTrackID); data.SetFirstTrackTimeLength(fFirstTrackTimeLength); data.SetFirstTrackFrequencyWidth(fFirstTrackFrequencyWidth); data.SetFirstTrackSlope(fFirstTrackSlope); data.SetFirstTrackIntercept(fFirstTrackIntercept); data.SetFirstTrackTotalPower(fFirstTrackTotalPower);
        data.SetUnknownEventTopology(fUnknownEventTopology);

        Int_t nTracks = fTracks->GetSize();
        Nymph::KTDataPtr dummyData;
        KTProcessedTrackData& procTrack = dummyData->Of< KTProcessedTrackData >();
        AllTrackData track( dummyData, procTrack );

        for (Int_t iTrack = 0; iTrack < nTracks; ++iTrack)
        {
            ((TProcessedTrackData*)((*fTracks)[iTrack]))->Unload(procTrack);
            data.AddTrack(track);
        }
        return;
    }

    //************************
    // TClassifiedEventData
    //************************

    TClassifiedEventData::TClassifiedEventData() :
            fComponent(0), fAcquisitionID(0), fEventID(0), fTotalEventSequences(0),
            fStartTimeInRunC(0.), fStartTimeInAcq(0.), fEndTimeInRunC(0.),fTimeLength(0.),
            fStartFrequency(0.), fEndFrequency(0.), fMinimumFrequency(0.), fMaximumFrequency(0.), fFrequencyWidth(0.),
            fStartTimeInRunCSigma(0.), fEndTimeInRunCSigma(0.), fTimeLengthSigma(0.),
            fStartFrequencySigma(0.), fEndFrequencySigma(0.), fFrequencyWidthSigma(0.),
            fFirstTrackID(0), fFirstTrackTimeLength(0.), fFirstTrackFrequencyWidth(0.), fFirstTrackSlope(0.), fFirstTrackIntercept(0.), fFirstTrackTotalPower(0.),
            fUnknownEventTopology(false)
    {
        // this cannot be initialized in the initializer list because ROOT
        fTracks = new TClonesArray("Katydid::TProcessedTrackData", 20);
        fClassifierResults = new TClonesArray("Katydid::TClassifierResultsData", 20);
    }

    TClassifiedEventData::TClassifiedEventData(const TClassifiedEventData& orig) :
            fComponent(orig.fComponent), fAcquisitionID(orig.fAcquisitionID), fEventID(orig.fEventID), fTotalEventSequences(orig.fTotalEventSequences),
            fStartTimeInRunC(orig.fStartTimeInRunC), fStartTimeInAcq(orig.fStartTimeInAcq), fEndTimeInRunC(orig.fEndTimeInRunC),fTimeLength(orig.fTimeLength),
            fStartFrequency(orig.fStartFrequency), fEndFrequency(orig.fEndFrequency), fMinimumFrequency(orig.fMaximumFrequency), fMaximumFrequency(orig.fMinimumFrequency), fFrequencyWidth(orig.fFrequencyWidth),
            fStartTimeInRunCSigma(orig.fStartTimeInRunCSigma), fEndTimeInRunCSigma(orig.fEndTimeInRunCSigma), fTimeLengthSigma(orig.fTimeLengthSigma),
            fStartFrequencySigma(orig.fStartFrequencySigma), fEndFrequencySigma(orig.fEndFrequencySigma), fFrequencyWidthSigma(orig.fFrequencyWidthSigma),
            fFirstTrackID(orig.fFirstTrackID), fFirstTrackTimeLength(orig.fFirstTrackTimeLength), fFirstTrackFrequencyWidth(orig.fFirstTrackFrequencyWidth), fFirstTrackSlope(orig.fFirstTrackSlope), fFirstTrackIntercept(orig.fFirstTrackIntercept), fFirstTrackTotalPower(orig.fFirstTrackTotalPower),
            fUnknownEventTopology(orig.fUnknownEventTopology)
    {
        // this cannot be initialized in the initializer list because ROOT
        fTracks = new TClonesArray(*orig.fTracks);
        fClassifierResults = new TClonesArray(*orig.fClassifierResults);
    }

    TClassifiedEventData::TClassifiedEventData(const KTMultiTrackEventData& orig) :
            fComponent(0), fAcquisitionID(0), fEventID(0), fTotalEventSequences(0),
            fStartTimeInRunC(0.), fStartTimeInAcq(0.), fEndTimeInRunC(0.),fTimeLength(0.),
            fStartFrequency(0.), fEndFrequency(0.), fMinimumFrequency(0.), fMaximumFrequency(0.), fFrequencyWidth(0.),
            fStartTimeInRunCSigma(0.), fEndTimeInRunCSigma(0.), fTimeLengthSigma(0.),
            fStartFrequencySigma(0.), fEndFrequencySigma(0.), fFrequencyWidthSigma(0.),
            fFirstTrackID(0), fFirstTrackTimeLength(0.), fFirstTrackFrequencyWidth(0.), fFirstTrackSlope(0.), fFirstTrackIntercept(0.), fFirstTrackTotalPower(0.),
            fUnknownEventTopology(false)
    {
        // this cannot be initialized in the initializer list because ROOT
        fTracks = new TClonesArray("Katydid::TProcessedTrackData", 20);
        fClassifierResults = new TClonesArray("Katydid::TClassifierResultsData", 20);
        Load(orig);
    }

    TClassifiedEventData::~TClassifiedEventData()
    {
        fTracks->Clear();
        fClassifierResults->Clear();
    }

    TObject* TClassifiedEventData::Clone(const char* newname)
    {
        TClassifiedEventData* newData = new TClassifiedEventData(*this);
        return newData;
    }

    TClassifiedEventData& TClassifiedEventData::operator=(const TClassifiedEventData& rhs)
    {
        fComponent = rhs.fComponent; fAcquisitionID = rhs.fAcquisitionID; fEventID = rhs.fEventID; fTotalEventSequences = rhs.fTotalEventSequences;
        fStartTimeInRunC = rhs.fStartTimeInRunC; fStartTimeInAcq = rhs.fStartTimeInAcq; fEndTimeInRunC = rhs.fEndTimeInRunC;fTimeLength = rhs.fTimeLength;
        fStartFrequency = rhs.fStartFrequency; fEndFrequency = rhs.fEndFrequency; fMinimumFrequency = rhs.fMinimumFrequency; fMaximumFrequency = rhs.fMaximumFrequency; fFrequencyWidth = rhs.fFrequencyWidth;
        fStartTimeInRunCSigma = rhs.fStartTimeInRunCSigma; fEndTimeInRunCSigma = rhs.fEndTimeInRunCSigma; fTimeLengthSigma = rhs.fTimeLengthSigma;
        fStartFrequencySigma = rhs.fStartFrequencySigma; fEndFrequencySigma = rhs.fEndFrequencySigma; fFrequencyWidthSigma = rhs.fFrequencyWidthSigma;
        fFirstTrackID = rhs.fFirstTrackID; fFirstTrackTimeLength = rhs.fFirstTrackTimeLength; fFirstTrackFrequencyWidth = rhs.fFirstTrackFrequencyWidth; fFirstTrackSlope = rhs.fFirstTrackSlope; fFirstTrackIntercept = rhs.fFirstTrackIntercept; fFirstTrackTotalPower = rhs.fFirstTrackTotalPower;
        fUnknownEventTopology = rhs.fUnknownEventTopology;
        fTracks->Clear(); (*fTracks) = *(rhs.fTracks);
        fClassifierResults->Clear(); (*fClassifierResults) = *(rhs.fClassifierResults);
        return *this;
    }

    void TClassifiedEventData::Load(const KTMultiTrackEventData& data)
    {
        fComponent = data.GetComponent(); fAcquisitionID = data.GetAcquisitionID(); fEventID = data.GetEventID(); fTotalEventSequences = data.GetTotalEventSequences();
        fStartTimeInRunC = data.GetStartTimeInRunC(); fStartTimeInAcq = data.GetStartTimeInAcq(); fEndTimeInRunC = data.GetEndTimeInRunC();fTimeLength = data.GetTimeLength();
        fStartFrequency = data.GetStartFrequency(); fEndFrequency = data.GetEndFrequency(); fMinimumFrequency = data.GetMinimumFrequency(); fMaximumFrequency = data.GetMaximumFrequency(); fFrequencyWidth = data.GetFrequencyWidth();
        fStartTimeInRunCSigma = data.GetStartTimeInRunCSigma(); fEndTimeInRunCSigma = data.GetEndTimeInRunCSigma(); fTimeLengthSigma = data.GetTimeLengthSigma();
        fStartFrequencySigma = data.GetStartFrequencySigma(); fEndFrequencySigma = data.GetEndFrequencySigma(); fFrequencyWidthSigma = data.GetFrequencyWidthSigma();
        fFirstTrackID = data.GetFirstTrackID(); fFirstTrackTimeLength = data.GetFirstTrackTimeLength(); fFirstTrackFrequencyWidth = data.GetFirstTrackFrequencyWidth(); fFirstTrackSlope = data.GetFirstTrackSlope(); fFirstTrackIntercept = data.GetFirstTrackIntercept(); fFirstTrackTotalPower = data.GetFirstTrackTotalPower();
        fUnknownEventTopology = data.GetUnknownEventTopology();
        Int_t nTracks = (Int_t)data.GetNTracks();
        fTracks->Clear(); fTracks->Expand(nTracks);
        fClassifierResults->Clear(); fClassifierResults->Expand(nTracks);
        Int_t iTrack = 0;
        for (TrackSetCIt trIt = data.GetTracksBegin(); trIt != data.GetTracksEnd(); ++trIt)
        {
            TProcessedTrackData* track = new((*fTracks)[iTrack]) TProcessedTrackData(trIt->fProcTrack);
            TClassifierResultsData* classifier = new((*fClassifierResults)[iTrack]) TClassifierResultsData(trIt->fData->Of< KTClassifierResultsData >());
            ++iTrack;
        }
        return;
    }
    void TClassifiedEventData::Unload(KTMultiTrackEventData& data) const
    {
        data.ClearTracks(); // do this first, since it clears some of the member variables other than just fTracks
        data.SetComponent(fComponent); data.SetAcquisitionID(fAcquisitionID); data.SetEventID(fEventID); data.SetTotalEventSequences(fTotalEventSequences);
        data.SetStartTimeInRunC(fStartTimeInRunC); data.SetStartTimeInAcq(fStartTimeInAcq); data.SetEndTimeInRunC(fEndTimeInRunC); data.SetTimeLength(fTimeLength);
        data.SetStartFrequency(fStartFrequency); data.SetEndFrequency(fEndFrequency); data.SetMinimumFrequency(fMinimumFrequency); data.SetMaximumFrequency(fMaximumFrequency); data.SetFrequencyWidth(fFrequencyWidth);
        data.SetStartTimeInRunCSigma(fStartTimeInRunCSigma); data.SetEndTimeInRunCSigma(fEndTimeInRunCSigma); data.SetTimeLengthSigma(fTimeLengthSigma);
        data.SetStartFrequencySigma(fStartFrequencySigma); data.SetEndFrequencySigma(fEndFrequencySigma); data.SetFrequencyWidthSigma(fFrequencyWidthSigma);
        data.SetFirstTrackID(fFirstTrackID); data.SetFirstTrackTimeLength(fFirstTrackTimeLength); data.SetFirstTrackFrequencyWidth(fFirstTrackFrequencyWidth); data.SetFirstTrackSlope(fFirstTrackSlope); data.SetFirstTrackIntercept(fFirstTrackIntercept); data.SetFirstTrackTotalPower(fFirstTrackTotalPower);
        data.SetUnknownEventTopology(fUnknownEventTopology);

        Int_t nTracks = fTracks->GetSize();
        Nymph::KTDataPtr dummyData;
        KTProcessedTrackData& procTrack = dummyData->Of< KTProcessedTrackData >();
        KTClassifierResultsData& classData = dummyData->Of< KTClassifierResultsData >();
        AllTrackData track( dummyData, procTrack );

        for (Int_t iTrack = 0; iTrack < nTracks; ++iTrack)
        {
            ((TProcessedTrackData*)((*fTracks)[iTrack]))->Unload(procTrack);
            ((TClassifierResultsData*)((*fClassifierResults)[iTrack]))->Unload(classData);
            data.AddTrack(track);
        }
        return;
    }

    //***********************
    // TDiscriminatedPoint
    //***********************

    TDiscriminatedPoint::TDiscriminatedPoint() :
            TObject(),
            fTimeInRunC(0), fFrequency(0), fAmplitude(0), fTimeInAcq(0),
            fMean(0), fVariance(0),fNeighborhoodAmplitude(0)
    {}

    TDiscriminatedPoint::TDiscriminatedPoint(const TDiscriminatedPoint& orig) :
            TObject(orig),
            fTimeInRunC(orig.fTimeInRunC), fFrequency(orig.fFrequency), fAmplitude(orig.fAmplitude), fTimeInAcq(orig.fTimeInAcq),
            fMean(orig.fMean), fVariance(orig.fVariance),fNeighborhoodAmplitude(orig.fNeighborhoodAmplitude)
    {}

    TDiscriminatedPoint::~TDiscriminatedPoint()
    {}

    TObject* TDiscriminatedPoint::Clone(const char* newname)
    {
        TDiscriminatedPoint* newData = new TDiscriminatedPoint(*this);
        return newData;
    }

    TDiscriminatedPoint& TDiscriminatedPoint::operator=(const TDiscriminatedPoint& rhs)
    {
        fTimeInRunC = rhs.fTimeInRunC; fFrequency = rhs.fFrequency; fAmplitude = rhs.fAmplitude; fTimeInAcq = rhs.fTimeInAcq;
        fMean = rhs.fMean; fVariance = rhs.fVariance; fNeighborhoodAmplitude = rhs.fNeighborhoodAmplitude;
        return *this;
    }

    // void TDiscriminatedPoint::Load(const KTDiscriminatedPoint& data)
    // {
    //     fTimeInRunC = data.GetTimeInRunC(); fFrequency = data.GetFrequency(); fAmplitude = data.GetAmplitude(); fTimeInAcq = data.GetTimeInAcq();
    //     fMean = data.GetMean(); fAmplitude = data.GetAmplitude(); fNeighborhoodAmplitude = data.GetNeighborhoodAmplitude();
    //     return;
    // }
    // void TDiscriminatedPoint::Unload(KTDiscriminatedPoint& data) const
    // {
    //     data.SetTimeInRunC(fTimeInRunC); data.SetFrequency(fFrequency); data.SetAmplitude(fAmplitude); data.SetTimeInAcq(fTimeInAcq); 
    //     data.SetMean(fMean); data.SetVariance(fVariance); data.SetNeighborhoodAmplitude(fNeighborhoodAmplitude);
    //     return;
    // }



    //************************
    // TSparseWaterfallCandidate
    //************************

    TSparseWaterfallCandidateData::TSparseWaterfallCandidateData() :
            fComponent(0), fAcquisitionID(0), fCandidateID(0),
            fTimeInRunC(0.), fTimeLength(0.),
            fMinFrequency(0.), fMaxFrequency(0.), fFrequencyWidth(0.)
    {
        // this cannot be initialized in the initializer list because ROOT
        fPoints = new TClonesArray("Katydid::TDiscriminatedPoint", 20);
    }

    TSparseWaterfallCandidateData::TSparseWaterfallCandidateData(const TSparseWaterfallCandidateData& orig) :
            fComponent(orig.fComponent), fAcquisitionID(orig.fAcquisitionID), fCandidateID(orig.fCandidateID),
            fTimeInRunC(orig.fTimeInRunC), fTimeLength(orig.fTimeLength),
            fMinFrequency(orig.fMinFrequency), fMaxFrequency(orig.fMaxFrequency), fFrequencyWidth(orig.fFrequencyWidth)
    {
        // this cannot be initialized in the initializer list because ROOT
        fPoints = new TClonesArray(*orig.fPoints);
    }

    TSparseWaterfallCandidateData::~TSparseWaterfallCandidateData()
    {
        fPoints->Clear();
    }

    TObject* TSparseWaterfallCandidateData::Clone(const char* newname)
    {
        TSparseWaterfallCandidateData* newData = new TSparseWaterfallCandidateData(*this);
        return newData;
    }

    TSparseWaterfallCandidateData& TSparseWaterfallCandidateData::operator=(const TSparseWaterfallCandidateData& rhs)
    {
        fComponent = rhs.fComponent; fAcquisitionID = rhs.fAcquisitionID; fCandidateID = rhs.fCandidateID;
        fTimeInRunC = rhs.fTimeInRunC; fTimeLength = rhs.fTimeLength; 
        fMinFrequency = rhs.fMinFrequency; fMaxFrequency = rhs.fMaxFrequency; fFrequencyWidth = rhs.fFrequencyWidth;
        fPoints->Clear(); (*fPoints) = *(rhs.fPoints);
        return *this;
    }

    // void TSparseWaterfallCandidateData::Load(const KTSparseWaterfallCandidateData& data)
    // {
    //     fComponent = data.GetComponent(); fAcquisitionID = data.GetAcquisitionID(); fCandidateID = data.GetCandidateID();
    //     fTimeInRunC = data.GetTimeInRunC(); fTimeLength = data.GetTimeLength();
    //     fMinFrequency = data.GetMinFrequency(); fMaxFrequency = data.GetMaxFrequency(); fFrequencyWidth = data.GetFrequencyWidth();
    //     Int_t nPoints = (Int_t)data.GetNPoints();
    //     fPoints->Clear(); fPoints->Expand(nPoints);
    //     Int_t iPoint = 0;
    //     for (PointSetCIt trIt = data.GetPointsBegin(); trIt != data.GetPointsEnd(); ++trIt)
    //     {
    //         TDiscriminatedPoint* point = new((*fPoints)[iPoint]) TDiscriminatedPoint(trIt->fPoint);
    //         ++iTrack;
    //     }
    //     return;
    // }
    // void TSparseWaterfallCandidateData::Unload(KTSparseWaterfallCandidateData& data) const
    // {
    //     data.ClearPoints(); // do this first, since it clears some of the member variables other than just fTracks
    //     data.SetComponent(fComponent); data.SetAcquisitionID(fAcquisitionID); data.SetCandidateID(fCandidateID); 
    //     data.SetTimeInRunC(fTimeInRunC); data.SetTimeLength(fTimeLength);
    //     data.SetMinFrequency(fMinFrequency); data.SetMaxFrequency(fMaxFrequency); data.SetFrequencyWidth(fFrequencyWidth);

    //     Int_t nPoints = fPoints->GetSize();
    //     Nymph::KTDataPtr dummyData;
    //     KTProcessedTrackData& discPoint = dummyData->Of< KTDiscriminatedPoint >();
    //     AllTrackData point( dummyData, discPoint );

    //     for (Int_t iTrack = 0; iTrack < nTracks; ++iTrack)
    //     {
    //         ((TProcessedTrackData*)((*fTracks)[iTrack]))->Unload(discPoint);
    //         data.AddTrack(track);
    //     }
    //     return;
    // }

}
