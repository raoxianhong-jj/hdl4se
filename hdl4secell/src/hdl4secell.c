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
* hdl4secell.c
  修改记录：
	202105180851: rxh, initial version
*/

#include "object.h"
#define IMPLEMENT_GUID
#include "hdl4secell.h"
#undef IMPLEMENT_GUID

IHDL4SEUnit** hdl4seCreateUnit(IHDL4SEModule** parent, IIDTYPE clsid, char* instanceparam, char* name)
{
	PARAMITEM param[3];
	IHDL4SEUnit** result = NULL;
	param[0].name = PARAMID_HDL4SE_UNIT_INSTANCE_PARAMETERS;
	param[0].pvalue = instanceparam;
	param[1].name = PARAMID_HDL4SE_UNIT_NAME;
	param[1].pvalue = name;
	param[2].name = PARAMID_HDL4SE_UNIT_PARENT;
	param[2].pvalue = parent;
	objectCreateEx(clsid, param, 3, IID_HDL4SEUNIT, &result);
	objectCall2(parent, AddUnit, result, name);
	return result;
}
