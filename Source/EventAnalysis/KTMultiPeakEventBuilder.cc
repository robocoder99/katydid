/*
 * KTMultiPeakEventBuilder.cc
 *
 *  Created on: Dec 7, 2015
 *      Author: N.S. Oblath
 */

#include "KTMultiPeakEventBuilder.hh"

#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTMultiTrackEventData.hh"
#include "KTNOFactory.hh"
#include "KTParam.hh"
#include "KTProcessedTrackData.hh"

#include <list>

#ifndef NDEBUG
#include <sstream>
#endif

using std::list;
using std::set;
using std::vector;

namespace Katydid
{
    KTLOGGER(tclog, "KTMultiPeakEventBuilder");

    KT_REGISTER_PROCESSOR(KTMultiPeakEventBuilder, "multi-peak-event-builder");

    KTMultiPeakEventBuilder::KTMultiPeakEventBuilder(const std::string& name) :
            KTPrimaryProcessor(name),
            fSidebandTimeTolerance(0.),
            fJumpTimeTolerance(0.),
            fJumpFreqTolerance(1.),
            fTimeBinWidth(1),
            fFreqBinWidth(1.),
            fCompTracks(1),
            fMPTracks(1),
            fCandidates(),
            fDataCount(0),
            fEventSignal("event", this),
            fEventsDoneSignal("clustering-done", this),
            fTakeTrackSlot("track", this, &KTMultiPeakEventBuilder::TakeTrack)
    //        fDoClusterSlot("do-cluster-trigger", this, &KTMultiPeakEventBuilder::Run)
    {
        RegisterSlot("do-clustering", this, &KTMultiPeakEventBuilder::DoClusteringSlot);
    }

    KTMultiPeakEventBuilder::~KTMultiPeakEventBuilder()
    {
    }

    bool KTMultiPeakEventBuilder::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        SetSidebandTimeTolerance(node->GetValue("sideband-time-tol", GetSidebandTimeTolerance()));
        SetJumpTimeTolerance(node->GetValue("jump-time-tol", GetJumpTimeTolerance()));
        SetJumpFreqTolerance(node->GetValue("jump-freq-tol", GetJumpFreqTolerance()));

        return true;
    }

    bool KTMultiPeakEventBuilder::TakeTrack(KTProcessedTrackData& track)
    {
        // ignore the track if it's been cut
        if (track.GetIsCut()) return true;

        // verify that we have the right number of components
        if (track.GetComponent() >= fCompTracks.size())
        {
            SetNComponents(track.GetComponent() + 1);
        }

        // copy the full track data
        fCompTracks[track.GetComponent()].insert(track);

        KTDEBUG(tclog, "Taking track: (" << track.GetStartTimeInRunC() << ", " << track.GetStartFrequency() << ", " << track.GetEndTimeInRunC() << ", " << track.GetEndFrequency());

        return true;
    }

    /*
    bool KTMultiPeakEventBuilder::TakePoint(double time, double frequency *//*, double amplitude*//*, unsigned component)
    {
        if (component >= fCompPoints.size())
        {
            SetNComponents(component + 1);
        }

        KTDBScan::Point newPoint(fNDimensions);
        newPoint(0) = time;
        newPoint(1) = frequency;
        fCompPoints[component].push_back(newPoint);

        KTDEBUG(tclog, "Point " << fCompPoints[component].size()-1 << " is now " << fCompPoints[component].back());

        return true;
    }
    */

    void KTMultiPeakEventBuilder::DoClusteringSlot()
    {
        if (! Run())
        {
            KTERROR(tclog, "An error occurred while running the event clustering");
        }
        return;
    }

    bool KTMultiPeakEventBuilder::Run()
    {
        return DoClustering();
    }

    bool KTMultiPeakEventBuilder::DoClustering()
    {
        if (! FindMultiPeakTracks())
        {
            KTERROR(tclog, "An error occurred while identifying multipeak tracks");
            return false;
        }

        if (! FindEvents())
        {
            KTERROR(tclog, "An error occurred while identifying events");
            return false;
        }

        KTDEBUG(tclog, "Event building complete");
        fEventsDoneSignal();

        return true;
    }

    bool KTMultiPeakEventBuilder::FindMultiPeakTracks()
    {
        // loop over components
        unsigned component = 0;
        for (vector< TrackSet >::const_iterator compIt = fCompTracks.begin(); compIt != fCompTracks.end(); ++compIt)
        {
            fMPTracks[component].clear();

            // loop over individual tracks
            TrackSetCIt trackIt = compIt->begin();
            if (trackIt == compIt->end()) continue;

            list< MultiPeakTrackRef > activeTrackRefs;
            activeTrackRefs.push_back(MultiPeakTrackRef());
            activeTrackRefs.begin()->InsertTrack(trackIt);

            ++trackIt;
            while (trackIt != compIt->end())
            {
                // loop over active track refs
                list< MultiPeakTrackRef >::iterator mptrIt = activeTrackRefs.begin();
                bool trackHasBeenAdded = false; // this will allow us to check all of the track refs for whether they're still active, even after adding the track to a ref
                while (mptrIt != activeTrackRefs.end())
                {
                    double deltaStartT = trackIt->GetStartTimeInRunC() - mptrIt->fMeanStartTimeInRunC;

                    // check to see if this track ref should no longer be active
                    if (trackIt->GetStartTimeInRunC() - mptrIt->fMeanStartTimeInRunC > fSidebandTimeTolerance)
                    {
                        // there's no way this track, or any following it in the set, will match in time
                        fMPTracks[component].insert(*mptrIt);
                        mptrIt = activeTrackRefs.erase(mptrIt); // this results in mptrIt being one element past the one that was erased
                    }
                    else
                    {
                        double deltaEndT = trackIt->GetEndTimeInRunC() - mptrIt->fMeanEndTimeInRunC;
                        // check if this track should be added to this track ref
                        if (! trackHasBeenAdded && fabs(deltaStartT) <= fSidebandTimeTolerance && fabs(deltaEndT) < fSidebandTimeTolerance)
                        {
                            // then this track matches this track ref
                            mptrIt->InsertTrack(trackIt);
                            trackHasBeenAdded = true;
                        }

                        ++mptrIt;
                    }
                } // while loop over active track refs
            } // while loop over tracks

            // now that we're done with tracks, all active track refs are finished as well
            list< MultiPeakTrackRef >::iterator mptrIt = activeTrackRefs.begin();
            while (mptrIt != activeTrackRefs.end())
            {
                fMPTracks[component].insert(*mptrIt);
                mptrIt = activeTrackRefs.erase(mptrIt); // this results in mptrIt being one element past the one that was erased
            }

            ++component;
        } // for loop over components

        return true;
    }

    bool KTMultiPeakEventBuilder::FindEvents()
    {
        KTPROG(tclog, "KTMultiPeakEventBuilder combining multi-peak tracks");
      
        // we're unpacking all components into a unified set of events, so this goes outside the loop
        // **note that if/when i change the type, this impacts the for loop over active events
        std::vector< std::pair< KTDataPtr, std::vector< double > > > active_events;

        // loop over components
        for (unsigned iComponent = 0; iComponent < fMPTracks.size(); ++iComponent)
        {
            KTDEBUG(tclog, "head-to-tail building events from MPTracks; component: " << iComponent);
            
            if (fMPTracks[iComponent].empty())
            {
                continue;
            }
            std::set< MultiPeakTrackRef, MTRComp >::iterator trackIt = fMPTracks[iComponent].begin();

            // loop over new Multi-Peak Tracks
            while (trackIt != fMPTracks[iComponent].end())
            {
                int track_assigned = -1; // keep track of which event the track when into
                
                // loop over active events and add this track to something
                for (std::vector< std::pair< KTDataPtr, std::vector< double > > >::const_iterator EventIt=active_events.begin(); EventIt != active_events.end();)
                {
                    // loop over end times in event
                    for (std::vector< double >::const_iterator end_timeIt=EventIt->second.begin(); end_timeIt != EventIt->second.end();)
                    {
                        // TODO: test if within frequency and time tolerance
                        // TODO: if within->add
                            // TODO: if this is the first event match, add this track to it; add this event index as the track_assigned
                            // TODO: else, add this event to the first match; remove this event from active events; continue so that the iterator doesn't get incremented
                            // TODO: break out of end_time loop
                        ++end_timeIt;
                    } // for loop over end times
                    ++EventIt;
                } // for loop over active events
                if (track_assigned != -1) // if the track didn't go into an active event, create a new one
                {
                    std::pair< KTDataPtr, std::vector< double > > new_event;
                    KTMultiTrackEventData& event = new_event.first->Of< KTMultiTrackEventData >();
                    // TODO: I probably need to set some data pointer attributes
                    // TODO: Then set some event attributes
                    // TODO: Then loop over and add tracks in the MP Track
                    // TODO: And set any attributes of those tracks
                    // TODO: Also, add the track end time to the end times vector <pair>.second
                    active_events.push_back(new_event);
                }


                /***************************************************/
                /*

                //active_events <configured prior to loop over tracks
                // is a vector< set< KTMultiTrackEventData, vector< EndTimes>>>
                this_track == <set by loop over tracks>

                track_added_somewhere = -1; // an int
                //events_to_conclude = []; // empty vector of int
                events_to_drop = []; // empty vector of ints
                
                for event in active_events:
                    if event.latest_end < this_track.start:
                        <add event to fCandidates, it is complete, do this now>
                        events_to_drop.pushback(my_iterator - my_vector.begin())
                        continue
                    if this_track_start in event.endtimes:
                        # track has not been put into an event yet, add it here
                        if track_added_somewhere == -1:
                            track_added_somewhere = my_iterator - my_vector.begin()
                            for peak in this_track:
                                event.add_track(peak)
                        # track has already been put somewhere, add this event there
                        else:
                            for track in event:
                                active_events[track_added_somwhere].add_track(track)
                            events_to_drop.push_back(my_iterator - my_vector.begin())
                if track_added_somewhere == -1:
                    <create a new event with only this track; pushback into vector>
                for iEvent in events_to_drop.rbegin():
                    active_events.erase(iEvent)
                */
                /***************************************************/

               

//                // loop over active track refs
//                list< MultiPeakTrackRef >::iterator mptrIt = activeTrackRefs.begin();
//                bool trackHasBeenAdded = false; // this will allow us to check all of the track refs for whether they're still active, even after adding the track to a ref
//                while (mptrIt != activeTrackRefs.end())
//                {
//                    double deltaStartT = trackIt->GetStartTimeInRunC() - mptrIt->fMeanStartTimeInRunC;
//
//                    // check to see if this track ref should no longer be active
//                    if (trackIt->GetStartTimeInRunC() - mptrIt->fMeanStartTimeInRunC > fSidebandTimeTolerance)
//                    {
//                        // there's no way this track, or any following it in the set, will match in time
//                        fMPTracks[component].insert(*mptrIt);
//                        mptrIt = activeTrackRefs.erase(mptrIt); // this results in mptrIt being one element past the one that was erased
//                    }
//                    else
//                    {
//                        double deltaEndT = trackIt->GetEndTimeInRunC() - mptrIt->fMeanEndTimeInRunC;
//                        // check if this track should be added to this track ref
//                        if (! trackHasBeenAdded && fabs(deltaStartT) <= fSidebandTimeTolerance && fabs(deltaEndT) < fSidebandTimeTolerance)
//                        {
//                            // then this track matches this track ref
//                            mptrIt->InsertTrack(trackIt);
//                            trackHasBeenAdded = true;
//                        }
//
//                        ++mptrIt;
//                    }
//                } // while loop over active track refs
                ++trackIt;
            } // while loop over tracks
//
//            // now that we're done with tracks, all active track refs are finished as well
//            list< MultiPeakTrackRef >::iterator mptrIt = activeTrackRefs.begin();
//            while (mptrIt != activeTrackRefs.end())
//            {
//                fMPTracks[component].insert(*mptrIt);
//                mptrIt = activeTrackRefs.erase(mptrIt); // this results in mptrIt being one element past the one that was erased
//            }

            //++component;
        } // for loop over components

       return true;
//       return false;
    }


    void KTMultiPeakEventBuilder::SetNComponents(unsigned nComps)
    {
        //TODO: fix this
        //fCompTracks.resize(nComps, vector< KTProcessedTrackData >());
        return;
    }

    KTMultiPeakEventBuilder::MultiPeakTrackRef::MultiPeakTrackRef() :
            fTrackRefs(),
            fMeanStartTimeInRunC(0.),
            fSumStartTimeInRunC(0.),
            fMeanEndTimeInRunC(0.),
            fSumEndTimeInRunC(0.)
    {}

    bool KTMultiPeakEventBuilder::MultiPeakTrackRef::InsertTrack(const TrackSetCIt& trackRef)
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


} /* namespace Katydid */
