#include "POOLCore/CommandOptions.h"

#include "SealBase/ShellEnvironment.h"

#include "boost/spirit/core.hpp"
#include "boost/any.hpp"

#include<iostream>
// #include<istream>
// #include<fstream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "CondCore/DBCommon/interface/DBSession.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/DBCommon/interface/ServiceLoader.h"
#include "CondCore/DBCommon/interface/ConnectMode.h"
#include "CondCore/DBCommon/interface/MessageLevel.h"

#include "CondFormats/OptAlignObjects/interface/OptAlignCSCFileReader.h"
#include "CondFormats/OptAlignObjects/interface/CSCZSensors.h"
#include "CondCore/DBCommon/interface/DBWriter.h"
#include "CondFormats/OptAlignObjects/interface/OpticalAlignments.h"
#include "CondCore/IOVService/interface/IOV.h"
#include "SealKernel/Service.h"
#include "POOLCore/POOLContext.h"
#include "SealKernel/Context.h"
#include "CondCore/MetaDataService/interface/MetaData.h"

#include <vector>
// #include <sstream>
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
  //   opt.flag = "-p";
  //   opt.property = "specParConfig";
  //   opt.type = pool::CommandOptions::Option::STRING;
  //   opt.helpEntry = "the specPar XML configuration file name";
  //   opt.required = true;
  //   ops.push_back(opt);
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
//   //not used right now.
//   opt.flag = "-debug";
//   opt.property = "debug";
//   opt.type = pool::CommandOptions::Option::BOOLEAN;
//   opt.required = false;
//   opt.helpEntry = "enable the verbose mode";
//   ops.push_back(opt);  
  // help (only) 
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
      //      std::cout << "1" << std::endl;
      if ( oacfr.getData(names) ) {
	if ( oacfr.next() ) {
	  //	  std::cout << "2" << std::endl;
	  if ( oacfr.getData(types) ) {
	    //	    std::cout << "3" << std::endl;
	    if ( names.size() != types.size() ) { 
	      std::cout << "Invalid file header.  Need names and types." << std::endl;
	      std::cout << "names.size() = " << names.size() << "  types.size() = " << types.size() << std::endl;
	    } else {
	      // 	    std::cout << "name-type: " ;
	      // 	    for ( size_t i = 0 ; i < names.size() ; i++ ) {
	      // 	      std::cout << names[i] << "-" << types[i] << ", ";
	      // 	    }
	      // 	    std::cout << std::endl << std::endl;
	      std::vector<std::string> stringData;
	      //	      std::vector<double> dblData;

	      CSCZSensorData csczsd;
	      CSCZSensors* csczs = new CSCZSensors;

	      while ( oacfr.next() ) {
		stringData.clear();
		oacfr.getData(stringData);
		if ( stringData.size() > 0 ) {
		  std::cout << "stringData.size()=" << stringData.size() << std::endl;
		  if ( types.size() != stringData.size() ) {
		    std::cout << "Error:  line does not have enough data." << std::endl;
		    std::cout << "types.size()=" << types.size() << "  stringData.size()=" << stringData.size() << std::endl;		  
		  } else {
		    std::cout << "types.size()=" << types.size() << "  stringData.size()=" << stringData.size() << std::endl;
		    for ( size_t i = 0; i < types.size(); i++ ) {
		      std::istringstream st( stringData[i] );
		      
		      if ( names[i] == "" ) {
		      } else if ( names[i] == "Sensornumber" ) {
			st >> csczsd.sensorNo_;
		      } else if ( names[i] == "MElayer" ) {
			st >> csczsd.meLayer_;
		      } else if ( names[i] == "Logical_Alignment_Name" ) {
			st >> csczsd.logicalAlignmentName_;
		      } else if ( names[i] == "CERN_Designator" ) {
			st >> csczsd.cernDesignator_;
		      } else if ( names[i] == "CERN_Barcode" ) {
			st >> csczsd.cernBarcode_;
		      } else if ( names[i] == "Abs_Slope" ) {
			st >> csczsd.absSlope_;
		      } else if ( names[i] == "Abs_Slope_Error" ) {
			st >> csczsd.absSlopeError_;
		      } else if ( names[i] == "Norm_Slope" ) {
			st >> csczsd.normSlope_;
		      } else if ( names[i] == "Norm_Slope_Error" ) {
			st >> csczsd.normSlopeError_;
		      } else if ( names[i] == "Abs_Intercept" ) {
			st >> csczsd.absIntercept_;
		      } else if ( names[i] == "Abs_Intercept_Error" ) {
			st >> csczsd.absInterceptError_;
		      } else if ( names[i] == "Norm_Intercept" ) {
			st >> csczsd.normIntercept_;
		      } else if ( names[i] == "Norm_Intercept_Error" ) {
			st >> csczsd.normInterceptError_;
		      } else if ( names[i] == "Shifts" ) {
			st >> csczsd.shifts_;
		      } 
		    } // end for loop for each of the data lines.
		    stringData.clear();
		  }
		  csczs->cscZSens_.push_back(csczsd);
		} else {
		  std::cout << "Skipping blank line" << std::endl;
		} // end if size()>0 else
	      } // end while
	      pool::POOLContext::loadComponent( "SEAL/Services/MessageService" );
	      pool::POOLContext::setMessageVerbosityLevel( seal::Msg::Info );
	      cond::ServiceLoader* loader=new cond::ServiceLoader;
	      ::putenv("CORAL_AUTH_USER=me");
	      ::putenv("CORAL_AUTH_PASSWORD=mypass");
	      loader->loadAuthenticationService( cond::Env );
	      loader->loadMessageService( cond::Error );
	      cond::DBSession* session=new cond::DBSession(theConnectionString);
	      session->setCatalog("file:CSCPoolCatalog.xml");
	      session->connect(cond::ReadWriteCreate);
	      cond::DBWriter pw(*session, theType);
	      cond::DBWriter iovw(*session, "IOV");
	      cond::IOV* initiov=new cond::IOV;
	      session->startUpdateTransaction();
	      std::string tok=pw.markWrite<CSCZSensors>(csczs);
	      initiov->iov.insert(std::make_pair(1,tok));
	      iovw.markWrite<cond::IOV>(initiov);
	      session->commit();
	      cond::MetaData metadata_svc(theConnectionString, *loader);
	      metadata_svc.connect();
	      metadata_svc.addMapping(theCalibrationName, tok);
	      metadata_svc.disconnect();
	      std::cout << "Wrote " << csczs->cscZSens_.size() << " sensor data to " << theConnectionString;
	      std::cout << " with token " << tok << " as name " << theCalibrationName  << std::endl;
	      delete session;
	    } // end else of if names.size=types.size
	  } // end if got types
	} // end if got names 
      } // if got  types

      return 0;
    }
  }  catch (const seal::Exception& e){
    std::cout << "ERROR: " << e.what() << std::endl;
    return 1;
  } catch ( std::exception& se ) {
    std::cout << se.what() << std::endl;
  } catch(const cond::Exception& er){
    std::cout<<er.what()<<std::endl;
    //delete loader;
    exit(-1);
  } catch ( ... ) {
    std::cout << "some other error" << std::endl;
  }


  return 0;
}
