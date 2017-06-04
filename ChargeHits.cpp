//
// Created by damian on 6/3/17.
//

#include "ChargeHits.h"


pixy_roimux::ChargeHits::ChargeHits(
        const pixy_roimux::ChargeData& t_chargeData,
        const pixy_roimux::RunParams& t_runParams) :
        m_chargeData(t_chargeData),
        m_runParams(t_runParams)
{
    // Need to instantiate the TSpectrum manually, otherwise the code doesn't compile. No idea why...
    m_spectrum = std::unique_ptr<TSpectrum>(new TSpectrum());
}


void pixy_roimux::ChargeHits::find2dHits(
        const TH2S& histo,
        const double discFracLow,
        const double discFracHigh,
        std::vector<pixy_roimux::Hit2d>& hits,
        std::multimap<unsigned, unsigned>& hitOrderLow,
        std::multimap<unsigned, unsigned>& hitOrderHigh)
{
    // Clear the hit vector and maps from potential old data.
    hits.clear();
    hitOrderLow.clear();
    hitOrderHigh.clear();
    // Index of the hits vector needed for the ordered maps.
    unsigned hitId = 0;
    // Loop over all channels of the input histo.
    // Pay attention to bin numbers!!! Loops (and everything else) start at 0, histos start at 1!!!
    for (unsigned channel = 0; channel < histo.GetNbinsY(); ++channel) {
        // Get the histo of a single channel using the ProjectionX method of TH2.
        auto channelHisto = std::unique_ptr<TH1D>(histo.ProjectionX("channelHisto", (channel + 1), (channel + 1)));
        // Run TSpectrum's peak finder.
        m_spectrum->Search(channelHisto.get(), m_specSig, m_specOpt.c_str(), m_specThr);
        // Get the data from the peak finder.
        const int nPeaks = m_spectrum->GetNPeaks();
        const double* const posX = m_spectrum->GetPositionX();
        const double* const posY = m_spectrum->GetPositionY();
        // Loop through all found peaks.
        for (int peak = 0; peak < nPeaks; ++peak) {
            // Disard peak if it's below the set threshold.
            if (static_cast<int>(posY[peak]) < m_discThr) {
                continue;
            }
            // Start and end of the pulse.
            int firstSample;
            int lastSample;
            bool foundFirstSample = false;
            bool foundLastSample = false;
            // Loop to find the first/last sample of the pulse using constant fraction discrimination.
            // We only search in the specified range.
            for (int sampleOffset = 1; sampleOffset <= m_discRange; ++sampleOffset) {
                // First sample. Only look if we haven't found it yet.
                if (!foundFirstSample) {
                    firstSample = static_cast<int>(posX[peak]) - 1 - sampleOffset;
                    // Check whether we've crossed the constant fraction threshold.
                    if ((firstSample >= 0) &&
                        (channelHisto->GetBinContent(firstSample + 1) < (discFracLow * posY[peak]))) {
                        foundFirstSample = true;
                    }

                }
                // Last sample. Only look if we haven't found it yet.
                if (!foundLastSample) {
                    lastSample = static_cast<int>(posX[peak]) - 1 + sampleOffset;
                    // Check whether we've crossed the constant fraction threshold.
                    if ((lastSample < channelHisto->GetNbinsX()) &&
                        (channelHisto->GetBinContent(lastSample + 1) < (discFracHigh * posY[peak]))) {
                        foundLastSample = true;
                    }
                }
            }
            // If we detected both the rising and the falling edge, build a 2D hit.
            if (foundFirstSample && foundLastSample) {
                pixy_roimux::Hit2d hit;
                hit.channel = channel;
                hit.firstSample = static_cast<unsigned>(firstSample);
                hit.lastSample = static_cast<unsigned>(lastSample);
                hit.pulseWidth = hit.lastSample - hit.firstSample + 1;
                hit.peakSample = static_cast<unsigned>(posX[peak]) - 1;
                hit.pulseHeight = static_cast<int>(posY[peak]);
                hit.pulseIntegral = 0;
                hit.pulseRaw.clear();
                hit.pulseRaw.resize(hit.pulseWidth);
                // Raw pulse data vector iterator.
                auto pulseRaw = hit.pulseRaw.begin();
                // Loop over all pulse samples to store and integrate them.
                for (unsigned sample = hit.firstSample; sample <= hit.lastSample; ++sample) {
                    const int pulseData = static_cast<int>(channelHisto->GetBinContent(sample + 1));
                    hit.pulseIntegral += pulseData;
                    *pulseRaw = pulseData;
                    // Increment the raw pulse data vector iterator.
                    ++pulseRaw;
                }
                // Push the hit to the hits vector.
                hits.push_back(hit);
                // Insert the hit ID into the hit order maps. The key is the sample where the signal rises/falls
                // above/below the constant fraction.
                hitOrderLow.insert(std::pair<unsigned, unsigned>(firstSample, hitId));
                hitOrderHigh.insert(std::pair<unsigned, unsigned>(lastSample, hitId));
                // Increment the hit ID.
                ++hitId;
            }
        }
    }
}


void pixy_roimux::ChargeHits::find3dHits(Event &t_event) {
    // Clear the match vectors from potential old data.
    t_event.pixel2roi.clear();
    t_event.roi2pixel.clear();
    // Preallocate the match vectors.
    t_event.pixel2roi.resize(t_event.pixelHits.size());
    t_event.roi2pixel.resize(t_event.roiHits.size());
    // Loop through ROI hits, sorted by rising pulse edge.
    for (const auto& roiHitOrderEntry : t_event.roiHitOrderLow) {
        // Get the ROI hit ID from the map entry.
        const unsigned roiHitId = roiHitOrderEntry.second;
        // Loop over pixel hits, sorted by falling pulse edge.
        // First loop over all pixel hits falling in between the risng and the falling edge of the current ROI hit.
        for (auto pixelHitOrderEntry = t_event.pixelHitOrderHigh.lower_bound(t_event.roiHits.at(roiHitId).firstSample);
             pixelHitOrderEntry != t_event.pixelHitOrderHigh.upper_bound(t_event.roiHits.at(roiHitId).lastSample);
             ++pixelHitOrderEntry) {
            // Get the pixel hit ID from the map entry.
            const unsigned pixelHitId = pixelHitOrderEntry->second;
            // Append matches to the match vectors.
            // Because we're currently inside the ROI pulse, we're sure this is an actual match.
            t_event.pixel2roi.at(pixelHitId).push_back(roiHitId);
            t_event.roi2pixel.at(roiHitId).push_back(pixelHitId);
        }
        // Now loop from the end of the ROI pulse until twice the peak finding range m_discRange after the end of the
        // pulse. This is the maximum length a pixel pulse can have. Thus, outside this range, a match to this ROI hit
        // is not possible.
        for (auto pixelHitOrderEntry = t_event.pixelHitOrderHigh.lower_bound(t_event.roiHits.at(roiHitId).lastSample);
             pixelHitOrderEntry !=
             t_event.pixelHitOrderHigh.upper_bound(t_event.roiHits.at(roiHitId).lastSample + 2 * m_discRange);
             ++pixelHitOrderEntry) {
            // Get the pixel hit ID from the map entry.
            const unsigned pixelHitId = pixelHitOrderEntry->second;
            // Because we're no longer inside the ROI pulse, we need to check whether there's an actual overlap between
            // the pixel pulse and the ROI pulse.
            if (t_event.pixelHits.at(pixelHitId).firstSample <= t_event.roiHits.at(roiHitId).lastSample) {
                // If they actually overlap, append the match to the match vectors.
                t_event.pixel2roi.at(pixelHitId).push_back(roiHitId);
                t_event.roi2pixel.at(roiHitId).push_back(pixelHitId);
            }
        }
    }
}


void pixy_roimux::ChargeHits::buildHitCandidates(Event &t_event) {
    // Clear the hit candidate vector from potential old data.
    t_event.hitCandidates.clear();
    // Preallocate the hit candidate vector.
    t_event.hitCandidates.resize(t_event.pixelHits.size());
    // Pixel hits vector const iterator.
    auto pixelHit = t_event.pixelHits.cbegin();
    // Hit candidates vector iterator.
    auto hitCandidate = t_event.hitCandidates.begin();
    // Loop over all pixel hits in the pixel to ROI map.
    for (const auto& candidateRoiHitIds : t_event.pixel2roi) {
        // Loop over all ROI hit IDs matched to the current pixel hit.
        for (const auto& candidateRoiHitId : candidateRoiHitIds) {
            // Get the pixel ID from the pixel hits vector.
            const unsigned pixelId = pixelHit->channel;
            // Get the ROI ID from the roi hits vector of the event using the ROI hit ID from the pixel to ROI map.
            const unsigned roiId = t_event.roiHits.at(candidateRoiHitId).channel;
            // Build the 3D hit using the coordinates and calibration constants from the RunParams.
            pixy_roimux::Hit3d hit;
            // Calculate x,y,z in mm.
            hit.x = static_cast<float>((m_runParams.getRoiCoor(roiId, 0) + m_runParams.getPixelCoor(pixelId, 0)) *
                                       m_runParams.getPixelPitch());
            hit.y = static_cast<float>((m_runParams.getRoiCoor(roiId, 1) + m_runParams.getPixelCoor(pixelId, 1)) *
                                       m_runParams.getPixelPitch());
            hit.z = static_cast<float>(m_runParams.getDriftLength() - (pixelHit->peakSample - m_runParams.getAnodeSample()) *
                                                               m_runParams.getSampleTime() * m_runParams.getDriftSpeed());
            // Calculate charge in C.
            hit.charge = static_cast<float>(pixelHit->pulseIntegral * (m_runParams.getAdcLsb() / m_runParams.getPreampGain()));
            hitCandidate->push_back(hit);
        }
        // Increment the pixel hits and 3D hit candidates vector iterators.
        ++pixelHit;
        ++hitCandidate;
    }
}


void pixy_roimux::ChargeHits::findHits() {
    // Clear events vector in case there's old data in it.
    m_events.clear();
    // Preallocate fHits for speed.
    m_events.resize(m_chargeData.getReadoutHistos().size());
    // Raw data vector const iterator. We'll read the raw data from there event by event.
    // Gives us access to a pair of histos containing the pixel (first) and the ROI (second) data.
    auto eventData = m_chargeData.getReadoutHistos().cbegin();
    // Events vector iterator. We'll store the events we built from the raw data in there.
    auto event = m_events.begin();
    // Loop over all events using the event IDs vector.
    for (const auto& eventId : m_chargeData.getEventIds()) {
        std::cout << "Processing event number " << eventId << ":\n";
        std::cout << "Running 2D hit finder...\n";
        // Find pixel hits.
        find2dHits(eventData->first, m_discFracPixelLow, m_discFracPixelHigh,
                   event->pixelHits, event->pixelHitOrderLow, event->pixelHitOrderHigh);
        std::cout << "Found " << event->pixelHits.size() << " pixel hits.\n";
        // Find ROI hits.
        find2dHits(eventData->second, m_discFracRoiLow, m_discFracRoiHigh,
                   event->roiHits, event->roiHitOrderLow, event->roiHitOrderHigh);
        std::cout << "Found " << event->roiHits.size() << " ROI hits.\n";

        std::cout << "Running 3D hit finder...\n";
        // Search for matches between pixel and ROI 2D hits.
        find3dHits(*event);
        // Build 3D hit candidates from the matches.
        buildHitCandidates(*event);
        // Some statistics.
        // Number of hit candidates for this event.
        unsigned nHitCandidates = 0;
        // Number of ambiguous hit candidates for this event.
        unsigned nAmbiguities = 0;
        // Number of pixel hits for this event that couldn't be matched to any ROI hits.
        unsigned nUnmatchedPixelHits = 0;
        // Loop over all pixel hits using the pixel to ROI hit map.
        for (const auto& candidateRoiHitIds : event->pixel2roi) {
            // Add number of ROI hit candidates for current pixel hit.
            nHitCandidates += candidateRoiHitIds.size();
            // If there's no ROI hit candidates, increment the unmatched counter.
            if (candidateRoiHitIds.empty()) {
                ++nUnmatchedPixelHits;
            }
                // If there's more than one ROI hit candidate, increment the ambiguity counter.
            else if (candidateRoiHitIds.size() > 1) {
                ++nAmbiguities;
            }
        }
        std::cout << "Found " << nHitCandidates << " 3D hit candidates.\n";
        std::cout << "Found " << nAmbiguities << " ambiguities.\n";
        std::cout << "Failed to match " << nUnmatchedPixelHits << " pixel hits.\n";

        // Store run, subrun and event ID to the event struct.
        event->runId = m_runParams.getRunId();
        event->subrunId = m_chargeData.getSubrunId();
        event->eventId = eventId;

        // Increment raw data and events vector iterators.
        ++eventData;
        ++event;
    }
}