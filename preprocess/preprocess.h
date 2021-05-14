/*
** HDL4SE: 软件Verilog综合仿真平台
** Copyright (C) 2021-2021, raoxianhong<raoxianhong@163.net>
** LCOM: 轻量级组件对象模型
** Copyright (C) 2021-2021, raoxianhong<raoxianhong@163.net>
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without 
** modification, are permitted provided that the following conditions are met:
** 
** * Redistributions of source code must retain the above copyright notice, 
**   this list of conditions and the following disclaimer.
** * Redistributions in binary form must reproduce the above copyright notice, 
**   this list of conditions and the following disclaimer in the documentation 
**   and/or other materials provided with the distribution.
** * The name of the author may be used to endorse or promote products 
**   derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
** THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
  修改记录：
    202105140816: 根据git的要求增加License
*/

#ifndef __PREPROCESS_H
#define __PREPROCESS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

#include "guid.h"

DEFINE_GUID(IID_PREPROCESS, 0x38f9f002, 0xa71f, 0x4b40, 0xae, 0x8a, 0xfd, 0xc9, 0xb6, 0xdf, 0x6c, 0xfc);

enum PreActionOP {
	PA_DEFINE,
	PA_UNDEF,
	PA_IFDEF,
	PA_IFNDEF,
	PA_IF,
	PA_ELSEIFDEF,
	PA_ELSEIF,
	PA_ELSE,
	PA_ENDIF,
	PA_INCLUDE,
	PA_MACRO,
	PA_NETTYPE,
};

typedef struct sIPreprocess {
  OBJECT_INTERFACE
  int (*SymbolEmitEnabled)(HOBJECT object);
  int (*SetFile)(HOBJECT object, const char * filename, int reset);
  int (*AddIncludePath)(HOBJECT object, const char * filename);
  int (*GetCurrentFile)(HOBJECT object, char *filename, int buflen, int *lineno, int *linepos, int *filepos);
  int (*SetParam)(HOBJECT object, const char * paramname, char *paramvalue);
  int (*GetParam)(HOBJECT object, const char * param, char * value, int buflen);
  int (*GetText)(HOBJECT object, char * buf, int maxsize);
  int (*GetLog)(HOBJECT object, char * value, int vlen);
  int (*PreAction)(HOBJECT object, int action, const char * macro, const char * value);
 }IPreprocess;

#define PREPROCESS_VARDECLARE
#define PREPROCESS_VARINIT(_objptr, _sid)

#define PREPROCESS_FUNCDECLARE(_obj, _clsid, _localstruct) \
  static int _obj##_preprocess_SymbolEmitEnabled(HOBJECT object); \
  static int _obj##_preprocess_SetFile(HOBJECT object, const char * filename, int reset); \
  static int _obj##_preprocess_AddIncludePath(HOBJECT object, const char * filename); \
  static int _obj##_preprocess_GetCurrentFile(HOBJECT object, char *filename, int buflen, int *lineno, int *linepos, int *filepos); \
  static int _obj##_preprocess_SetParam(HOBJECT object, const char * paramname, const char *paramvalue); \
  static int _obj##_preprocess_GetParam(HOBJECT object, const char * param, char * value, int buflen); \
  static int _obj##_preprocess_GetText(HOBJECT object, char * buf, int maxsize); \
  static int _obj##_preprocess_GetLog(HOBJECT object, char * value, int vlen); \
  static int _obj##_preprocess_PreAction(HOBJECT object, int action, const char * macro, const char * value); \
  static const IPreprocess _obj##_preprocess_interface = { \
		INTERFACE_HEADER(_obj, IPreprocess, _localstruct) \
			 _obj##_preprocess_SymbolEmitEnabled, \
             _obj##_preprocess_SetFile, \
             _obj##_preprocess_AddIncludePath, \
             _obj##_preprocess_GetCurrentFile, \
             _obj##_preprocess_SetParam, \
             _obj##_preprocess_GetParam, \
             _obj##_preprocess_GetText, \
             _obj##_preprocess_GetLog, \
			 _obj##_preprocess_PreAction, \
};
    
#endif    

#ifdef __cplusplus
}
#endif

#endif 
