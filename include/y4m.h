// Copyright 2019 Joe Drago. All rights reserved.
// SPDX-License-Identifier: BSD-2-Clause
#pragma once

#include "avif/avif.h"

#ifdef __cplusplus
extern "C" {
#endif

avifBool y4mRead(avifImage * avif, const char * inputFilename);
avifBool y4mWrite(avifImage * avif, const char * outputFilename);

#ifdef __cplusplus
} // extern "C"
#endif
