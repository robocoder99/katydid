/*
 * KTMultiSliceROOTWriter.cc
 *
 *  Created on: Jan 28, 2013
 *      Author: nsoblath
 */

#include "KTMultiSliceROOTWriter.hh"

using std::string;

namespace Katydid
{
    KTLOGGER(publog, "KTMultiSliceROOTWriter");


    KT_REGISTER_WRITER(KTMultiSliceROOTWriter, "multislice-root-writer");
    KT_REGISTER_PROCESSOR(KTMultiSliceROOTWriter, "multislice-root-writer");

    KTMultiSliceROOTWriter::KTMultiSliceROOTWriter(const std::string& name) :
            KTWriterWithTypists< KTMultiSliceROOTWriter, KTMultiSliceROOTTypeWriter >(name),
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

    bool KTMultiSliceROOTWriter::Configure(const scarab::param_node* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetUseTFile(node->get_value<bool>("use-tfile", fUseTFile));
            SetTFilename(node->get_value("output-tfile", fTFilename));
            SetTFileFlag(node->get_value("tfile-flag", fTFileFlag));

            SetUseGraphics(node->get_value<bool>("use-graphics", fUseGraphics));
            SetGraphicsFilePath(node->get_value("graphics-file-path", fGraphicsFilePath));
            SetGraphicsFilenameBase(node->get_value("graphics-filename-base", fGraphicsFilenameBase));
            SetGraphicsFileType(node->get_value("graphics-file-type", fGraphicsFileType));
        }

        return true;
    }

    bool KTMultiSliceROOTWriter::OpenAndVerifyFile()
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
