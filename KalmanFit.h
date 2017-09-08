//
// Created by damian on 6/22/17.
//

#ifndef PIXY_ROIMUX_KALMANFIT_H
#define PIXY_ROIMUX_KALMANFIT_H


#include <iostream>
#include <map>
#include <memory>
#include "TClonesArray.h"
#include "TFile.h"
#include "TGeoManager.h"
#include "TMatrixT.h"
#include "TRandom3.h"
#include "TTree.h"
#include "TVector3.h"
#include "AbsMeasurement.h"
#include "ConstField.h"
#include "Exception.h"
#include "EventDisplay.h"
#include "FieldManager.h"
#include "DAF.h"
#include "MaterialEffects.h"
#include "mySpacepointDetectorHit.h"
#include "mySpacepointMeasurement.h"
#include "MeasurementFactory.h"
#include "MeasurementProducer.h"
#include "RKTrackRep.h"
#include "TGeoMaterialInterface.h"
#include "Track.h"
#include "TrackCand.h"
#include "ChargeHits.h"
#include "RunParams.h"


namespace pixy_roimux {
    class KalmanFit {
    public:
        KalmanFit(
                const RunParams &t_runParams,
                const std::string t_geoFileName,
                const bool t_initDisplay = false);

        void fit(
                const ChargeHits &t_chargeHits,
                const std::string t_treeFileName);

        void openEventDisplay() {
            if (m_display) {
                m_display->open();
            }
            else {
                std::cerr << "WARNING: Cannot open event display because it was not initialised!" << std::endl;
            }
        }


    private:
        void fitEvent(const Event &t_event);

        const RunParams &m_runParams;

        genfit::EventDisplay *m_display = nullptr;

        genfit::MeasurementFactory<genfit::AbsMeasurement> m_measurementFactory;

        genfit::MeasurementProducer<genfit::mySpacepointDetectorHit, genfit::mySpacepointMeasurement> m_measurementProducer;

        genfit::DAF m_kalmanFitter;

        genfit::Track *m_trackPtr = nullptr;

        const int m_detId = 1;

        std::unique_ptr<TTree> m_tree;

        TClonesArray m_hits;

        TRandom3 m_gaussRng;

        TMatrixDSym m_cov;

        TMatrixDSym m_posCov;

        unsigned m_eventId;
    };
}


#endif //PIXY_ROIMUX_KALMANFIT_H
