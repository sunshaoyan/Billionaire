// Copyright (C) Horizon Robotics. 2019. All Rights Reserved.
// ██╗  ██╗ █████╗  ██████╗██╗  ██╗ █████╗ ████████╗██╗  ██╗ ██████╗ ███╗   ██╗    ██████╗  ██████╗  ██╗ █████╗ 
// ██║  ██║██╔══██╗██╔════╝██║ ██╔╝██╔══██╗╚══██╔══╝██║  ██║██╔═══██╗████╗  ██║    ╚════██╗██╔═████╗███║██╔══██╗
// ███████║███████║██║     █████╔╝ ███████║   ██║   ███████║██║   ██║██╔██╗ ██║     █████╔╝██║██╔██║╚██║╚██████║
// ██╔══██║██╔══██║██║     ██╔═██╗ ██╔══██║   ██║   ██╔══██║██║   ██║██║╚██╗██║    ██╔═══╝ ████╔╝██║ ██║ ╚═══██║
// ██║  ██║██║  ██║╚██████╗██║  ██╗██║  ██║   ██║   ██║  ██║╚██████╔╝██║ ╚████║    ███████╗╚██████╔╝ ██║ █████╔╝
// ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═══╝    ╚══════╝ ╚═════╝  ╚═╝ ╚════╝ 
// ██████╗ ██╗██╗     ██╗ ██████╗ ███╗   ██╗ █████╗ ██╗██████╗ ███████╗                                         
// ██╔══██╗██║██║     ██║██╔═══██╗████╗  ██║██╔══██╗██║██╔══██╗██╔════╝                                         
// ██████╔╝██║██║     ██║██║   ██║██╔██╗ ██║███████║██║██████╔╝█████╗                                           
// ██╔══██╗██║██║     ██║██║   ██║██║╚██╗██║██╔══██║██║██╔══██╗██╔══╝                                           
// ██████╔╝██║███████╗██║╚██████╔╝██║ ╚████║██║  ██║██║██║  ██║███████╗                                         
// ╚═════╝ ╚═╝╚══════╝╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚═╝  ╚═╝╚═╝╚═╝  ╚═╝╚══════╝      
// Tingcheng Wu , 2019-12-16

#include "bilionaire/devcom.h"
#include "bilionaire/hbipc_ap.h"
#include "bilionaire/hbipc_errno.h"
#include <sys/stat.h>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static bool videoinit_ = false;
static bool smartinit_ = false;

static int domain_id_ = -1;
static int provider_id_ = -1;
static struct session session_connect_;

int initSmartFrame();
int deinitSmartFrame();

int InitSmart(){
  if(smartinit_){
    Log("Already initialized!\n");
    return -101; 
  }
  int ret = 0;
  smartinit_ = false;
  ret = initSmartFrame();
  if(ret){
    Log("Init smart frame error: %d\n", ret);
    return ret;
  }
  smartinit_ = true;
  Log("Smart initialize finish: success!\n");
  return 0;
}

int DeinitSmart(){
  if(!smartinit_){
    Log("Deinitialize but uninited!\n");
    return -31;
  }
  int ret = deinitSmartFrame();
  if(ret<0){
    return ret;
  }
  smartinit_ = false;
  return 0;
}

int ReadSmartFrame(char *output, int *len) {
  if(!smartinit_){
    Log("read rtp package error: uninitialized!\n");
    return -41;
  }
  char buf[BUF_LEN];
  int ret = hbipc_ap_recvframe(&session_connect_, buf, sizeof(buf));
  if(ret<0){
    Log("[hbipc_ap_recvframe] read dev info error: %d\n",ret);
    return ret;
  }
  *len = ret;
  memcpy(output, buf, ret);
  return 0;
}

int initSmartFrame() {
  int ret = -1;
  char domain_name[] = "X2BIF001";

  struct domain_configuration domain_config= {
      2,
      {
        {"X2BIF001", 0, "/dev/x2_bif"},
        {"X2SD001", 1, "/dev/x2_sd"},
        {},
      },
  };
  char echo_buf_1[BUF_LEN];

  UUID_DEFINE(server_id, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xe);
  // UUID_DEFINE(server_id, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf);

  if ((ret = hbipc_ap_init(domain_name, TF_BLOCK, 0, &domain_config)) < 0) {
    Log("1:hbipc_ap_init_ error: %d\n", ret);
    return -21;
  }

  domain_id_ = ret;
  if ((ret = hbipc_ap_startserver(domain_id_, (unsigned char * )server_id, NULL, TF_BLOCK)) < 0) {
    Log("1:hbipc_ap_startserver error: %d\n", ret);
    return -22;
  }

  provider_id_ = ret;
  if (ret = hbipc_ap_connect(domain_id_, provider_id_, &session_connect_) < 0) {
    Log("1:hbipc_ap_connect error: %d\n", ret);
    return -23;
  }
  return 0;
}

int deinitSmartFrame(){
  int ret = 0;
  if ((ret = hbipc_ap_disconnect(&session_connect_)) < 0) {
                Log("hbipc_ap_disconnect error: %d", ret);
        }
        if ((ret = hbipc_ap_stopserver(domain_id_, provider_id_)) < 0) {
                Log("hbipc_ap_stopserver error: %d", ret);
        }
        if ((ret = hbipc_ap_deinit(domain_id_)) < 0) {
                Log("hbipc_ap_deinit error: %d", ret);
  }
  return 0;
}

