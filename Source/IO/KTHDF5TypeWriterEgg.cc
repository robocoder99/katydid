/*
 * KTHDF5TypeWriterEgg.cc
 *
 *  Created on: Sept 9, 2014
 *      Author: nsoblath
 */

#include "KTHDF5TypeWriterEgg.hh"

#include "KTEggHeader.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTRawTimeSeries.hh"
#include "KTRawTimeSeriesData.hh"
#include "KTSliceHeader.hh"
#include "KTTimeSeries.hh"
#include "KTTimeSeriesData.hh"

#include <cstring>
#include <sstream>



using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "KTHDF5TypeWriterEgg");

    static KTTIRegistrar< KTHDF5TypeWriter, KTHDF5TypeWriterEgg > sHDF5TWEggRegistrar;

    KTHDF5TypeWriterEgg::KTHDF5TypeWriterEgg() :
            KTHDF5TypeWriter()
    {
    }

    KTHDF5TypeWriterEgg::~KTHDF5TypeWriterEgg()
    {
    }


    void KTHDF5TypeWriterEgg::RegisterSlots()
    {
        fWriter->RegisterSlot("write_header", this, &KTHDF5TypeWriterEgg::WriteEggHeader);
        fWriter->RegisterSlot("setup_from_header", this, &KTHDF5TypeWriterEgg::ProcessEggHeader);
        fWriter->RegisterSlot("raw_ts", this, &KTHDF5TypeWriterEgg::WriteRawTimeSeriesData);
        return;
    }


    // *********************
    // Egg Header
    // *********************

    void KTHDF5TypeWriterEgg::ProcessEggHeader(KTEggHeader* header)
    {   
        // TODO(kofron): this should probably belong to the writer.
        // TODO(kofron): storage size should be set here too, not just 8 bit.
        /*
            Inform the writer about the slice size so it can set up
            its buffers and so on.
        */
        if (!fWriter->OpenAndVerifyFile()) return;
        if(header != NULL) {
            fWriter->SetComponents(header->GetNChannels());
            fWriter->SetRawSliceSize(header->GetRawSliceSize());
            fWriter->SetSliceSize(header->GetSliceSize());  

            fWriter->CreateDataspaces();
            fWriter->AddGroup("/raw_data");
        }
    }

    void KTHDF5TypeWriterEgg::WriteEggHeader(KTEggHeader* header) {
        // TODO(kofron): clearly this is insufficient
        // TODO(kofron): H5T_VARIABLE length for strings?
        if (!fWriter->OpenAndVerifyFile()) return;

        H5::Group* header_grp = fWriter->AddGroup("/metadata");
       
        // Write the timestamp
        std::string string_attr = header->GetTimestamp();
        H5::StrType ts_type(H5::PredType::C_S1, string_attr.length() + 1);
        H5::DataSpace attr_space(H5S_SCALAR);
        H5::Attribute attr = header_grp->createAttribute("timestamp",
                                                        ts_type,
                                                        attr_space);
        attr.write(ts_type, string_attr.c_str());

        // Write the description
        string_attr = header->GetDescription();
        H5::StrType desc_type(H5::PredType::C_S1, string_attr.length() + 1);
        attr = header_grp->createAttribute("description",
                                           desc_type,
                                           attr_space);
        attr.write(desc_type, string_attr.c_str());

        // Write AcqusitionMode
        attr = header_grp->createAttribute("acquisition_mode",
                                           H5::PredType::NATIVE_UINT,
                                           attr_space);
        unsigned value = header->GetAcquisitionMode();
        attr.write(H5::PredType::NATIVE_UINT, &value);

        // NChannels
        attr = header_grp->createAttribute("n_channels",
                                           H5::PredType::NATIVE_UINT,
                                           attr_space);
        value = header->GetNChannels();
        attr.write(H5::PredType::NATIVE_UINT, &value);

        // Raw Slice Size, Slice Size, Slice Stride, and RecordSize
        attr = header_grp->createAttribute("raw_slice_size",
                                            H5::PredType::NATIVE_UINT,
                                            attr_space);
        value = header->GetRawSliceSize();
        attr.write(H5::PredType::NATIVE_UINT, &value);

        attr = header_grp->createAttribute("slice_size",
                                            H5::PredType::NATIVE_UINT,
                                            attr_space);
        value = header->GetSliceSize();
        attr.write(H5::PredType::NATIVE_UINT, &value);

        attr = header_grp->createAttribute("slice_stride",
                                            H5::PredType::NATIVE_UINT,
                                            attr_space);
        value = header->GetSliceStride();
        attr.write(H5::PredType::NATIVE_UINT, &value);

        attr = header_grp->createAttribute("record_size",
                                            H5::PredType::NATIVE_UINT,
                                            attr_space);
        value = header->GetRecordSize();
        attr.write(H5::PredType::NATIVE_UINT, &value);

        // Run Duration and Acquisition Rate
        attr = header_grp->createAttribute("run_duration",
                                            H5::PredType::NATIVE_UINT,
                                            attr_space);
        value = header->GetRunDuration();
        attr.write(H5::PredType::NATIVE_UINT, &value);

        attr = header_grp->createAttribute("acquisition_rate",
                                            H5::PredType::NATIVE_DOUBLE,
                                            attr_space);
        double fp_value = header->GetAcquisitionRate();
        attr.write(H5::PredType::NATIVE_DOUBLE, &fp_value);


        // RunType, RunSourceType, and FormatModeType
        attr = header_grp->createAttribute("run_type",
                                            H5::PredType::NATIVE_UINT,
                                            attr_space);
        value = header->GetRunType();
        attr.write(H5::PredType::NATIVE_UINT, &value);

/*        attr = header_grp->createAttribute("run_source_type",
                                            H5::PredType::NATIVE_UINT,
                                            attr_space);
        value = header->GetRunSourceType();
        attr.write(H5::PredType::NATIVE_UINT, &value);*/

        attr = header_grp->createAttribute("format_mode",
                                            H5::PredType::NATIVE_UINT,
                                            attr_space);
        value = header->GetFormatMode();
        attr.write(H5::PredType::NATIVE_UINT, &value);

        // ADC parameters
        attr = header_grp->createAttribute("data_type_size",
                                            H5::PredType::NATIVE_UINT,
                                            attr_space);
        value = header->GetDataTypeSize();
        attr.write(H5::PredType::NATIVE_UINT, &value);

        attr = header_grp->createAttribute("bit_depth",
                                            H5::PredType::NATIVE_UINT,
                                            attr_space);
        value = header->GetBitDepth();
        attr.write(H5::PredType::NATIVE_UINT, &value);

        attr = header_grp->createAttribute("voltage_min",
                                            H5::PredType::NATIVE_DOUBLE,
                                            attr_space);
        fp_value = header->GetVoltageMin();
        attr.write(H5::PredType::NATIVE_DOUBLE, &fp_value);

        attr = header_grp->createAttribute("voltage_range",
                                            H5::PredType::NATIVE_DOUBLE,
                                            attr_space);
        fp_value = header->GetVoltageRange();
        attr.write(H5::PredType::NATIVE_DOUBLE, &fp_value);

        return;
    }

    //*****************
    // Raw Time Series Data
    //*****************

    void KTHDF5TypeWriterEgg::WriteRawTimeSeriesData(KTDataPtr data)
    {
        if (! data) return;

        // A buffer to work in.
        unsigned buffer[fWriter->GetRawSliceSize()];

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
        std::stringstream ss;
        ss << "/raw_data/" << "slice_" << sliceNumber;
        std::string slice_name;
        ss >> slice_name;        
        H5::DataSet* dset = fWriter->CreateRawTimeSeriesDataSet(slice_name);

        KTRawTimeSeriesData& tsData = data->Of<KTRawTimeSeriesData>();
        unsigned nComponents = tsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
           
            const KTRawTimeSeries* spectrum = tsData.GetTimeSeries(iComponent);
            if (spectrum != NULL)
            {
                std::cout << spectrum[0](0) << std::endl;
                for(int i=0; i < fWriter->GetRawSliceSize(); i++) {
                    // TODO(kofron): wat
                    buffer[i] = spectrum[0](i);
                }
                dset->write(buffer, H5::PredType::NATIVE_UINT);
            }
        }
        return;
    }

    //*****************
    // Time Series Data
    //*****************

    void KTHDF5TypeWriterEgg::WriteTimeSeriesData(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTTimeSeriesData& tsData = data->Of<KTTimeSeriesData>();
        unsigned nComponents = tsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTTimeSeries* spectrum = tsData.GetTimeSeries(iComponent);
            if (spectrum != NULL)
            {
                // write time series here
            }
        }
        return;
    }

} /* namespace Katydid */
