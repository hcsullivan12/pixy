//
// Created by damian on 6/3/17.
//

#ifndef PIXY_ROIMUX_EVENT_H
#define PIXY_ROIMUX_EVENT_H


#include <map>
#include <vector>


namespace pixy_roimux {
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
        /// Index in the raw histogram of the positive peak sample of the pulse.
        ///
        unsigned posPeakSample;

        ///
        /// Index in the raw histogram of the first negative sample of the pulse.
        ///
        unsigned zeroCrossSample;

        ///
        /// Index in the raw histogram of the negative peak sample of the pulse.
        ///
        unsigned negPeakSample;

        ///
        /// Index in the raw histogram of the last sample of the pulse.
        ///
        unsigned lastSample;

        ///
        /// Positive ulse width in number of samples.
        ///
        unsigned posPulseWidth;

        ///
        /// Negative ulse width in number of samples.
        ///
        unsigned negPulseWidth;

        ///
        /// ADC value at the positive peak of the pulse.
        ///
        int posPulseHeight;

        ///
        /// ADC value at the negative peak of the pulse.
        ///
        int negPulseHeight;

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

        unsigned pixelHitId;

        unsigned roiHitId;
    };


    struct PrincipalComponents {
        unsigned numHitsUsed;
        std::vector<double> eigenValues;
        std::vector<std::vector<double>> eigenVectors;
        std::vector<double> avePosition;
        double aveHitDoca;
    };


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
        std::multimap<unsigned, unsigned> pixelHitOrderLead;

        ///
        /// Map from viper2dHit.lastSample to its index in pixelHits.
        ///
        std::multimap<unsigned, unsigned> pixelHitOrderTrail;

        ///
        /// ROI hits stored at viper2dHit structs.
        ///
        std::vector<Hit2d> roiHits;

        ///
        /// Map from viper2dHit.firstSample to its index in roiHits.
        ///
        std::multimap<unsigned, unsigned> roiHitOrderLead;

        ///
        /// Map from viper2dHit.lastSample to its index in roiHits.
        ///
        std::multimap<unsigned, unsigned> roiHitOrderTrail;

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

        std::vector<int> pcaIds;

        PrincipalComponents principalComponents;
    };
}



#endif //PIXY_ROIMUX_EVENT_H
