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
#include "FWCore/Framework/interface/IOVSyncValue.h"

#include "CondFormats/OptAlignObjects/interface/OptAlignCSCFileReader.h"
#include "CondFormats/OptAlignObjects/interface/MBAChBenchSurveyPlate.h"
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
  opt.property = "type";//std::string tok1=w.write<OpticalAlignments>(oa,"OpticalAlignments");
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

		// get ready to write data to db
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
		std::vector<long long> times;
		std::vector<std::string> payloadTokens;

      
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

	      MBAChBenchSurveyPlateData mbachbenchsurveyplated;
	      MBAChBenchSurveyPlate* mbachbenchsurveyplate = new MBAChBenchSurveyPlate;

	      long long endTime;
	      int thisID;

	      // initialize
	      oacfr.next();
	      oacfr.getData(stringData);
	      for ( size_t i = 0; i < types.size(); i++ ) {
		std::istringstream st( stringData[i] );
		if ( names[i] == "MeasDateTime" ) {
		  st >> endTime;
		} else if ( names[i] == "EDMSID" ) {
		  st >> thisID;
		}
	      }

	      do {
		oacfr.getData(stringData);		
		if ( stringData.size() > 0 ) {
		  //std::cout << "stringData.size()=" << stringData.size() << std::endl;
		  if ( types.size() != stringData.size() ) {
		    std::cout << "Error:  line does not have enough data." << std::endl;
		    //std::cout << "types.size()=" << types.size() << "  stringData.size()=" << stringData.size() << std::endl;		  
		  } else {
		    //std::cout << "types.size()=" << types.size() << "  stringData.size()=" << stringData.size() << std::endl;
		    for ( size_t i = 0; i < types.size(); i++ ) {
		      std::istringstream st( stringData[i] );
		      
		      if ( names[i] == "" ) {
		      } else if ( names[i] == "EDMSID" ) {
			st >> mbachbenchsurveyplated.edmsID_;
		      } else if ( names[i] == "SurveyCode" ) {
			st >> mbachbenchsurveyplated.surveyCode_;
		      } else if ( names[i] == "Line" ) {
			st >> mbachbenchsurveyplated.line_;
		      } else if ( names[i] == "Plate" ) {
			st >> mbachbenchsurveyplated.plate_;
		      } else if ( names[i] == "Side" ) {
			st >> mbachbenchsurveyplated.side_;
		      } else if ( names[i] == "Object" ) {
			st >> mbachbenchsurveyplated.object_;
		      } else if ( names[i] == "X" ) {
			st >> mbachbenchsurveyplated.posX_;
		      } else if ( names[i] == "Y" ) {
			st >> mbachbenchsurveyplated.posY_;
		      } else if ( names[i] == "Z" ) {
			st >> mbachbenchsurveyplated.posZ_;
		      } else if ( names[i] == "MeasDateTime" ) {
			st >> mbachbenchsurveyplated.measDateTime_;
		      } 
		    } // end for loop for each of the data lines.
		    stringData.clear();
		  }
		  if ( endTime < mbachbenchsurveyplated.measDateTime_ ) {
		    endTime = mbachbenchsurveyplated.measDateTime_;
		  }
		  std::cout << "The ID = " << thisID << " with time " << endTime << std::endl;
		  if ( thisID != mbachbenchsurveyplated.edmsID_ ) {
		    //write the thing & save tok & save endTime
		    
		    session->startUpdateTransaction();
		    std::string tok=pw.markWrite<MBAChBenchSurveyPlate>(mbachbenchsurveyplate);
		    session->commit();
		    std::cout << "Wrote " <<  (mbachbenchsurveyplate->mbaChBenchSurveyPlate_).size() << " data to " << theConnectionString;
		    //		    std::cout << " with IOV metadata tag" << theCalibrationName  << std::endl;
		    std::cout << " payload token " << tok << std::endl;

		    payloadTokens.push_back(tok);
		    times.push_back(endTime);
		    endTime = mbachbenchsurveyplated.measDateTime_;
		    thisID = mbachbenchsurveyplated.edmsID_;
		    //clear and re-start mbachbenchsurveyplate
		    //delete mbachbenchsurveyplate;
		    mbachbenchsurveyplate = new MBAChBenchSurveyPlate;
		    mbachbenchsurveyplate->mbaChBenchSurveyPlate_.push_back(mbachbenchsurveyplated);
		  } else {
		    mbachbenchsurveyplate->mbaChBenchSurveyPlate_.push_back(mbachbenchsurveyplated);
		  }
		} else {
		  std::cout << "Skipping blank line" << std::endl;
		} // end if size()>0 else
		stringData.clear();
	      } while ( oacfr.next() ); // end while

	      //risky  can we check somehow???

	      cond::DBWriter iovw(*session, "IOV");
	      cond::IOV* initiov=new cond::IOV;
	      for ( size_t iovInd = 0; iovInd < times.size() ; ++iovInd ) {
		initiov->iov.insert(std::make_pair(times[iovInd],payloadTokens[iovInd]));
	      }
		session->startUpdateTransaction();

	      std::string iovtok = iovw.markWrite<cond::IOV>(initiov);

	      session->commit();
	      session->disconnect();

	      cond::MetaData metadata_svc(theConnectionString, *loader);
	      metadata_svc.connect();
	      metadata_svc.addMapping(theCalibrationName, iovtok);
	      metadata_svc.disconnect();
	      delete session;
	    } // end else of if names.size=types.size
	  } // end if got types
	} // end if got names 
      } // if got  types

      return 0;
    }
  } catch(const cond::Exception& er){
    std::cout<<er.what()<<std::endl;
    exit(-1);
  } catch ( std::exception& se ) {
    std::cout << se.what() << std::endl;
  } catch ( ... ) {
    std::cout << "some other error" << std::endl;
  }


  return 0;
}
