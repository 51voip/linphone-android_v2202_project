/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

/******************************************************************

 iLBC Speech Coder ANSI-C Source Code

 WebRtcIlbcfix_EnergyInverse.h

******************************************************************/

#ifndef WEBRTC_MODULES_AUDIO_CODING_CODECS_ILBC_MAIN_SOURCE_ENERGY_INVERSE_H_
#define WEBRTC_MODULES_AUDIO_CODING_CODECS_ILBC_MAIN_SOURCE_ENERGY_INVERSE_H_

#include "defines.h"

/* Inverses the in vector in into Q29 domain */

void WebRtcIlbcfix_EnergyInverse(
    WebRtc_Word16 *energy,     /* (i/o) Energy and inverse
                                                                   energy (in Q29) */
    int noOfEnergies);   /* (i)   The length of the energy
                                   vector */

#endif
