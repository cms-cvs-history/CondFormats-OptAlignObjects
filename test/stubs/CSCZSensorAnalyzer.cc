// -*- C++ -*-
//
// Package:    CondFormats/OptAlignObjects/test/stubs
// Class:      CSCZSensorAnalyzer.cc
// 
/**\class CSCZSensorAnalyzer CSCZSensorAnalyzer.cc CondFormats/OptAlignObjects/test/stubs/CSCZSensorAnalyzer.cc

 Description: test access to the OptAlignments via PoolDBESSource

 Implementation:
     Iterate over retrieved alignments.
*/
//
// Original Author:  Mike Case
//         Created:  Mon Jan 17 11:47:40 CET 2006
// $Id: CSCZSensorAnalyzer.cc,v 1.1 2006/01/26 13:44:15 case Exp $
//
//

#include <stdexcept>
#include <string>
#include <iostream>
#include <map>
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "CondFormats/OptAlignObjects/interface/CSCZSensors.h"
#include "CondFormats/DataRecord/interface/CSCZSensorsRcd.h"

#include "CLHEP/Matrix/Matrix.h"

using namespace std;

  class CSCZSensorAnalyzer : public edm::EDAnalyzer
  {
  public:
    explicit  CSCZSensorAnalyzer(edm::ParameterSet const& p) 
    { }
    explicit  CSCZSensorAnalyzer(int i) 
    { }
    virtual ~ CSCZSensorAnalyzer() { }
    virtual void analyze(const edm::Event& e, const edm::EventSetup& c);
  private:
  };

  void
   CSCZSensorAnalyzer::analyze(const edm::Event& e, const edm::EventSetup& context)
  {
    using namespace edm::eventsetup;
    // Context is not used.
    std::cout <<" I AM IN RUN NUMBER "<<e.id().run() <<std::endl;
    std::cout <<" ---EVENT NUMBER "<<e.id().event() <<std::endl;
    edm::ESHandle<CSCZSensors> csczsHandle;
    context.get<CSCZSensorsRcd>().get(csczsHandle);

    //  just iterate over all of them...
    const CSCZSensors* csczs = csczsHandle.product();

    std::cout << "sensorNo ";
    std::cout << "meLayer ";
    std::cout << "logicalAlignmentName ";
    std::cout << "cernDesignator ";
    std::cout << "cernBarcode ";
    std::cout << "absSlope ";
    std::cout << "absSlopeError ";
    std::cout << "normSlope ";
    std::cout << "normSlopeError ";
    std::cout << "absIntercept ";
    std::cout << "absInterceptError ";
    std::cout << "normIntercept ";
    std::cout << "normInterceptError ";
    std::cout << "shifts ";
    std::cout << std::endl;

    for ( std::vector<CSCZSensorData>::const_iterator vit = csczs->cscZSens_.begin();
	  vit != csczs->cscZSens_.end(); ++vit ) {
      std::cout << vit->sensorNo_ << " " << vit->meLayer_ << " ";
      std::cout << vit->logicalAlignmentName_ << " " << vit->cernDesignator_ << " ";
      std::cout << vit->cernBarcode_ << " " << vit->absSlope_ << " ";
      std::cout << vit->absSlopeError_ << " " << vit->normSlope_ << " ";
      std::cout << vit->normSlopeError_ << " " << vit->absIntercept_ << " ";
      std::cout << vit->absInterceptError_ << " " << vit->shifts_ << std::endl;
    }
  } //end of ::Analyze()
  DEFINE_FWK_MODULE(CSCZSensorAnalyzer)
