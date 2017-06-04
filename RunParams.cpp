//
// Created by damian on 6/3/17.
//

#include "RunParams.h"


pixy_roimux::RunParams::RunParams(const unsigned t_runId) : m_runId(t_runId) {
    // Run 1 parameters.
    if (m_runId == 1) {
        m_nPixels = 36;
        m_nRois = 28;
        m_nChans = m_nPixels + m_nRois;
        m_pixelPitch = 2.86;
        m_driftLength = 600.;
        m_sampleTime = .41;
        m_driftSpeed = 2.1;
        m_anodeSample = 93;
        m_adcLsb = .1512;
        m_preampGain = 25.;

        // Generate DAQ readout channel mapping for run 1.
        // Be careful, not to confuse the histograms because the DAQ histogram containing all the ROI (induction) channels
        // is called "Col_x" and the DAQ histogram containing most of the pixel (collection) channels is called "Ind_x".
        m_daq2readout.resize(m_nChans);
        m_readout2daq.resize(m_nChans);
        for (unsigned i = 0; i < 16; ++i) {
            m_daq2readout.at(2 * i) = i;
            m_daq2readout.at(2 * i + 1) = i + 16;
            m_daq2readout.at(2 * i + 32) = i + 48;
            m_daq2readout.at(2 * i + 33) = i + 32;
            m_readout2daq.at(i) = 2 * i;
            m_readout2daq.at(i + 16) = 2 * i + 1;
            m_readout2daq.at(i + 32) = 2 * i + 33;
            m_readout2daq.at(i + 48) = 2 * i + 32;
        }

        // Generate pixel and ROI coordinates for run 1.
        // All coordinates are in units of pixel pitch. The pixel coordinates are relative to the upper left pixel (0, 0).
        // To get the absolute pixel coordinates, the pixel coordinates need to be added to the ROI coordinates which in
        // in turn are offsets of the upper left pixel (0, 0). See the readout PCB design for details.
        m_pixelCoor.resize(m_nPixels);
        for (unsigned i = 0; i < m_nPixels; ++i) {
            m_pixelCoor.at(i).resize(2);
            m_pixelCoor.at(i).at(0) = i % 6;
            m_pixelCoor.at(i).at(1) = - static_cast<int>(i) / 6;
        }
        m_roiCoor = {{{1, 27},    {1, 21},    {1, 15},
                            {7, 30},    {7, 24},    {7, 18},    {7, 12},    {10, 6},
                            {10, 36},   {13, 30},   {13, 24},   {13, 18},   {13, 12},   {16, 6},
                            {16, 36},   {19, 30},   {19, 24},   {19, 18},   {19, 12},   {22, 6},
                            {22, 36},   {25, 30},   {25, 24},   {25, 18},   {25, 12},
                            {31, 27},   {31, 21},   {31, 15}}};
    }
        // Run 2 parameters.
    else if (m_runId == 2) {
        m_nPixels = 36;
        m_nRois = 28;
        m_nChans = m_nPixels + m_nRois;
        m_pixelPitch = 2.86;
        m_driftLength = 600.;
        m_sampleTime = .21;
        m_driftSpeed = 2.1;
        m_anodeSample = 47;
        m_adcLsb = .1512;
        m_preampGain = 25.;

        // Generate DAQ readout channel mapping for run 1.
        // Be careful, not to confuse the histograms because the DAQ histogram containing all the ROI (induction) channels
        // is called "Col_x" and the DAQ histogram containing most of the pixel (collection) channels is called "Ind_x".
        m_daq2readout.resize(m_nChans);
        m_readout2daq.resize(m_nChans);
        for (unsigned i = 0; i < m_nChans; ++i) {
            m_daq2readout.at(i) = i;
            m_readout2daq.at(i) = i;
        }

        // Generate pixel and ROI coordinates for run 1.
        // All coordinates are in units of pixel pitch. The pixel coordinates are relative to the upper left pixel (0, 0).
        // To get the absolute pixel coordinates, the pixel coordinates need to be added to the ROI coordinates which in
        // in turn are offsets of the upper left pixel (0, 0). See the readout PCB design for details.
        m_pixelCoor.resize(m_nPixels);
        for (unsigned i = 0; i < m_nPixels; ++i) {
            m_pixelCoor.at(i).resize(2);
            m_pixelCoor.at(i).at(0) = i % 6;
            m_pixelCoor.at(i).at(1) = - static_cast<int>(i) / 6;
        }
        m_roiCoor = {{{1, 27},    {1, 21},    {1, 15},
                            {7, 30},    {7, 24},    {7, 18},    {7, 12},    {10, 6},
                            {10, 36},   {13, 30},   {13, 24},   {13, 18},   {13, 12},   {16, 6},
                            {16, 36},   {19, 30},   {19, 24},   {19, 18},   {19, 12},   {22, 6},
                            {22, 36},   {25, 30},   {25, 24},   {25, 18},   {25, 12},
                            {31, 27},   {31, 21},   {31, 15}}};
    }
        // Die if we received an invalid run ID.
    else {
        std::cerr << "ERROR: Unknown run ID: " << m_runId << '!' << std::endl;
        exit(1);
    }
}