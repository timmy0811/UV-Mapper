#pragma once

#include "config.h"

void copyToClipboard(const char* text, size_t lenght)
{
    if (!OpenClipboard(nullptr)) {
        return;
    }

    EmptyClipboard();

    HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE, lenght + 1);
    if (!clipbuffer) {
        CloseClipboard();
        return;
    }

    char* buffer = static_cast<char*>(GlobalLock(clipbuffer));
    strcpy_s(buffer, strlen(text) + 1, text);
    GlobalUnlock(clipbuffer);

    SetClipboardData(CF_TEXT, clipbuffer);

    CloseClipboard();
}