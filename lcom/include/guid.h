/*
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
* 
  修改记录：
    202105140816: 根据git的要求增加License
*/

/*
	我们使用GUID作为类，接口和参数的唯一标识，由工具保证标识不重复
	一个GUID由16字节组成，可以用guidgen工具来生成，GUIID在头文件中
	用定义成一个extern const Ouint32_t name[4];
	在uuid.c中先定义IMPLEMENT_GUID，再包含所有的头文件，这样就等于实际
	实现了这个guid
*/

#undef DEFINE_GUID
#ifdef IMPLEMENT_GUID
#if _BYTE_ORDER == _BIG_ENDIAN
#define DEFINE_GUID(name, L, s1, s2, b1, b2, b3, b4, b5, b6, b7, b8) \
const Ouint32_t name[4] = {L, \
													((s2) << 0) | (s1 << 16),  \
													((b1) << 0) | ((b2) << 8) | ((b3) << 16) | ((b4) << 24), \
													((b5) << 0) | ((b6) << 8) | ((b7) << 16) | ((b8) << 24), \
													};
#else
#define DEFINE_GUID(name, L, s1, s2, b1, b2, b3, b4, b5, b6, b7, b8) \
const Ouint32_t name[4] = {L, \
													((s1) << 0) | (s2 << 16),  \
													((b4) << 0) | ((b3) << 8) | ((b2) << 16) | ((b1) << 24), \
													((b8) << 0) | ((b7) << 8) | ((b6) << 16) | ((b5) << 24), \
												};
#endif /*_BYTE_ORDER == _BIG_ENDIAN*/							
#else

#define DEFINE_GUID(name, L, s1, s2, b1, b2, b3, b4, b5, b6, b7, b8) \
extern const Ouint32_t name[4];

#endif

