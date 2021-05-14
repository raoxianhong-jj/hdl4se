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

#ifndef __DLIST_H
#define __DLIST_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

#include "guid.h"

DEFINE_GUID(IID_DLIST, 0xcdad8509, 0xb8, 0x4c3e, 0xa3, 0xcc, 0xbe, 0x7c, 0xf0, 0xc, 0x20, 0x8e);

struct sIDList;
struct sIDListVar;

typedef struct sIDList IDList;
typedef struct sIDListVar IDListVar, *IDListVarPtr;

struct sIDList {
  OBJECT_INTERFACE
};

#define DLIST_VARDECLARE \
	INTERFACE_DECLARE(IDList) \
	IDListVar* __dlist_pLast; \
	IDListVar* __dlist_pNext;

struct sIDListVar {
	DLIST_VARDECLARE
};

#define DLIST_VARINIT(_objptr, _obj) \
	INTERFACE_INIT(IDList, _objptr, _obj, dlist); \
	_objptr->__dlist_pLast =  \
	_objptr->__dlist_pNext = (IDListVarPtr)&_objptr->INTERFACE_VAR(IDList);

#define DLIST_FUNCIMPL(_obj, _clsid, _localstruct) \
static const IDList _obj##_dlist_interface = { \
	INTERFACE_HEADER(_obj, IDList, _localstruct) \
};

typedef int (*dlist_traversan_func)(IDListVarPtr item, void *param);

/* 
初始化一个表头 
*/
int dlistInit(IDListVarPtr list);

/* 
在表的最后面追加一个项
*/
int dlistAppendItem(IDListVarPtr list, HOBJECT item);

/*
在表的最前面插入一个项
*/
int dlistInsertItem(IDListVarPtr list, HOBJECT item);

/*
将list2表中的项链接到表list中，list2则清空
*/
int dlistCancat(IDListVarPtr list, IDListVarPtr list2);

/*
遍历表list，对表list中的每一项调用func，param作为参数传到func中
*/
int dlistTraversal(IDListVarPtr list, dlist_traversan_func func, void * param);

/*
将item项插入到项before之前
*/
int dlistInsertBefore(HOBJECT before, HOBJECT item);

/*
将item项插入到项after之后
*/    
int dlistInsertAfter(HOBJECT after, HOBJECT item);    

/*
将项item从所在表list中拆下来
*/
int dlistDetach(HOBJECT item);

/*
得到表中项的个数
*/
int dlistItemCount(IDListVarPtr list);

/*
删除表中的每一项（每一项将调用Release）
*/
int dlistRemoveAll(IDListVarPtr list);

#endif    

#ifdef __cplusplus
}
#endif

#endif /* __DLIST_H */
