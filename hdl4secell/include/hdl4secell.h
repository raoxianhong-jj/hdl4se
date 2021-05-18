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
* hdl4secell.h
  修改记录：
    202105180851: rxh, initial version
*/

#ifndef __HDL4SECELL_H
#define __HDL4SECELL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

typedef unsigned int HDL4SEUINT32;

#include "guid.h"

DEFINE_GUID(IID_HDL4SEUNIT, 0x57521e7a, 0xfdc5, 0x4682, 0x94, 0xc8, 0x8d, 0x2d, 0x2d, 0xa0, 0x5a, 0xc8);

typedef struct sIHDL4SEUnit {
    OBJECT_INTERFACE
    int (*Connect)(HOBJECT object, int index, HOBJECT from, int fromindex, int width);
    int (*GetValue)(HOBJECT object, int index, int width, HDL4SEUINT32* value);
}IHDL4SEUnit;

#define HDL4SEUNIT_VARDECLARE
#define HDL4SEUNIT_VARINIT(_objptr, _sid)

#define HDL4SEUNIT_FUNCDECLARE(_obj, _clsid, _localstruct) \
    static int _obj##_hdl4se_unit_Connect(HOBJECT object, int index, HOBJECT from, int fromindex, int width); \
    static int _obj##_hdl4se_unit_GetValue(HOBJECT object, int index, int width, HDL4SEUINT32 *value); \
    static const IHDL4SEUnit _obj##_hdl4se_unit_interface = { \
   	INTERFACE_HEADER(_obj, IHDL4SEUnit, _localstruct) \
   	    _obj##_hdl4se_unit_Connect, \
   	    _obj##_hdl4se_unit_GetValue, \
    };

DEFINE_GUID(IID_HDL4SESTATE, 0x136dbccd, 0x4da, 0x40fa, 0x94, 0x29, 0x18, 0x25, 0x68, 0x17, 0xe3, 0x63);

typedef struct sIHDL4SEState {
    OBJECT_INTERFACE
    int (*ClkTick)(HOBJECT object);
    int (*Setup)(HOBJECT object);
}IHDL4SEState;

#define HDL4SESTATE_VARDECLARE
#define HDL4SESTATE_VARINIT(_objptr, _sid)

#define HDL4SESTATE_FUNCDECLARE(_obj, _clsid, _localstruct) \
    static int _obj##_hdl4se_state_ClkTick(HOBJECT object); \
    static int _obj##_hdl4se_state_Setup(HOBJECT object); \
    static const IHDL4SEState _obj##_hdl4se_state_interface = { \
   	INTERFACE_HEADER(_obj, IHDL4SEState, _localstruct) \
   	    _obj##_hdl4se_state_ClkTick, \
   	    _obj##_hdl4se_state_Setup, \
    };

DEFINE_GUID(IID_HDL4SEMODULE, 0x88cf84f9, 0x17ac, 0x4edf, 0xbf, 0x0, 0xc7, 0x32, 0xd5, 0x26, 0x99, 0x2a);
DEFINE_GUID(PARAMID_HDL4SE_UNIT_INSTANCE_PARAMETERS, 0xad12c414, 0x631b, 0x42cb, 0xb9, 0xbb, 0xba, 0xbd, 0x78, 0x21, 0x3f, 0xef);
DEFINE_GUID(PARAMID_HDL4SE_UNIT_NAME, 0x13c48518, 0x82e6, 0x4f71, 0xb7, 0x5b, 0x24, 0x47, 0xf9, 0xee, 0x4f, 0x6d);
DEFINE_GUID(PARAMID_HDL4SE_UNIT_PARENT, 0x71dd0555, 0x1133, 0x4b69, 0xab, 0x6a, 0x33, 0x2b, 0xb5, 0x57, 0x75, 0x2b);

#define PORTTYPE_INPUT  0
#define PORTTYPE_OUTPUT 1
#define PORTTYPE_INOUT  2

typedef struct sIHDL4SEModule {
    OBJECT_INTERFACE
    int (*AddPort)(HOBJECT object, int width, int type, const char* name);
    int (*AddUnit)(HOBJECT object, IHDL4SEUnit** unit, const char* name);
}IHDL4SEModule;

#define HDL4SEMODULE_VARDECLARE
#define HDL4SEMODULE_VARINIT(_objptr, _sid)

#define HDL4SEMODULE_FUNCDECLARE(_obj, _clsid, _localstruct) \
    static int _obj##_hdl4se_module_AddPort(HOBJECT object, int width, int type, const char * name); \
    static int _obj##_hdl4se_module_AddUnit(HOBJECT object, IHDL4SEUnit ** unit, const char * name); \
    static const IHDL4SEModule _obj##_hdl4se_module_interface = { \
   	INTERFACE_HEADER(_obj, IHDL4SEModule, _localstruct) \
 	    _obj##_hdl4se_module_AddPort, \
 	    _obj##_hdl4se_module_AddUnit, \
    };

IHDL4SEUnit** hdl4seCreateUnit(IHDL4SEModule** parent, IIDTYPE clsid, char* instanceparam, char* name);

/*
以下定义请与hdl4cell.v中的定义保持一致 
*/

DEFINE_GUID(CLSID_HDL4SE_MUX2,  0x9B0B3D25, 0x346D, 0x48B9, 0xAB, 0xB9, 0xED, 0x75, 0x59, 0x10, 0x42, 0x5D);
DEFINE_GUID(CLSID_HDL4SE_MUX4,  0x041F3AA1, 0x97CD, 0x4412, 0x9E, 0x8E, 0xD0, 0x4A, 0xDF, 0x29, 0x1A, 0xE2);
DEFINE_GUID(CLSID_HDL4SE_MUX8,  0xDD99B7F6, 0x9ED1, 0x45BB, 0x81, 0x50, 0xED, 0x78, 0xEE, 0xF9, 0x82, 0xCA);
DEFINE_GUID(CLSID_HDL4SE_MUX16, 0x69B4A095, 0x0644, 0x4B9E, 0x9C, 0xF0, 0x29, 0x54, 0x74, 0xD7, 0xC2, 0x43);
DEFINE_GUID(CLSID_HDL4SE_SPLIT2,0x29D9C8D6, 0x810E, 0x41D0, 0xBC, 0xEF, 0xA5, 0xB8, 0x6E, 0xE1, 0xEE, 0x01);
DEFINE_GUID(CLSID_HDL4SE_SLPLT4,0xD5152459, 0x6798, 0x49C8, 0x83, 0x76, 0x21, 0xEB, 0xE8, 0xA9, 0xEE, 0x3C);
DEFINE_GUID(CLSID_HDL4SE_BIND2, 0xDA8C1494, 0xB6F6, 0x4910, 0xBB, 0x2B, 0xC9, 0xBC, 0xFC, 0xB9, 0xFA, 0xD0);
DEFINE_GUID(CLSID_HDL4SE_BIND3, 0xD1F303E2, 0x3ED1, 0x42FD, 0x87, 0x62, 0x3A, 0xA6, 0x23, 0xDA, 0x90, 0x1E);
DEFINE_GUID(CLSID_HDL4SE_BIND4, 0x0234ECE7, 0xA9C5, 0x406B, 0x9A, 0xE7, 0x48, 0x41, 0xEA, 0x0D, 0xF7, 0xC9);
DEFINE_GUID(CLSID_HDL4SE_CONST, 0x8FBE5B87, 0xB484, 0x4f95, 0x82, 0x91, 0xDB, 0xEF, 0x86, 0xA1, 0xC3, 0x54);

#define BINOP_ADD 0
#define BINOP_SUB 1
#define BINOP_MUL 2
#define BINOP_DIV 3
#define BINOP_EQ 4
#define BINOP_NEQ 5
#define BINOP_LT 6
#define BINOP_LE 7
#define BINOP_GE 8
#define BINOP_GT 9
#define BINOP_AND 10
#define BINOP_OR 11
#define BINOP_XOR 12
DEFINE_GUID(CLSID_HDL4SE_BINOP, 0x060FB913, 0x1C0F, 0x4704, 0x8E, 0xC2, 0xA0, 0x8B, 0xF5, 0x38, 0x70, 0x62);

#define UNOP_NEG 0
#define UNOP_BITAND 1
#define UNOP_BITOR 2
#define UNOP_BITXOR 3
DEFINE_GUID(CLSID_HDL4SE_UNOP,  0xE6772805, 0x57BB, 0x4b39, 0xA1, 0x0D, 0xFD, 0xA6, 0xA4, 0x81, 0x0E, 0x3B);
DEFINE_GUID(CLSID_HDL4SE_REG,   0x76FBFD4B, 0xFEAD, 0x45fd, 0xAA, 0x27, 0xAF, 0xC5, 0x8A, 0xC2, 0x41, 0xC2);

/* 这个在hdl4se.v文件中没有，wire可以作为基本单元在门级网表中出现 */
DEFINE_GUID(CLSID_HDL4SE_WIRE,  0x7acb063f, 0x081c, 0x45c6, 0xaa, 0x1d, 0xdc, 0x43, 0xb1, 0x78, 0x14, 0x63);
DEFINE_GUID(CLSID_HDL4SE_MODULE,0x167d115e, 0x0c1e, 0x484e, 0x8b, 0xf3, 0xa7, 0x38, 0x5b, 0x3d, 0x3a, 0x57);


#endif /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __HDL4SECELL_H */