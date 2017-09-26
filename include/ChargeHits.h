//
// Created by damian on 6/3/17.
//

#ifndef PIXY_ROIMUX_CHARGEHITS_H
#define PIXY_ROIMUX_CHARGEHITS_H


#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "TH1D.h"
#include "TH2S.h"
#include "TSpectrum.h"
#include "ChargeData.h"
#include "Event.h"
#include "NoiseFilter.h"
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
                const RunParams &t_runParams) :
                m_chargeData(t_chargeData),
                m_runParams(t_runParams) {
        }

        ///
        /// Find hits.
        ///
        void findHits(const bool t_bipolarRoiHits = true);

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
                std::vector<Hit2d> &t_hits,
                std::multimap<unsigned, unsigned> &t_hitOrderLead,
                std::multimap<unsigned, unsigned> &t_hitOrderTrail,
                unsigned &t_nMissed,
                const bool t_bipolar,
                const double t_discSigmaPosLead,
                const double t_discSigmaPosPeak,
                const double t_discAbsPosPeak,
                const double t_discSigmaPosTrail,
                const double t_discSigmaNegLead,
                const double t_discSigmaNegPeak,
                const double t_discAbsNegPeak,
                const double t_discSigmaNegTrail
        );

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
        /// Vector to help draw ambiguity histogram
        ///
	std::vector<unsigned> ambiguities;

	///
        /// Vector to help draw unmatched histogram
        ///
	std::vector<unsigned> unmatched;

	///
        /// Vector to help draw time acceptance histogram
        ///
	std::vector<unsigned> timeAcceptance;	

	///
        /// Vector to help draw posPulseHeight for pixel histogram
        ///
	std::vector<unsigned> posPulsePixelMax;

	///
        /// Vector to help draw posPulseHeight for ROI histogram
        ///
	std::vector<unsigned> posPulseROIMax;

	///
        /// Vector to help draw thrPosPeak for Pixel histogram
        ///
	std::vector<double> thrPosPeakVec;

    };
}


#endif //PIXY_ROIMUX_CHARGEHITS_H
