/*
 * KTHDF5Writer.cc
 *
 *  Created on: Jan 23, 2013
 *      Author: nsoblath
 */

#include "KTHDF5Writer.hh"

#include "KTCommandLineOption.hh"
#include "KTParam.hh"

#include "TFile.h"
#include "TTree.h"

using std::set;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "KTHDF5Writer");


    KT_REGISTER_WRITER(KTHDF5Writer, "hdf5-writer");
    KT_REGISTER_PROCESSOR(KTHDF5Writer, "hdf5-writer");

    static KTCommandLineOption< string > sRTWFilenameCLO("HDF5 Writer", "HDF5 writer filename", "hdf5-file");

    KTHDF5Writer::KTHDF5Writer(const std::string& name) :
            KTWriterWithTypists< KTHDF5Writer >(name),
            fFilename("my_file.h5"),
            fFile(NULL),
            slice_size(0),
            raw_slice_size(0),
            raw_time_slice_dspace(NULL),
            time_slice_dspace(NULL)
    {
        RegisterSlot("close-file", this, &KTHDF5Writer::CloseFile);
    }

    KTHDF5Writer::~KTHDF5Writer()
    {
            CloseFile();
    }

    bool KTHDF5Writer::Configure(const KTParamNode* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetFilename(node->GetValue("output-file", fFilename));
        }

        // Command-line settings
        SetFilename(fCLHandler->GetCommandLineValue< string >("hdf5-file", fFilename));

        return true;
    }

    bool KTHDF5Writer::OpenAndVerifyFile()
    {
        if (fFile == NULL) {
            KTINFO(publog, "Opening HDF5 file");
            fFile = new H5::H5File(fFilename.c_str(), H5F_ACC_TRUNC);
        }
        if (!fFile) {
            delete fFile;
            fFile = NULL;
            KTERROR(publog, "Error opening HDF5 file!!");
            return false;
        }
        return true;
    }

    H5::H5File* KTHDF5Writer::OpenFile(const std::string& filename, const std::string& flag)
    {
        CloseFile();
        this->fFile = new H5::H5File(filename.c_str(), H5F_ACC_TRUNC);
        KTINFO(publog, "opened HDF5 file!");
        return this->fFile;
    }

    void KTHDF5Writer::CloseFile()
    {
        if (fFile != NULL) {
            KTINFO(publog, 
                    "HDF5 data written to file <" 
                    << this->fFilename 
                    << ">; closing file");
            delete fFile;
            fFile = NULL;
        }
        return;
    }

    H5::Group* KTHDF5Writer::AddGroup(const std::string& groupname) {
        H5::Group* result = new H5::Group(fFile->createGroup(groupname));
        return result;
    }

    void KTHDF5Writer::SetComponents(const unsigned n_channels) {
        this->n_channels = n_channels;
    }

    unsigned KTHDF5Writer::GetSliceSize() {
        return this->slice_size;
    }

    void KTHDF5Writer::SetSliceSize(const unsigned slice_size) {
        this->slice_size = slice_size;
    }

    unsigned KTHDF5Writer::GetRawSliceSize() {
        return this->raw_slice_size;
    }

    void KTHDF5Writer::SetRawSliceSize(const unsigned slice_size) {
        this->raw_slice_size = slice_size;
    }

    void KTHDF5Writer::CreateDataspaces() {
        /*
        If the dataspaces have already been created, this is a no-op.  
        Otherwise, we want to create two dataspaces - 1XM and 1XN, where
        M is the size of a raw time slice, and N is the size of a time slice.
        */
        if(this->raw_time_slice_dspace == NULL 
            || this->time_slice_dspace == NULL) {
            hsize_t raw_dims[] = {this->n_channels, this->raw_slice_size};
            hsize_t dims[] = {this->n_channels, this->slice_size};

            this->raw_time_slice_dspace = new H5::DataSpace(2, raw_dims);
            this->time_slice_dspace = new H5::DataSpace(2, dims);
        }
    }

    H5::DataSpace* KTHDF5Writer::GetRawTimeDataspace() {
        return this->raw_time_slice_dspace;
    }

    H5::DataSpace* KTHDF5Writer::GetTimeDataspace() {
        return this->time_slice_dspace;
    }

    H5::DataSet* KTHDF5Writer::CreateRawTimeSeriesDataSet(const std::string& name) {
        H5::DSetCreatPropList plist;
        plist.setFillValue(H5::PredType::NATIVE_INT, 0);
        H5::DataSet* dset = new H5::DataSet(fFile->createDataSet(name.c_str(),
                                                                 H5::PredType::NATIVE_INT,
                                                                 *(this->raw_time_slice_dspace),
                                                                 plist));
        return dset;
    }

} /* namespace Katydid */
