#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
//#include "TApplication.h"
//#include "TBrowser.h"
//#include "TCanvas.h"
//#include "TFile.h"
//#include "TH2S.h"
#include "ChargeData.h"
#include "ChargeHits.h"
#include "RunParams.h"


int main(int argc, char** argv) {
    // Start time point for timer.
    auto clkStart = std::chrono::high_resolution_clock::now();

    // TApplication for ROOT graphics.
    /*TApplication app("VIPER Reconstruction ROOT Application", &argc, argv);

    // Some tests of ROOT graphics...
    TBrowser browser;
    TCanvas canvas;*/

    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " runId rootFileName csvBaseFileName" << std::endl;
        exit(1);
    }
    const unsigned runId = static_cast<unsigned>(std::stoul(argv[1]));
    const std::string rootFileName = std::string(argv[2]);
    const std::string csvBaseFileName = std::string(argv[3]);
    const unsigned subrunId = 0;
    const unsigned nSamples = 2000;

    // Create the VIPER runParams containing all the needed run parameters.
    const pixy_roimux::RunParams runParams(runId);

    // Load events directly from ROOT file.
    std::cout << "Extracting chargeData...\n";
    const pixy_roimux::ChargeData chargeData(rootFileName, subrunId, runParams, nSamples);

    // Find the chargeHits.
    std::cout << "Initialising hit finder...\n";
    pixy_roimux::ChargeHits chargeHits(chargeData, runParams);
    // Set up TSpectrum.
    chargeHits.setSpecParams(5., "nodraw", .2);
    std::cout << "Running hit finder...\n";
    chargeHits.findHits();

    // Write chargeHits of events in eventIds vector to CSV files so we can plot them with viper3Dplot.py afterwards.
    unsigned nHitCandidates = 0;
    unsigned nAmbiguities = 0;
    unsigned nUnmatchedPixelHits = 0;
    // Loop through events.
    for (const auto& event : chargeHits.getEvents()) {
        std::cout << "Writing event number " << event.eventId << " to file...\n";
        // Compose CSV filename and open file stream.
        const std::string csvFileName = csvBaseFileName + "_event" + std::to_string(event.eventId) + ".csv";
        std::ofstream csvFile(csvFileName, std::ofstream::out);
        csvFile << "X,Y,Z,Q" << std::endl;
        // viperEvents.hitCandidates is a vector of vectors so we need two loops.
        // Outer loop. Actually loops through pixel chargeHits of current event.
        for (const auto& hitCandidates : event.hitCandidates) {
            // Inner loop. Loops through all hit candidates of current pixel hit.
            for (const auto& hit : hitCandidates) {
                // Append coordinates and charge to file.
                csvFile << hit.x << "," << hit.y << "," << hit.z << "," << hit.charge << std::endl;
            }
        }
        // Close CSV file.
        csvFile.close();
        // Calculate some stats.
        // Loop over all pixel chargeHits using the pixel to ROI hit runParams.
        for (const auto& candidateRoiHitIds : event.pixel2roi) {
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
    }
    unsigned long nEvents = chargeHits.getEvents().size();
    float averageHitCandidates = static_cast<float>(nHitCandidates) / static_cast<float>(nEvents);
    float averageAmbiguities = static_cast<float>(nAmbiguities) / static_cast<float>(nEvents);
    float averageUnmatchedPixelHits = static_cast<float>(nUnmatchedPixelHits) / static_cast<float>(nEvents);
    const std::string statsFileName = csvBaseFileName + "_stats.txt";
    std::ofstream statsFile(statsFileName, std::ofstream::out);
    statsFile << "Number of events processed: " << nEvents << std::endl;
    statsFile << "Average number of hit candidates per event: " << averageHitCandidates << std::endl;
    statsFile << "Average number of ambiguities per event: " << averageAmbiguities << std::endl;
    statsFile << "Average number of unmatched pixel chargeHits per event: " << averageUnmatchedPixelHits << std::endl;
    statsFile.close();

    std::cout << "Done.\n";

    // Stop time point for timer.
    auto clkStop = std::chrono::high_resolution_clock::now();
    // Calculate difference between timer start and stop.
    auto clkDuration = std::chrono::duration_cast<std::chrono::milliseconds>(clkStop - clkStart);
    std::cout << "Elapsed time for " << chargeHits.getEvents().size() << " processed events is: "
              << clkDuration.count() << "ms\n";

    // Keep ROOT graphics open.
    //app.Run();

    return 0;
}