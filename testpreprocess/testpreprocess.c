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

#include "stdlib.h"
#include "stdio.h"
#include "object.h"
#include "preprocess.h"
#include "verilog_preprocess.h"
#include "string.h"

int main(int argc, char* argv[])
{
	char buf[8192];
	int len;
	IPreprocess ** p;
	preprocessVerilogCreate(&p);
	objectCall1(p, AddIncludePath, "c:/tools/");
	objectCall2(p, SetFile, "d:/test.v", 0);
	
	do {
		len = objectCall2(p, GetText, buf, 8191);
		if (len > 0)
			printf(buf);
	} while (len > 0);

	
	objectRelease(p);
	return 0;
}

