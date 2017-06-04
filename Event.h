//
// Created by damian on 6/3/17.
//

#ifndef PIXY_ROIMUX_EVENT_H
#define PIXY_ROIMUX_EVENT_H


#include <map>
#include <vector>


namespace pixy_roimux {
    struct Hit2d;
    struct Hit3d;


///
/// Event struct.
/// The high(low) multimaps map the start(end) of the hit pulse to the hit in pixelHits and roiHits respectively. The
/// pixel2roi(roi2pixel) vectors map each pixel(roi) hit to all its matched roi(pixel) hits, i.e. pixel2roi.at(x) will
/// return a vector containing the indices of all the hits in roiHits matched to the x-th pixelHit. If an ambiguous
/// match occured, the vector will contain more than one index and naturally if matching failed, the vector will be
/// empty.
///
    struct Event {
        ///
        /// Run ID.
        ///
        unsigned runId;

        ///
        /// Subrun ID.
        ///
        unsigned subrunId;

        ///
        /// Event ID.
        ///
        unsigned eventId;

        ///
        /// Pixel hits stored at viper2dHit structs.
        ///
        std::vector<Hit2d> pixelHits;

        ///
        /// Map from viper2dHit.firstSample to its index in pixelHits.
        ///
        std::multimap<unsigned, unsigned> pixelHitOrderLow;

        ///
        /// Map from viper2dHit.lastSample to its index in pixelHits.
        ///
        std::multimap<unsigned, unsigned> pixelHitOrderHigh;

        ///
        /// ROI hits stored at viper2dHit structs.
        ///
        std::vector<Hit2d> roiHits;

        ///
        /// Map from viper2dHit.firstSample to its index in roiHits.
        ///
        std::multimap<unsigned, unsigned> roiHitOrderLow;

        ///
        /// Map from viper2dHit.lastSample to its index in roiHits.
        ///
        std::multimap<unsigned, unsigned> roiHitOrderHigh;

        ///
        /// Vector of indices of all matched roiHits for each pixelHit entry.
        ///
        std::vector<std::vector<unsigned>> pixel2roi;

        ///
        /// Vector of indices of all matched pixelHits for each roiHit entry.
        ///
        std::vector<std::vector<unsigned>> roi2pixel;

        ///
        /// viper3dHit candidates generated from pixel2roi and pixelHits.
        /// Has the same dimensions as pixel2roi.
        ///
        std::vector<std::vector<Hit3d>> hitCandidates;
    };


///
/// 2D hit struct.
/// Used to store pixel and ROI hits.
///
    struct Hit2d {
        ///
        /// Pixel channel.
        ///
        unsigned channel;

        ///
        /// Index in the raw histogram of the first sample of the pulse.
        ///
        unsigned firstSample;

        ///
        /// Index in the raw histogram of the peak sample of the pulse.
        ///
        unsigned peakSample;

        ///
        /// Index in the raw histogram of the last sample of the pulse.
        ///
        unsigned lastSample;

        ///
        /// Pulse width in number of samples (usually lastSample - firstSample + 1).
        ///
        unsigned pulseWidth;

        ///
        /// ADC value at the peak of the pulse.
        ///
        int pulseHeight;

        ///
        /// Integral of the pulse from firstSample until and including lastSample.
        /// In ADC units times number of samples.
        ///
        int pulseIntegral;

        ///
        /// Raw pulse data extracted from histogram from firstSample until and including lastSample.
        /// Size is equal to pulseWidth.
        ///
        std::vector<int> pulseRaw;
    };


///
/// 3D hit struct.
/// Used to store 3D hit (candidates).
///
    struct Hit3d {
        ///
        /// X coordinate in mm.
        ///
        float x;

        ///
        /// Y coordinate in mm.
        ///
        float y;

        ///
        /// Z coordinate in mm.
        ///
        float z;

        ///
        /// Charge in C.
        ///
        float charge;
    };
}



#endif //PIXY_ROIMUX_EVENT_H
