/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "utils.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <Windows.h>

char*
buff_addc (char*  buff, char*  buffEnd, int  c)
{
    int  avail = buffEnd - buff;

    if (avail <= 0)  /* already in overflow mode */
        return buff;

    if (avail == 1) {  /* overflowing, the last byte is reserved for zero */
        buff[0] = 0;
        return buff + 1;
    }

    buff[0] = (char) c;  /* add char and terminating zero */
    buff[1] = 0;
    return buff + 1;
}

char*
buff_adds (char*  buff, char*  buffEnd, const char*  s)
{
    int  slen = strlen(s);

    return buff_addb(buff, buffEnd, s, slen);
}

char*
buff_addb (char*  buff, char*  buffEnd, const void*  data, int  len)
{
    int  avail = (buffEnd - buff);

    if (avail <= 0 || len <= 0)  /* already overflowing */
        return buff;

    if (len > avail)
        len = avail;

    memcpy(buff, data, len);

    buff += len;

    /* ensure there is a terminating zero */
    if (buff >= buffEnd) {  /* overflow */
        buff[-1] = 0;
    } else
        buff[0] = 0;

    return buff;
}

char*
buff_add  (char*  buff, char*  buffEnd, const char*  format, ... )
{
    int      avail;

    avail = (buffEnd - buff);

    if (avail > 0) {
        va_list  args;
        int      nn;

        va_start(args, format);
        nn = vsnprintf( buff, avail, format, args);
        va_end(args);

        if (nn < 0) {
            /* some C libraries return -1 in case of overflow,
             * but they will also do that if the format spec is
             * invalid. We assume ADB is not buggy enough to
             * trigger that last case. */
            nn = avail;
        }
        else if (nn > avail) {
            nn = avail;
        }

        buff += nn;

        /* ensure that there is a terminating zero */
        if (buff >= buffEnd)
            buff[-1] = 0;
        else
            buff[0] = 0;
    }
    return buff;
}
//GBK 转成 UTF8

#define CP_GBK (936) 
void GBK_to_UTF8(const char* in, int len, std::string& out)
{
	int wbufferlen = (int)::MultiByteToWideChar(CP_GBK,MB_PRECOMPOSED,in,(int)len,NULL,0);
	wchar_t* pwbuffer = new wchar_t[wbufferlen+4];
	if ( NULL == pwbuffer )
	{
		return;
	}
	wbufferlen = (int)::MultiByteToWideChar(CP_GBK,MB_PRECOMPOSED,in,(int)len,pwbuffer,wbufferlen+2);
	//wchar_t -> UTF8
	int bufferlen = ::WideCharToMultiByte(CP_UTF8,0,pwbuffer,wbufferlen,NULL,0,NULL,NULL);
	char* pBuffer = new char[bufferlen + 4];
	if ( NULL == pBuffer )
	{
		delete[] pwbuffer;
		return;
	}
	int out_len = ::WideCharToMultiByte(CP_UTF8,0,pwbuffer,wbufferlen,pBuffer,bufferlen+2,NULL,NULL);
	pBuffer[bufferlen] = '\0';
	out.assign( pBuffer, out_len );
	delete[] pwbuffer;
	delete[] pBuffer;
	return;
}

//UTF8  转成 GBK
void UTF8_to_GBK(const char* in, int len, std::string& out)
{

	//1) UTF-8 -> whcar_t
	int wbufferlen = (int)::MultiByteToWideChar(CP_UTF8,0,in,(int)len,NULL,0);    //may cause error
	wchar_t* pwbuffer = new wchar_t[wbufferlen+4];
	if ( NULL == pwbuffer )
	{
		return;
	}
	wbufferlen = (int)::MultiByteToWideChar(CP_UTF8,0,in,(int)len,pwbuffer,wbufferlen+2);
	//2) wchar_t -> GBK
	int bufferlen = (int)::WideCharToMultiByte(CP_GBK,0,pwbuffer,wbufferlen,NULL,0,NULL,NULL);
	char* pBuffer = new char[bufferlen + 4];
	if ( NULL == pBuffer )
	{
		delete[] pwbuffer;
		return;
	}
	int out_len = ::WideCharToMultiByte(CP_GBK,0,pwbuffer,wbufferlen,pBuffer,bufferlen+2,NULL,NULL);  
	pBuffer[bufferlen] = '\0';
	out.assign( pBuffer, out_len );
	delete[] pwbuffer;
	delete[] pBuffer;

	return;
}
// Unicode 转换UTF8
void Unicode_to_UTF8(const wchar_t* in, int len, std::string& out)
{
	size_t out_len = len * 3 + 1;
	char* pBuf = new char[out_len];
	if ( NULL == pBuf )
	{
		return;
	}
	char* pResult = pBuf;
	memset(pBuf, 0, out_len);

	out_len = ::WideCharToMultiByte(CP_UTF8, 0, in, len, pBuf, len * 3, NULL, NULL);
	out.assign( pResult, out_len );
	delete [] pResult;
	pResult = NULL;
	return;
}



// utf8 转换Unicode

void UTF8_to_Unicode(const char* in, int len, std::wstring& out)
{
	wchar_t* pBuf = new wchar_t[len + 1];
	if ( NULL == pBuf )
	{
		return;
	}
	size_t out_len = (len + 1) * sizeof(wchar_t);
	memset(pBuf, 0, (len + 1) * sizeof(wchar_t));
	wchar_t* pResult = pBuf;

	out_len = ::MultiByteToWideChar(CP_UTF8, 0, in, len, pBuf, len * sizeof(wchar_t));
	out.assign( pResult, out_len );
	delete [] pResult;
	pResult = NULL;
}