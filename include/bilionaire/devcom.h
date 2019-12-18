// Copyright (C) Horizon Robotics. 2019. All Rights Reserved.

#ifndef bilionaire_INCLUDE_bilionaire_DEVCOM_H_
#define bilionaire_INCLUDE_bilionaire_DEVCOM_H_
#include <string>

#define BUF_LEN (1 * 1024 * 1024)
#define FRAME_LEN (BUF_LEN - 16)

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC int InitSmart();
EXTERNC int DeinitSmart();
EXTERNC int InitVideo(char *filename);
EXTERNC int DeinitVideo();
EXTERNC int ReadVideoRtpPkg(char *output, int *len);
EXTERNC int ReadSmartFrame(char *output, int *len);

#undef EXTERNC
#endif //bilionaire_INCLUDE_bilionaire_DEVCOM_H_
