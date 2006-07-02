#include "CondCore/DBCommon/interface/DBWriter.h"
#include "CondFormats/OptAlignObjects/interface/OpticalAlignments.h"
#include "CondFormats/OptAlignObjects/interface/OAQuality.h"
#include "CondCore/IOVService/interface/IOV.h"
#include "SealKernel/Service.h"
#include "POOLCore/POOLContext.h"
#include "SealKernel/Context.h"
#include "CondCore/MetaDataService/interface/MetaData.h"

#include "CondCore/DBCommon/interface/DBSession.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/DBCommon/interface/ServiceLoader.h"
#include "CondCore/DBCommon/interface/ConnectMode.h"
#include "CondCore/DBCommon/interface/MessageLevel.h"

#include <string>
#include <iostream>
int main(){

  OpticalAlignments* oa(new OpticalAlignments);
  OpticalAlignInfo oainfo;
  OpticalAlignParam lValue, aValue;

  lValue.value_ = 1.0123;
  lValue.error_ = 0.0123;
  lValue.quality_ = oa_unknown;

  aValue.value_ = 1.0123;
  aValue.error_ = 0.00123;
  aValue.quality_ = oa_unknown;

  oainfo.x_ = lValue;
  oainfo.y_ = lValue;
  oainfo.z_ = lValue;
  oainfo.angx_ = aValue;
  oainfo.angy_ = aValue;
  oainfo.angz_ = aValue;
  oainfo.type_ = "plate";
  oainfo.ID_ = 1;

  std::cout << "done filling oainfo" << std::endl;
  oa->opticalAlignments_.push_back(oainfo);
  oa->opticalAlignments_.push_back(oainfo);
  oa->opticalAlignments_.push_back(oainfo);
  std::cout << "done with push_back of oainfo to oa" << std::endl;

  pool::POOLContext::loadComponent( "SEAL/Services/MessageService" );
  pool::POOLContext::setMessageVerbosityLevel( seal::Msg::Info );
  cond::ServiceLoader* loader=new cond::ServiceLoader;
  ::putenv("CORAL_AUTH_USER=me");
  ::putenv("CORAL_AUTH_PASSWORD=mypass");
  loader->loadAuthenticationService( cond::Env );
  loader->loadMessageService( cond::Error );
  cond::DBSession* session=new cond::DBSession("sqlite_file:test.db");
  session->setCatalog("file:PoolFileCatalog.xml");
  session->connect(cond::ReadWriteCreate);

  cond::DBWriter pw(*session, "OpticalAlignments");
  cond::DBWriter iovw(*session, "IOV");
  cond::IOV* initiov=new cond::IOV;
  session->startUpdateTransaction();
  //	      std::string tok=pw.markWrite<CSCZSensors>(csczs);
  std::string tok1 = pw.markWrite<OpticalAlignments>(oa);

  lValue.value_ = 1.0234;
  lValue.error_ = 0.0234;
  lValue.quality_ = oa_unknown;

  aValue.value_ = 1.0234;
  aValue.error_ = 0.00234;
  aValue.quality_ = oa_unknown;

  oainfo.x_ = lValue;
  oainfo.y_ = lValue;
  oainfo.z_ = lValue;
  oainfo.angx_ = aValue;
  oainfo.angy_ = aValue;
  oainfo.angz_ = aValue;
  oainfo.type_ = "plate";
  oainfo.ID_ = 1;

  OpticalAlignments* oatoo=new OpticalAlignments;
  oatoo->opticalAlignments_.push_back(oainfo);
  oatoo->opticalAlignments_.push_back(oainfo);
  oatoo->opticalAlignments_.push_back(oainfo);
  std::string tok2=pw.markWrite<OpticalAlignments>(oatoo);

  initiov->iov.insert(std::make_pair(1,tok1));
  initiov->iov.insert(std::make_pair(3,tok2));
  std::string iovtok = iovw.markWrite<cond::IOV>(initiov);
  session->commit();
  session->disconnect();
  cond::MetaData metadata_svc("sqlite_file:test.db", *loader);
  metadata_svc.connect();
  metadata_svc.addMapping("OpticalAlignmentsTry3", iovtok);
  metadata_svc.disconnect();
  std::cout << "Wrote OpticalAlignments to sqlite_file:test.db";
  std::cout << " with token " << iovtok << " as OpticalAlignmentsTry3 "  << std::endl;
  delete session;

}
