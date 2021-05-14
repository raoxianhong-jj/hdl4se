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
#include "../include/object.h"
#define IMPLEMENT_GUID
#include "../include/dlist.h"
#undef IMPLEMENT_GUID

/* 
初始化一个表头 
*/
int dlistInit(IDListVarPtr plist)
{
	if (plist==NULL)
		return -1;
	plist->__dlist_pNext = plist;
	plist->__dlist_pLast = plist;
	return 0;
}

/* 
在表的最后面追加一个项
*/
int dlistAppendItem(IDListVarPtr plist, HOBJECT item)
{
	IDListVarPtr pitem = NULL;
	if (plist == NULL)
		return -1;
	objectQueryInterface(item, IID_DLIST, &pitem);
	if (pitem == NULL) {
		return -1;
	}
	pitem->__dlist_pNext = plist;
	pitem->__dlist_pLast = plist->__dlist_pLast;
	pitem->__dlist_pNext->__dlist_pLast = pitem;
	pitem->__dlist_pLast->__dlist_pNext = pitem;
	objectRelease(pitem);
	return 0;
}

/* 
在表的最前面追加一个项 
*/
int dlistInsertItem(IDListVarPtr plist, HOBJECT item)
{
	IDListVarPtr pitem = NULL;
	if (plist == NULL)
		return -1;
	objectQueryInterface(item, IID_DLIST, &pitem);
	if (pitem == NULL) {
		return -1;
	}
	pitem->__dlist_pLast = plist;
	pitem->__dlist_pNext = plist->__dlist_pNext;
	pitem->__dlist_pLast->__dlist_pNext = pitem;
	pitem->__dlist_pNext->__dlist_pLast = pitem;
	objectRelease(pitem);
	return 0;
}
/* 
将list2表中的项链接到表list中，list2则清空 
*/
int dlistCancat(IDListVarPtr plist, IDListVarPtr plist2)
{
	if ( (plist == NULL) || (plist2 == NULL) ) {
		return -1;
	}
	if (plist2->__dlist_pNext != plist2) {
		plist ->__dlist_pLast->__dlist_pNext = plist2->__dlist_pNext;
		plist2->__dlist_pNext->__dlist_pLast = plist ->__dlist_pLast;
		plist2->__dlist_pLast->__dlist_pNext = plist;
		plist ->__dlist_pLast = plist2->__dlist_pLast;
		plist2->__dlist_pNext = plist2;
		plist2->__dlist_pLast = plist2;
	}
	return 0;
}

/* 
遍历表list，对表list中的每一项调用func，param作为参数传到func中 
*/
int dlistTraversal(IDListVarPtr plist, dlist_traversan_func func, void * param)
{
	IDListVarPtr pitem, pitemtemp;
	if (plist == NULL)
		return -1;
	pitem = plist->__dlist_pNext; 
	while (pitem != plist) {
		pitemtemp = pitem->__dlist_pNext;
		if (func(pitem, param) != 0)
			break;
		pitem = pitemtemp;
	}
	return 0;
}

/* 
将item项插入到项before之前 
*/
int dlistInsertBefore(HOBJECT before, HOBJECT item)
{
	IDListVarPtr pitem = NULL;
	IDListVarPtr pbefore = NULL;
	objectQueryInterface(item, IID_DLIST, &pitem);
	objectQueryInterface(before, IID_DLIST, &pbefore);
	if ( (pitem == NULL) || (pbefore==NULL) ) {
		objectRelease(item);
		objectRelease(before);
		return -1;
	}
	pitem->__dlist_pNext = pbefore;
	pitem->__dlist_pLast = pbefore->__dlist_pLast;
	pitem->__dlist_pLast->__dlist_pNext = pitem;
	pitem->__dlist_pNext->__dlist_pLast = pitem;
	objectRelease(item);
	objectRelease(before);
	return 0;
}

/* 
将item项插入到项after之后 
*/    
int dlistInsertAfter(HOBJECT after, HOBJECT item)    
{
	IDListVarPtr pitem = NULL;
	IDListVarPtr pafter = NULL;
	objectQueryInterface(item, IID_DLIST, &pitem);
	objectQueryInterface(after, IID_DLIST, &pafter);
	if ( (pitem == NULL) || (pafter==NULL) ) {
		objectRelease(item);
		objectRelease(after);
		return -1;
	}
	pitem->__dlist_pLast = pafter;
	pitem->__dlist_pNext = pafter->__dlist_pNext;
	pitem->__dlist_pLast->__dlist_pNext = pitem;
	pitem->__dlist_pNext->__dlist_pLast = pitem;
	objectRelease(item);
	objectRelease(after);
	return 0;
}

/* 
将项item从所在表list中拆下来 
*/
int dlistDetach(HOBJECT item)
{
	IDListVarPtr pitem = NULL;
	objectQueryInterface(item, IID_DLIST, &pitem);
	if (pitem == NULL) {
		return -1;
	}
	pitem->__dlist_pLast->__dlist_pNext = pitem->__dlist_pNext;
	pitem->__dlist_pNext->__dlist_pLast = pitem->__dlist_pLast;
	pitem->__dlist_pLast = pitem;
	pitem->__dlist_pNext = pitem;
	objectRelease(item);
	return 0;
}

/* 
得到表中项的个数 
*/
int dlistItemCount(IDListVarPtr plist)
{
	int count;
	IDListVarPtr pitem;
	if (plist==NULL)
		return 0;
	count = 0;
	pitem = plist->__dlist_pNext;
	while (pitem != plist) {
		pitem = pitem->__dlist_pNext;
		count++;
	}
	return count;
}

/* 
删除表中的每一项（每一项将调用Release）
*/
int dlistRemoveAll(IDListVarPtr plist)
{
	IDListVarPtr pitem, pnextitem;
	if (plist==NULL)
		return -1;
	pitem = plist->__dlist_pNext; 
	while (pitem != plist) {
		pnextitem = pitem->__dlist_pNext;
		objectRelease((const IDList **)pitem);
		pitem = pnextitem;
	}
	plist->__dlist_pNext = plist->__dlist_pLast = plist;
	return 0;
}
