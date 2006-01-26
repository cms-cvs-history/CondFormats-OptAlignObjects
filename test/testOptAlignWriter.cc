#include "CondCore/DBCommon/interface/DBWriter.h"
#include "CondFormats/OptAlignObjects/interface/OpticalAlignments.h"
#include "CondCore/IOVService/interface/IOV.h"
#include "SealKernel/Service.h"
#include "POOLCore/POOLContext.h"
#include "SealKernel/Context.h"
#include "CondCore/MetaDataService/interface/MetaData.h"

#include <string>
#include <iostream>
int main(){
  pool::POOLContext::loadComponent( "SEAL/Services/MessageService" );
  pool::POOLContext::setMessageVerbosityLevel( seal::Msg::Info );
  cond::DBWriter w(std::string("sqlite_file:test.db"));
  w.startTransaction();
  
  OpticalAlignments* oa(new OpticalAlignments);
  OpticalAlignInfo oainfo;
  //  OpticalAlignParam x, y, z, xAng, yAng, zAng;
  OpticalAlignParam lValue, aValue;

  lValue.value_ = 1.0123;
  lValue.error_ = 0.0123;
  lValue.iState_ = 'u';

  aValue.value_ = 1.0123;
  aValue.error_ = 0.00123;
  aValue.iState_ = 'u';

//   x.value_ = 1.0123;
//   y.value_ = 1.0123;
//   z.value_ = 1.0123;
//   x.error_ = 0.0123;
//   y.error_ = 0.0123;
//   z.error_ = 0.0123;
//   x.iState_ = 'u';
//   y.iState_ = 'u';
//   z.iState_ = 'u';
//   xAng.value_ = 1.0123;
//   yAng.value_ = 1.0123;
//   zAng.value_ = 1.0123;
//   xAng.error_ = 0.00123;
//   yAng.error_ = 0.00123;
//   zAng.error_ = 0.00123;
//   xAng.iState_='u';
//   yAng.iState_='u';
//   zAng.iState_='u';

  oainfo.x_ = lValue;
  oainfo.y_ = lValue;
  oainfo.z_ = lValue;
  oainfo.angx_ = aValue;
  oainfo.angy_ = aValue;
  oainfo.angz_ = aValue;
  oainfo.objectType_ = "plate";
  oainfo.objectID_ = 1;

  std::cout << "done filling oainfo" << std::endl;
  oa->opticalAlignments_.push_back(oainfo);
  oa->opticalAlignments_.push_back(oainfo);
  oa->opticalAlignments_.push_back(oainfo);
  std::cout << "done with push_back of oainfo to oa" << std::endl;
  std::string tok1=w.write<OpticalAlignments>(oa,"OpticalAlignments");
  std::cout << "done writing" << std::endl;

  OpticalAlignments* oatoo=new OpticalAlignments;
  oatoo->opticalAlignments_.push_back(oainfo);
  std::string tok2=w.write<OpticalAlignments>(oatoo,"OpticalAlignments");
  cond::IOV* initiov=new cond::IOV;
  initiov->iov.insert(std::make_pair(1,tok1));
  initiov->iov.insert(std::make_pair(2,tok2));
  std::string iovtok1=w.write<cond::IOV>(initiov, "IOV");

  w.commitTransaction();

  w.startTransaction();
  cond::IOV* bestiov=new cond::IOV;
  bestiov->iov.insert(std::make_pair(10,tok1));
  bestiov->iov.insert(std::make_pair(20,tok2));
  std::string iovtok2=w.write<cond::IOV>(bestiov, "IOV");
  w.commitTransaction();

  cond::MetaData metadata_svc("sqlite_file:test.db");
  metadata_svc.addMapping("OpticalAlignmentsTry3", iovtok1);
  metadata_svc.addMapping("OpticalAlignmentsTry4", iovtok2);
}
