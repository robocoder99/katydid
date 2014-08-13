/*
 * KTConsensusThresholding.cc
 *
 *  Created on: Aug 7, 2014
 *      Author: nsoblath
 */

#include "KTConsensusThresholding.hh"

//#include "KTKDTreeData.hh"
//#include "KTKDTree.hh"
#include "KTLogger.hh"
#include "KTParam.hh"
#include "KTSliceHeader.hh"

using std::string;


namespace Katydid
{
    KTLOGGER(kdlog, "KTConsensusThresholding");

    KT_REGISTER_PROCESSOR(KTConsensusThresholding, "consensus-thresholding");

    KTConsensusThresholding::KTConsensusThresholding(const std::string& name) :
            KTProcessor(name),
            fMembershipRadius(1.0),
            fMinNumberVotes(1),
            fKDTreeSignal("kd-tree-out", this),
            fKDTreeSlot("kd-tree-in", this, &KTConsensusThresholding::ConsensusVote)
    {
    }

    KTConsensusThresholding::~KTConsensusThresholding()
    {
    }

    bool KTConsensusThresholding::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;
        if (node->Has("membership-radius"))
        {
            SetMembershipRadius(node->GetValue< double >("membership-radius"));
        }
        if (node->Has("min-number-votes"))
        {
            SetMinNumberVotes(node->GetValue< unsigned >("min-number-votes"));
        }

        return true;
    }

    bool KTConsensusThresholding::ConsensusVote(KTKDTreeData& kdTreeData)
    {
        bool ret_val = true;
        unsigned nComponents = kdTreeData.GetNComponents();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTTreeIndex< double >* kdTree = kdTreeData.GetTreeIndex(iComponent);
            const std::vector< KTKDTreeData::Point >& setOfPoints = kdTreeData.GetSetOfPoints(iComponent);
            if (! this->ConsensusVoteComponent(kdTree, setOfPoints))
            {
                ret_val = false;
            }
        }
        return ret_val;
    }

    bool KTConsensusThresholding::ConsensusVoteComponent(const KTTreeIndex< double >* kdTree, const std::vector< KTKDTreeData::Point >& setOfPoints)
    {   
        int nPoints = 0;//this needs to come from the kdtree
        nPoints = kdTree->size();
        //int currentClusterID = 0;

        int* votes[nPoints][nPoints];
        //int* membership[nPoints];
        
        for (unsigned iPoint = 0; iPoint < nPoints; ++iPoint)
        {
            size_t nearestID = kdTree->knnSearch(iPoint, 2).GetIndicesAndDists()[1].second;

            double frequencyDelta = setOfPoints[nearestID].fCoords[1] - setOfPoints[iPoint].fCoords[1];
            double timeDelta = setOfPoints[nearestID].fCoords[0] - setOfPoints[iPoint].fCoords[0];
            if (! timeDelta == 0)
            {
                double slope = frequencyDelta / timeDelta;
                double intercept = setOfPoints[iPoint].fCoords[1] - slope * setOfPoints[iPoint].fCoords[0];

                bool close_enough = true;
                double* test_pt;
                std::vector< std::pair< size_t, double > > indicesDist;
                double k = 2.0;
                //const nanoflann::SearchParams& searchParams;
                while (close_enough)
                {
                    test_pt[0] = setOfPoints[iPoint].fCoords[0] + k * timeDelta;
                    test_pt[1] = setOfPoints[iPoint].fCoords[1] + k * frequencyDelta;
                    kdTree->RadiusSearch(test_pt, fMembershipRadius, indicesDist, nanoflann::SearchParams(32, 0, true));
                    if (indicesDist.size() > 0)
                    {
                        k += 1.0;
                        votes[iPoint][indicesDist[0].first] += 1;
                    } else {
                        close_enough = false;
                    }
                }
                close_enough = true;
                k = -2.0;
                while (close_enough)
                {
                    test_pt[0] = setOfPoints[iPoint].fCoords[0] + k * timeDelta;
                    test_pt[1] = setOfPoints[iPoint].fCoords[1] + k * frequencyDelta;
                    kdTree->RadiusSearch(test_pt, fMembershipRadius, indicesDist, nanoflann::SearchParams(32, 0, true));
                    if (indicesDist.size() > 0)
                    {
                        k -= 1.0;
                        votes[iPoint][indicesDist[0].first] += 1;
                    } else {
                        close_enough = false;
                    }
                }
            }
        }
        return true;
    }

/*
    void KTConsensusThresholding::VoteCore(bool doPositive, size_t thisPID, size_t neighborPID, const std::vector< KTKDTreeData::Point >& setOfPoints, double slope, double intercept)
    {
        bool closeEnough = true;
        double k = 2.0;
        double deltak = 1.0;
        if (! doPositive) {
            k = -2.0;
        }

        / *double test_pt[2];
        int looplim = 100000000;//unmotived "large" number of tries
        int loopcount = 0;
        while (closeEnough && (loopcount < looplim)) {
            loopcount += 1;
            test_pt[0] = setOfPoitns[thisPID].fCoords[0] + k * (setOfPoints[neighborPID].fCoords[0] - setOfPoints[thisPID].fCoords[0]);
            test_pt[1] = setOfPoitns[thisPID].fCoords[1] + k * (setOfPoints[neighborPID].fCoords[1] - setOfPoints[thisPID].fCoords[1]);
            
            if (1.0 < fMembershipRadius) 
            {
                k += 1.0;
            } else {
                closeEnough = false;
            }
        }
        * /
    }
*/

} /* namespace Katydid */
