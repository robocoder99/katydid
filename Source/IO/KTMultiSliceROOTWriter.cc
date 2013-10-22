/*
 * KTMultiSliceROOTWriter.cc
 *
 *  Created on: Jan 28, 2013
 *      Author: nsoblath
 */

#include "KTMultiSliceROOTWriter.hh"

#include "KTNOFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"

using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");


    static KTDerivedNORegistrar< KTWriter, KTMultiSliceROOTWriter > sMSRWriterRegistrar("multislice-root-writer");
    static KTDerivedNORegistrar< KTProcessor, KTMultiSliceROOTWriter > sMSRWProcRegistrar("multislice-root-writer");

    KTMultiSliceROOTWriter::KTMultiSliceROOTWriter(const std::string& name) :
            KTWriterWithTypists< KTMultiSliceROOTWriter >(name),
            fUseTFile(true),
            fTFilename("multi_slice.root"),
            fTFileFlag("recreate"),
            fUseGraphics(false),
            fGraphicsFilePath(),
            fGraphicsFilenameBase("slice"),
            fGraphicsFileType("png"),
            fFile(NULL)
    {
        RegisterSlot("start", this, &KTMultiSliceROOTWriter::Start);
        RegisterSlot("finish", this, &KTMultiSliceROOTWriter::Finish);
    }

    KTMultiSliceROOTWriter::~KTMultiSliceROOTWriter()
    {
        if (fFile != NULL)
        {
            fFile->Close();
        }
        delete fFile;
    }

    Bool_t KTMultiSliceROOTWriter::Configure(const KTPStoreNode* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetUseTFile(node->GetData<Bool_t>("use-tfile", fUseTFile));
            SetTFilename(node->GetData<string>("output-tfile", fTFilename));
            SetTFileFlag(node->GetData<string>("tfile-flag", fTFileFlag));

            SetUseGraphics(node->GetData<Bool_t>("use-graphics", fUseGraphics));
            SetGraphicsFilePath(node->GetData<string>("graphics-file-path", fGraphicsFilePath));
            SetGraphicsFilenameBase(node->GetData<string>("graphics-filename-base", fGraphicsFilenameBase));
            SetGraphicsFileType(node->GetData<string>("graphics-file-type", fGraphicsFileType));
        }

        return true;
    }

    Bool_t KTMultiSliceROOTWriter::OpenAndVerifyFile()
    {
        if (fUseTFile)
        {
            if (fFile == NULL)
            {
                fFile = new TFile(fTFilename.c_str(), fTFileFlag.c_str());
            }
            if (! fFile->IsOpen())
            {
                delete fFile;
                fFile = NULL;
                KTERROR(publog, "Output file <" << fTFilename << "> did not open!");
                return false;
            }
            fFile->cd();
        }
        return true;
    }

    void KTMultiSliceROOTWriter::Start()
    {
        for (TypeWriterMap::iterator it = fTypeWriters.begin(); it != fTypeWriters.end(); it++)
        {
            static_cast< KTMEROOTTypeWriterBase* >(it->second)->StartNewHistograms();
        }
        return;
    }

    void KTMultiSliceROOTWriter::Finish()
    {
        for (TypeWriterMap::iterator it = fTypeWriters.begin(); it != fTypeWriters.end(); it++)
        {
            static_cast< KTMEROOTTypeWriterBase* >(it->second)->FinishHistograms();
        }
        return;
    }


} /* namespace Katydid */