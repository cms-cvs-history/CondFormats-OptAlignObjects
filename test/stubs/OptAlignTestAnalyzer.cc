// -*- C++ -*-
//
// Package:    CondFormats/OptAlignObjects/test/stubs
// Class:      OptAlignTestAnalyzer.cc
// 
/**\class OptAlignTestAnalyzer OptAlignTestAnalyzer.cc CondFormats/OptAlignObjects/test/stubs/OptAlignTestAnalyzer.cc

 Description: test access to the OptAlignments via PoolDBESSource

 Implementation:
     Iterate over retrieved alignments.
*/
//
// Original Author:  Mike Case
//         Created:  Mon Jan 17 11:47:40 CET 2006
// $Id$
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

#include "CondFormats/OptAlignObjects/interface/OpticalAlignments.h"
#include "CondFormats/DataRecord/interface/OpticalAlignmentsRcd.h"

#include "CLHEP/Matrix/Matrix.h"

using namespace std;

  class OptAlignTestAnalyzer : public edm::EDAnalyzer
  {
  public:
    explicit  OptAlignTestAnalyzer(edm::ParameterSet const& p) 
    { }
    explicit  OptAlignTestAnalyzer(int i) 
    { }
    virtual ~ OptAlignTestAnalyzer() { }
    virtual void analyze(const edm::Event& e, const edm::EventSetup& c);
  private:
  };

  void
   OptAlignTestAnalyzer::analyze(const edm::Event& e, const edm::EventSetup& context)
  {
    using namespace edm::eventsetup;
    // Context is not used.
    std::cout <<" I AM IN RUN NUMBER "<<e.id().run() <<std::endl;
    std::cout <<" ---EVENT NUMBER "<<e.id().event() <<std::endl;
    edm::ESHandle<OpticalAlignments> oa;
    context.get<OpticalAlignmentsRcd>().get(oa);

    //  just iterate over all of them...
    const OpticalAlignments* myoa=oa.product();

    std::cout << *myoa << std::endl;

  } //end of ::Analyze()
  DEFINE_FWK_MODULE(OptAlignTestAnalyzer)
