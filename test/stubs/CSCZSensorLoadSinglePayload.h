#include "FWCore/Framework/interface/EDAnalyzer.h"

#include <string>

namespace edm{
  class ParameterSet;
  class Event;
  class EventSetup;
}

//
// class decleration
//

class CSCZSensorLoadSinglePayload : public edm::EDAnalyzer {
 public:
  explicit CSCZSensorLoadSinglePayload(const edm::ParameterSet& iConfig );
  ~CSCZSensorLoadSinglePayload();
  virtual void analyze( const edm::Event&, const edm::EventSetup& );
  virtual void endJob();
 private:
  std::string sourceFile_;
  // ----------member data ---------------------------
};
