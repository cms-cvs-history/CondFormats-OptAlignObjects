#include "CSCZSensorLoadSinglePayload.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ServiceRegistry/interface/Service.h"

#include "CondCore/DBOutputService/interface/PoolDBOutputService.h"

#include "CondFormats/OptAlignObjects/interface/CSCZSensors.h"
#include "CondFormats/OptAlignObjects/interface/OpticalAlignments.h"

#include "CondFormats/DataRecord/interface/CSCZSensorsRcd.h"

#include "CondFormats/OptAlignObjects/interface/OptAlignCSCFileReader.h"

#include "FWCore/ParameterSet/interface/FileInPath.h"

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

CSCZSensorLoadSinglePayload::CSCZSensorLoadSinglePayload(const edm::ParameterSet& iConfig)
{
  std::cout<<"CSCZSensorLoadSinglePayload::CSCZSensorLoadSinglePayload"<<std::endl;
  sourceFile_ = iConfig.getParameter<edm::FileInPath>("SourceFile").fullPath();
}

CSCZSensorLoadSinglePayload::~CSCZSensorLoadSinglePayload()
{
  std::cout<<"CSCZSensorLoadSinglePayload::CSCZSensorLoadSinglePayload"<<std::endl;
}

void
CSCZSensorLoadSinglePayload::analyze(const edm::Event& evt, 
				     const edm::EventSetup& evtSetup){
  std::cout << "CSCZSensorLoadSinglePayload::analyze does nothing!" << std::endl;
}


void
CSCZSensorLoadSinglePayload::endJob()
{
  std::cout<<"CSCZSensorLoadSinglePayload::endJob does all the work."<<std::endl;
  OptAlignCSCFileReader oacfr( sourceFile_ );
  std::vector<std::string> names, types;
  oacfr.next();
  if ( oacfr.getData(names) ) {
    if ( oacfr.next() ) {
      if ( oacfr.getData(types) ) {
	if ( names.size() != types.size() ) { 
	  std::cout << "Invalid file header.  Need names and types." << std::endl;
	  std::cout << "names.size() = " << names.size() << "  types.size() = " << types.size() << std::endl;
	} else {
	  std::vector<std::string> stringData;
	  CSCZSensors* csczs = new CSCZSensors;
	  while ( oacfr.next() ) {
	    stringData.clear();
	    oacfr.getData(stringData);
	    CSCZSensorData csczsd;
	    if ( stringData.size() > 0 ) {
	      if ( types.size() != stringData.size() ) {
		std::cout << "Error:  line does not have enough data." << std::endl;
	      } else {
		for ( size_t i = 0; i < types.size(); i++ ) {
		  // used to convert strings to numbers.
		  std::istringstream st( stringData[i] );
		  if ( names[i] == "" ) {
		    // do nothing if there is no name! => that column is NOT going into the DB.
		    // since it is not processed here.
		  } else if ( names[i] == "Sensor_type" ) {
		    // for each data-member of your object, you need an if like this.
		    if ( types[i] == "CHAR" ) {
		      std::string tstr;
		      while ( !st.eof() ) {
			st >> tstr;
			csczsd.sensorType_ = csczsd.sensorType_ + ' ' + tstr;
		      }
		    } else {
		      std::cout << "Sensor_type should be CHAR" << std::endl;
		    }
		  } else if ( names[i] == "Sensor_number" ) {
		    if ( types[i] == "INT" ) {
		      st >> csczsd.sensorNo_;
		    } else {
		      std::cout << "Sensor_number should be INT" << std::endl;
		    }
		  } else if ( names[i] == "ME_layer" ) {
		    if ( types[i] == "CHAR" ) {
		      std::string tstr;
		      while ( !st.eof() ) {
			st >> tstr;
			csczsd.meLayer_ = csczsd.meLayer_ + ' ' + tstr;
		      }
		    } else {
		      std::cout << "ME_layer should be CHAR" << std::endl;
		    }
		  } else if ( names[i] == "Logical_Alignment_Name" ) {
		    if ( types[i] == "CHAR" ) {
		      std::string tstr;
		      while ( !st.eof() ) {
			st >> tstr;
			csczsd.logicalAlignmentName_ = csczsd.logicalAlignmentName_ + ' ' + tstr;
		      }
		    } else {
		      std::cout << "Logical_Alignment_Name should be CHAR" << std::endl;
		    }
		  } else if ( names[i] == "CERN_Designator" ) {
		    if ( types[i] == "CHAR" ) {
		      std::string tstr;
		      while ( !st.eof() ) {
			st >> tstr;
			csczsd.cernDesignator_ = csczsd.cernDesignator_ + ' ' + tstr;
		      }
		    } else {
		      std::cout << "CERN_Designator should be CHAR" << std::endl;
		    }
		  } else if ( names[i] == "CERN_Barcode" ) {
		    if ( types[i] == "CHAR" ) {
		      std::string tstr;
		      while ( !st.eof() ) {
			st >> tstr;
			csczsd.cernBarcode_ = csczsd.cernBarcode_ + ' ' + tstr;
		      }
		    } else {
		      std::cout << "CERN_Barcode should be CHAR" << std::endl;
		    }
		  } else if ( names[i] == "Abs_Slope" ) {
		    if ( types[i] == "FLOAT" ) {
		      st >> csczsd.absSlope_;
		    } else {
		      std::cout << "Abs_Slope should be FLOAT" << std::endl;
		    }
		  } else if ( names[i] == "Abs_Slope_Error" ) {
		    if ( types[i] == "FLOAT" ) {
		      st >> csczsd.absSlopeError_;
		    } else {
		      std::cout << "Abs_Slope_Error should be FLOAT" << std::endl;
		    }

		  } else if ( names[i] == "Norm_Slope" ) {
		    if ( types[i] == "FLOAT" ) {
		      st >> csczsd.normSlope_;
		    } else {
		      std::cout << "Norm_Slope should be FLOAT" << std::endl;
		    }

		  } else if ( names[i] == "Norm_Slope_Error" ) {
		    if ( types[i] == "FLOAT" ) {
		      st >> csczsd.normSlopeError_;
		    } else {
		      std::cout << "Norm_Slope_Error should be FLOAT" << std::endl;
		    }
		  } else if ( names[i] == "Abs_Intercept" ) {
		    if ( types[i] == "FLOAT" ) {
		      st >> csczsd.absIntercept_;
		    } else {
		      std::cout << "Abs_Intercept should be FLOAT" << std::endl;
		    }
		  } else if ( names[i] == "Abs_Intercept_Error" ) {
		    if ( types[i] == "FLOAT" ) {
		      st >> csczsd.absInterceptError_;
		    } else {
		      std::cout << "Abs_Intercep_Error should be FLOAT" << std::endl;
		    }
		  } else if ( names[i] == "Norm_Intercept" ) {
		    if ( types[i] == "FLOAT" ) {
		      st >> csczsd.normIntercept_;
		    } else {
		      std::cout << "Norm_Intercept should be FLOAT" << std::endl;
		    }
		  } else if ( names[i] == "Norm_Intercept_Error" ) {
		    if ( types[i] == "FLOAT" ) {
		      st >> csczsd.normInterceptError_;
		    } else {
		      std::cout << "Norm_Intercept_Error should be FLOAT" << std::endl;
		    }
		  } else if ( names[i] == "Shifts_due_to_shims_etc" ) {
		    if ( types[i] == "FLOAT" ) {
		      st >> csczsd.shifts_;
		    } else {
		      std::cout << "Shifts_due_to_shims_etc should be FLOAT" << std::endl;
		    }
		  } 
		} // end for loop for each of the data lines.
		stringData.clear();
	      }
	      csczs->cscZSens_.push_back(csczsd);
	    } else {
	      std::cout << "Skipping blank line" << std::endl;
	    } // end if size()>0 else
	  } // end while
	  std::cout << "Filled " <<  (csczs->cscZSens_).size() << " into CSCZSensors" <<std::endl;  
	  edm::Service<cond::service::PoolDBOutputService> mydbservice;
	  if( mydbservice.isAvailable() ){
	    try{
	      mydbservice->newValidityForNewPayload<CSCZSensors>(csczs,mydbservice->endOfTime());
	    }catch(const cond::Exception& er){
	      std::cout<<er.what()<<std::endl;
	    }catch(const std::exception& er){
	      std::cout<<"caught std::exception "<<er.what()<<std::endl;
	    }catch(...){
	      std::cout<<"Funny error"<<std::endl;
	    }
	  }else{
	    std::cout<<"Service is unavailable"<<std::endl;
	  }
	} // else of if names.size() != types.size()
      }  // if oacfr.getData(types)
    }   // if oacfr.next()
  }    // if oacfr.getData(names)
}
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(CSCZSensorLoadSinglePayload)
