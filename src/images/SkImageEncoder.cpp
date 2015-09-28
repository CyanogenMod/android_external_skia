/*
 * Copyright 2009 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkImageEncoder.h"
#include "SkBitmap.h"
#include "SkStream.h"
#include "SkTemplates.h"

SkImageEncoder::~SkImageEncoder() {}

bool SkImageEncoder::encodeStream(SkWStream* stream, const SkBitmap& bm,
                                  int quality) {
    quality = SkMin32(100, SkMax32(0, quality));
    return this->onEncode(stream, bm, quality);
}

bool SkImageEncoder::encodeFile(const char file[], const SkBitmap& bm,
                                int quality) {
    quality = SkMin32(100, SkMax32(0, quality));
    SkFILEWStream   stream(file);
    return this->onEncode(&stream, bm, quality);
}

SkData* SkImageEncoder::encodeData(const SkBitmap& bm, int quality) {
    SkDynamicMemoryWStream stream;
    quality = SkMin32(100, SkMax32(0, quality));
    if (this->onEncode(&stream, bm, quality)) {
        return stream.copyToData();
    }
    return nullptr;
}

bool SkImageEncoder::EncodeFile(const char file[], const SkBitmap& bm, Type t,
                                int quality) {
    SkAutoTDelete<SkImageEncoder> enc(SkImageEncoder::Create(t));
    return enc.get() && enc.get()->encodeFile(file, bm, quality);
}

bool SkImageEncoder::EncodeStream(SkWStream* stream, const SkBitmap& bm, Type t,
                                  int quality) {
    SkAutoTDelete<SkImageEncoder> enc(SkImageEncoder::Create(t));
    return enc.get() && enc.get()->encodeStream(stream, bm, quality);
}

SkData* SkImageEncoder::EncodeData(const SkBitmap& bm, Type t, int quality) {
    SkAutoTDelete<SkImageEncoder> enc(SkImageEncoder::Create(t));
    return enc.get() ? enc.get()->encodeData(bm, quality) : nullptr;
}

SkData* SkImageEncoder::EncodeData(const SkImageInfo& info, const void* pixels, size_t rowBytes,
                                   Type t, int quality) {
    SkBitmap bm;
    if (!bm.installPixels(info, const_cast<void*>(pixels), rowBytes)) {
        return nullptr;
    }
    SkAutoTDelete<SkImageEncoder> enc(SkImageEncoder::Create(t));
    return enc.get() ? enc.get()->encodeData(bm, quality) : nullptr;
}

SkData* SkImageEncoder::EncodeData(const SkPixmap& pmap, Type t, int quality) {
    return EncodeData(pmap.info(), pmap.addr(), pmap.rowBytes(), t, quality);
}

SkData* SkImageEncoder::ReencodeData(SkData* encoded, Type t) {
    SkAutoTDelete<SkImageEncoder> enc(SkImageEncoder::Create(t));
    return enc.get() ? enc.get()->onReencodeData(encoded) : nullptr;
}

