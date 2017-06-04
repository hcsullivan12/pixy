//
// Created by damian on 6/3/17.
//

#ifndef PIXY_ROIMUX_CHARGEHITS_H
#define PIXY_ROIMUX_CHARGEHITS_H


#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "TH1D.h"
#include "TH2S.h"
#include "TSpectrum.h"
#include "ChargeData.h"
#include "Event.h"
#include "RunParams.h"


namespace pixy_roimux {
///
/// This class implements a 3D hit finder using ROOT's TSpectrum class for peak finding and a constant fraction
/// discrimination for timing. As input it needs a viperData object containing the raw data and a viperMap object for
/// the pixel and ROI coordinates. The parameters for the peak finder of the TSpectrum instance can be set by the user.
/// The timing works by finding the first and the last sample of the pixel and ROI pulses by means of a constant
/// fraction of the peak value of the pulse. The fraction can be set independently for pixel and ROIs and start and end
/// of the pulse, respectively. After this, the found pixel pulses are searched for overlaps with ROI pulses and all
/// matches are written to both a vector that contains the indices of all matched ROI pulses for a pixel pulse and vice
/// versa. Besides, the raw pulse, its integral and 3D hit candidates are calculated. All the data is stored in a vector
/// containing one viperEvent struct per event. They can be accessed by const reference or by reference.
///
    class ChargeHits {
    public:

        ///
        /// Constructor reading raw data from a viperData object.
        /// Needs a viperMap object for the pixel and ROI coordinates.
        ///
        ChargeHits(
                const ChargeData &t_chargeData,
                const RunParams &t_runParams);

        ///
        /// Set the parameters for the peak finder of ROOT's TSpectrum class.
        /// See the TSpectrum documentation for details.
        ///
        void setSpecParams(
                const double t_sigma,
                const std::string t_option,
                const double t_threshold) {
            m_specSig = t_sigma;
            m_specOpt = t_option;
            m_specThr = t_threshold;
        }

        ///
        /// Set the parameters for the constant fraction discrimination.
        /// discFracPixel and discFracRoi are the discrimination fractions for pixels and ROIs, respectively. Low and high
        /// thereof are corresponging to the rising and falling pulse edges, respectively. discThr is an absolut threshold
        /// applied on both pixels and ROIs to discard events with peaks that are too low. discRange is a range in number of
        /// samples before and after the peak within which the rising and falling edges are detected. If one of the edges
        /// falls outside this range, the pulse is discarded.
        ///
        void setDiscParams(
                const double t_discFracPixelLow,
                const double t_discFracPixelHigh,
                const double t_discFracRoiLow,
                const double t_discFracRoiHigh,
                const int t_discThr,
                const unsigned t_discRange) {
            m_discFracPixelLow = t_discFracPixelLow;
            m_discFracPixelHigh = t_discFracPixelHigh;
            m_discFracRoiLow = t_discFracRoiLow;
            m_discFracRoiHigh = t_discFracRoiHigh;
            m_discThr = t_discThr;
            m_discRange = t_discRange;
        }

        ///
        /// Find hits.
        ///
        void findHits();

        ///
        /// Get the vector containing all the hits of all events.
        ///
        std::vector<Event> &getEvents() {
            return m_events;
        }

        ///
        /// Get the vector containint all the hist of all events as const reference.
        ///
        const std::vector<Event> &getEvents() const {
            return m_events;
        }


    private:

        ///
        /// Private method used internally to find all the 2D hits in a histrogram using ROOT's TSpectrum class.
        /// discFracLow and discFracHigh are the fractions used for the constant fraction discrimination. The hits are
        /// stored in a vector of viper2dHits passed by reference. Besides hitOrderLow(High) map the time of the first(last)
        /// pulse sample to the index of the hit. This allows to simply loop through the ROI hits and the pixel hits using
        /// these maps to match them.
        ///
        void find2dHits(
                const TH2S &t_histo,
                const double t_discFracLow,
                const double t_discFracHigh,
                std::vector<Hit2d> &t_hits,
                std::multimap<unsigned, unsigned> &t_hitOrderLow,
                std::multimap<unsigned, unsigned> &t_hitOrderHigh);

        ///
        /// Private method used internally to find the 3D hits using the 2D hits found in both readout histos by the 2D hit
        /// finder. The method reads the 2D hits from the viperEvent passed by reference and writes back two vectors
        /// containing all the matched ROI(pixel) hits for each pixel(ROI) hit. A match occurs if a pixel and a ROI pulse
        /// overlap. This method only performs the matching. BuildHitCandidates is called afterwards to build the 3D hit
        /// candidates containing 3D coordinates and reconstructed charge.
        ///
        void find3dHits(Event &t_event);

        ///
        /// Find hit candidates.
        /// This method builds viper3dHits using the matches found by Find3dHits. It reads the vectors mapping pixels to
        /// ROIs from the viperEvent passed by reference, builds viper3dEvents and writes them back to the event struct.
        ///
        void buildHitCandidates(Event &t_event);

        ///
        /// Vector containing all events.
        ///
        std::vector<Event> m_events;

        ///
        /// viperData object containing the raw data.
        ///
        const ChargeData &m_chargeData;

        ///
        /// viperMap used to convert pixel and ROI channels to actual geometrical coordinates.
        ///
        const RunParams &m_runParams;

        ///
        /// Instance of ROOT's TSpectrum classe used to find the peaks in the raw data.
        /// For some reason, this doesn't work without a pointer.
        ///
        std::unique_ptr<TSpectrum> m_spectrum;

        ///
        /// Sigma parameter of the TSpectrum.
        /// Consult the TSpectrum documentation for details.
        ///
        double m_specSig = 10.;

        ///
        /// Threshold parameter of the TSpectrum.
        /// Consult the TSpectrum documentation for details.
        ///
        double m_specThr = .4;

        ///
        /// Option parameter of the TSpectrum.
        /// Consult the TSpectrum documentation for details.
        ///
        std::string m_specOpt = "nodraw";

        ///
        /// Constant fraction for the discrimination of the rising edge of pixel hits.
        ///
        double m_discFracPixelLow = .25;

        ///
        /// Constant fraction for the discrimination of the falling edge of pixel hits.
        ///
        double m_discFracPixelHigh = .25;

        ///
        /// Constant fraction for the discrimination of the rising edge of ROI hits.
        ///
        double m_discFracRoiLow = .25;

        ///
        /// Constant fraction for the discrimination of the falling edge of ROI hits.
        ///
        double m_discFracRoiHigh = .25;

        ///
        /// Absolut threshold used to discard bad peaks.
        /// This is needed because TSpectrum finds very low peaks in histos containing no hits.
        ///
        int m_discThr = 100;

        ///
        /// Range within which the discriminator looks for the crossing of the constant fraction threshold.
        ///
        unsigned m_discRange = 100;
    };
}


#endif //PIXY_ROIMUX_CHARGEHITS_H
