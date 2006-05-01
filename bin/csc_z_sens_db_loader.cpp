/**
 * This is an example application for reading ASCII files into memory and writing
 * them out 
 **/

// Command line options from Pool and Seal
#include "POOLCore/CommandOptions.h"
#include "SealBase/ShellEnvironment.h"

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "CondCore/DBCommon/interface/DBSession.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/DBCommon/interface/ServiceLoader.h"
#include "CondCore/DBCommon/interface/ConnectMode.h"
#include "CondCore/DBCommon/interface/MessageLevel.h"
#include "FWCore/Framework/interface/IOVSyncValue.h"

// OptAlignCSCFileReader reads comma separated ASCII files 
// and provides them as a vector of strings.
#include "CondFormats/OptAlignObjects/interface/OptAlignCSCFileReader.h"

// CSCZSensors are the objects being written into the framework Pool
// format. 
#include "CondFormats/OptAlignObjects/interface/CSCZSensors.h"
#include "CondCore/DBCommon/interface/DBWriter.h"
#include "CondFormats/OptAlignObjects/interface/OpticalAlignments.h"
#include "CondCore/IOVService/interface/IOV.h"
#include "SealKernel/Service.h"
#include "POOLCore/POOLContext.h"
#include "SealKernel/Context.h"
#include "CondCore/MetaDataService/interface/MetaData.h"

#include <vector>
#include <map>

namespace std { } using namespace std;

int  main( int argc, char** argv )
{
  std::vector<pool::CommandOptions::Option> ops;
  pool::CommandOptions::Option opt;
  opt.flag = "-s";
  opt.property = "sourceFile";
  opt.type = pool::CommandOptions::Option::STRING;
  opt.helpEntry = "the source text source file name";
  opt.required = true;
  ops.push_back(opt);
  opt.flag = "-c";
  opt.property = "connectionString";
  opt.type = pool::CommandOptions::Option::STRING;
  opt.helpEntry = "the connection string";
  opt.required = true;
  ops.push_back(opt);
  opt.flag = "-n";
  opt.property = "calibrationName";
  opt.type = pool::CommandOptions::Option::STRING;
  opt.helpEntry = "name of name of the calibration being stored";
  opt.required = true;
  ops.push_back(opt);
  opt.flag = "-u";
  opt.property = "user";
  opt.type = pool::CommandOptions::Option::STRING;
  opt.helpEntry = "the user name, required for DB servers (e.g. ORACLE)";
  opt.required = false;
  ops.push_back(opt);
  opt.flag = "-p";
  opt.property = "password";
  opt.type = pool::CommandOptions::Option::STRING;
  opt.helpEntry = "the user password, required for DB servers (e.g. ORACLE)";
  opt.required = false;
  ops.push_back(opt);
  opt.flag = "-t";
  opt.property = "type";
  opt.type = pool::CommandOptions::Option::STRING;
  opt.helpEntry = "the type or table analogue";
  opt.required = true;
  ops.push_back(opt);
  opt.flag = "-h";
  opt.property = "help";
  opt.type = pool::CommandOptions::Option::BOOLEAN;
  opt.helpEntry = "display help";
  opt.required = false;  
  ops.push_back(opt);
  pool::CommandOptions cmd(ops);

  try {
    cmd.parse(argc,argv);
    const std::map<std::string,std::string>& opsOut = cmd.userOptions();
    std::map<std::string,std::string>::const_iterator iter = opsOut.find("help");
    if(iter!=opsOut.end() || opsOut.size()==0){
      cmd.help();
    } else {
    
      std::string theSourceFile;
      iter = opsOut.find("sourceFile");
      if(iter!=opsOut.end()){
	theSourceFile = (*iter).second;
      }      
      std::string theConnectionString;
      iter = opsOut.find("connectionString");
      if(iter!=opsOut.end()){
	theConnectionString = (*iter).second;
      } 
      std::string theUserName;
      iter = opsOut.find("user");
      if(iter!=opsOut.end()){
	theUserName = (*iter).second;
      } 
      std::string thePassword;
      iter = opsOut.find("password");
      if(iter!=opsOut.end()){
	thePassword = (*iter).second;
      } 
      std::string theCalibrationName;
      iter = opsOut.find("calibrationName");
      if(iter!=opsOut.end()){
	theCalibrationName = (*iter).second;
      } 
      std::string theType;
      iter = opsOut.find("type");
      if(iter!=opsOut.end()){
	theType = (*iter).second;
      } 
      
      OptAlignCSCFileReader oacfr( theSourceFile );
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
			  std::cout << "Sensor_type should be CHAR" << endl;
			}
		      } else if ( names[i] == "Sensor_number" ) {
			if ( types[i] == "INT" ) {
			  st >> csczsd.sensorNo_;
			} else {
			  std::cout << "Sensor_number should be INT" << endl;
			}
		      } else if ( names[i] == "ME_layer" ) {
			if ( types[i] == "CHAR" ) {
			  std::string tstr;
			  while ( !st.eof() ) {
			    st >> tstr;
			    csczsd.meLayer_ = csczsd.meLayer_ + ' ' + tstr;
			  }
			} else {
			  std::cout << "ME_layer should be CHAR" << endl;
			}
		      } else if ( names[i] == "Logical_Alignment_Name" ) {
			if ( types[i] == "CHAR" ) {
			  std::string tstr;
			  while ( !st.eof() ) {
			    st >> tstr;
			    csczsd.logicalAlignmentName_ = csczsd.logicalAlignmentName_ + ' ' + tstr;
			  }
			} else {
			  std::cout << "Logical_Alignment_Name should be CHAR" << endl;
			}
		      } else if ( names[i] == "CERN_Designator" ) {
			if ( types[i] == "CHAR" ) {
			  std::string tstr;
			  while ( !st.eof() ) {
			    st >> tstr;
			    csczsd.cernDesignator_ = csczsd.cernDesignator_ + ' ' + tstr;
			  }
			} else {
			  std::cout << "CERN_Designator should be CHAR" << endl;
			}
		      } else if ( names[i] == "CERN_Barcode" ) {
			if ( types[i] == "CHAR" ) {
			  std::string tstr;
			  while ( !st.eof() ) {
			    st >> tstr;
			    csczsd.cernBarcode_ = csczsd.cernBarcode_ + ' ' + tstr;
			  }
			} else {
			  std::cout << "CERN_Barcode should be CHAR" << endl;
			}
		      } else if ( names[i] == "Abs_Slope" ) {
			if ( types[i] == "FLOAT" ) {
			  st >> csczsd.absSlope_;
			} else {
			  std::cout << "Abs_Slope should be FLOAT" << endl;
			}
		      } else if ( names[i] == "Abs_Slope_Error" ) {
			if ( types[i] == "FLOAT" ) {
			  st >> csczsd.absSlopeError_;
			} else {
			  std::cout << "Abs_Slope_Error should be FLOAT" << endl;
			}

		      } else if ( names[i] == "Norm_Slope" ) {
			if ( types[i] == "FLOAT" ) {
			  st >> csczsd.normSlope_;
			} else {
			  std::cout << "Norm_Slope should be FLOAT" << endl;
			}

		      } else if ( names[i] == "Norm_Slope_Error" ) {
			if ( types[i] == "FLOAT" ) {
			st >> csczsd.normSlopeError_;
			} else {
			  std::cout << "Norm_Slope_Error should be FLOAT" << endl;
			}
		      } else if ( names[i] == "Abs_Intercept" ) {
			if ( types[i] == "FLOAT" ) {
			st >> csczsd.absIntercept_;
			} else {
			  std::cout << "Abs_Intercept should be FLOAT" << endl;
			}
		      } else if ( names[i] == "Abs_Intercept_Error" ) {
			if ( types[i] == "FLOAT" ) {
			  st >> csczsd.absInterceptError_;
			} else {
			  std::cout << "Abs_Intercep_Error should be FLOAT" << endl;
			}
		      } else if ( names[i] == "Norm_Intercept" ) {
			if ( types[i] == "FLOAT" ) {
			  st >> csczsd.normIntercept_;
			} else {
			  std::cout << "Norm_Intercept should be FLOAT" << endl;
			}
		      } else if ( names[i] == "Norm_Intercept_Error" ) {
			if ( types[i] == "FLOAT" ) {
			st >> csczsd.normInterceptError_;
			} else {
			  std::cout << "Norm_Intercept_Error should be FLOAT" << endl;
			}
		      } else if ( names[i] == "Shifts_due_to_shims_etc" ) {
			if ( types[i] == "FLOAT" ) {
			st >> csczsd.shifts_;
			} else {
			  std::cout << "Shifts_due_to_shims_etc should be FLOAT" << endl;
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
	      std::cout << "Filled " <<  (csczs->cscZSens_).size() << " sensor data to " << theConnectionString;
	      std::cout << " ... " << std::endl;
	      //	      std::cout << " string of first is: |" << csczs->cscZSens_[0].sensorType_ << "|" << std::endl;
	      pool::POOLContext::loadComponent( "SEAL/Services/MessageService" );
	      pool::POOLContext::setMessageVerbosityLevel( seal::Msg::Info );
	      cond::ServiceLoader* loader=new cond::ServiceLoader;
	      ::putenv("CORAL_AUTH_USER=me");
	      ::putenv("CORAL_AUTH_PASSWORD=mypass");
	      loader->loadAuthenticationService( cond::Env );
	      loader->loadMessageService( cond::Error );
	      cond::DBSession* session=new cond::DBSession(theConnectionString);
	      session->setCatalog("file:PoolFileCatalog.xml");
	      session->connect(cond::ReadWriteCreate);
	      cond::DBWriter pw(*session, theType);
	      cond::DBWriter iovw(*session, "IOV");
	      cond::IOV* initiov=new cond::IOV;
	      session->startUpdateTransaction();
	      std::string tok=pw.markWrite<CSCZSensors>(csczs);
	      unsigned long myTime=(unsigned long)edm::IOVSyncValue::endOfTime().eventID().run();
	      //	      myIov->iov[mytime]=tk.tokenAsString();
	      std::cout << "The end-of-time is " << myTime << std::endl;
	      initiov->iov.insert(std::make_pair(myTime,tok));
	      std::string iovtok = iovw.markWrite<cond::IOV>(initiov);
	      session->commit();
	      session->disconnect();
	      cond::MetaData metadata_svc(theConnectionString, *loader);
	      metadata_svc.connect();
	      metadata_svc.addMapping(theCalibrationName, iovtok);
	      metadata_svc.disconnect();
	      std::cout << "Done with save, token " << tok << " as name " << theCalibrationName  << std::endl;
	      delete session;
	    } // end else of if names.size=types.size
	  } // end if got types
	} // end if got names 
      } // if got  types

      return 0;
    }
  }  catch(const cond::Exception& er){
    std::cout<<er.what()<<std::endl;
    exit(-1);
  } catch ( std::exception& se ) {
    std::cout << se.what() << std::endl;
  } catch ( ... ) {
    std::cout << "some other error" << std::endl;
  }


  return 0;
}
