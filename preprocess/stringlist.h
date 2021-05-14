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

typedef struct s_stringitem {
	struct s_stringitem * pNext;
	struct s_stringitem * pLast;
	char * string;
}stringitem;

#define ITEM_DESTROY(pitem) \
do { \
	if ((pitem)->string != NULL) \
		free((pitem)->string); \
	free(pitem); \
} while (0)

#define ITEM_INIT(pitem) \
do { \
	(pitem)->string = NULL; \
}while (0)

#define DLIST_INIT(list) \
do { \
	(list)->pNext = list; \
	(list)->pLast = list; \
} while (0)

#define DLIST_ADDITEM(list, pitem) \
do { \
	pitem->pNext = list; \
	pitem->pLast = (list)->pLast; \
	pitem->pNext->pLast = pitem; \
	pitem->pLast->pNext = pitem; \
}while (0)

#define DLIST_DESTROY(ITEM, list) \
do { \
	ITEM *pitem, *pnextitem; \
	pitem = (list)->pNext; \
	while (pitem != (list)) { \
		pnextitem = pitem->pNext; \
		ITEM_DESTROY(pitem); \
		pitem = pnextitem; \
	} \
	(list)->pNext = (list)->pLast = list; \
}while (0)

#define DLIST_EVERY_ITEM_BEGIN(ITEM, list, pitem) \
{ \
	ITEM * pitem, *pnextitem; \
	pitem = (list)->pNext; \
	while (pitem != (list)) { \
		pnextitem = pitem->pNext; 

#define DLIST_EVERY_ITEM_END() \
	    pitem = pnextitem; \
	} \
}

static int stringlistlen(stringitem * list)
{
	int len = 0;
	DLIST_EVERY_ITEM_BEGIN(stringitem, list, pitem) {
		len += strlen(pitem->string) + 1; /* 每两个string之间加个; */
	} DLIST_EVERY_ITEM_END();
	len++; /* 最后加个0字符位置 */
	return len;
}

static int getstringlist(stringitem * list, int gap, char * buf, int buflen)
{
	char *tempbuf;
	int slen;
	int len = stringlistlen(list);
	if ( (buflen < len) || (buf == NULL))
		return len;
	tempbuf = buf;
	DLIST_EVERY_ITEM_BEGIN(stringitem, list, pitem) {
		slen = strlen(pitem->string);
		memcpy(tempbuf, pitem->string, slen);
		tempbuf[slen] = gap;
		tempbuf += slen + 1;
	} DLIST_EVERY_ITEM_END();
	tempbuf[0] = 0;
	return len;
}

static int stringlistaddstring(stringitem * list, const char * string)
{
	stringitem * pItem;
	pItem = (stringitem *)malloc(sizeof(stringitem));
	if (pItem == NULL)
		return -1;
	pItem->string = STRDUP(string);
	DLIST_ADDITEM(list, pItem);
	return 0;
}

static int stringlistaddstringfirst(stringitem * list, const char * string)
{
	stringitem * pItem;
	pItem = (stringitem *)malloc(sizeof(stringitem));
	if (pItem == NULL)
		return -1;
	pItem->string = STRDUP(string);
	pItem->pLast = list; \
	pItem->pNext = (list)->pNext; \
	pItem->pNext->pLast = pItem; \
	pItem->pLast->pNext = pItem; \
	return 0;
}

static int stringlistcount(stringitem * list)
{
	int count;
	count = 0;
	DLIST_EVERY_ITEM_BEGIN(stringitem, list, pitem) {
		count++;
	} DLIST_EVERY_ITEM_END();
	return count;
}

static stringitem * stringlistfindstring(stringitem * list, const char * string, int *itemindex)
{
	int len, index;
	len = strlen(string);
	index = 0;
	DLIST_EVERY_ITEM_BEGIN(stringitem, list, pitem) {
		if (memcmp(pitem->string, string, len)== 0) {
			if (itemindex != NULL)
				*itemindex = index;
			return pitem;
		}
		index++;
	} DLIST_EVERY_ITEM_END();
	return NULL;
}

static stringitem * stringlistfindkey(stringitem * list, const char * key)
{
	stringitem * pitem;
	char * buf;
	buf = (char *)malloc(strlen(key)+2);
	strcpy(buf, key);
	strcat(buf, "=");
	pitem = stringlistfindstring(list, buf, NULL);
	free(buf);
	return pitem;
}

static int stringlistaddstringpair(stringitem * list, const char * key, const char * string)
{
	stringitem * pItem;
	int len;
	char * buf;
	len = strlen(key) + 1 + strlen(string) + 1;
	buf = malloc(len);
	if (buf == NULL)
		return -1;
	strcpy(buf, key);
	strcat(buf, "=");
	pItem = stringlistfindstring(list, buf, NULL);
	if (pItem != NULL) {
		free(pItem->string);
	} else {
		pItem = (stringitem *)malloc(sizeof(stringitem));
		if (pItem == NULL)
			return -1;
		DLIST_ADDITEM(list, pItem);
	}
	pItem->string = buf;
	strcat(pItem->string, string);
	return 0;
}

static int stringlistremovestring(stringitem* list, const char * string)
{
	int len;
	len = strlen(string);
	DLIST_EVERY_ITEM_BEGIN(stringitem, list, pitem) {
		if (memcmp(pitem->string, string, len)== 0) {
			pitem->pLast->pNext = pitem->pNext;
			pitem->pNext->pLast = pitem->pLast;
			ITEM_DESTROY(pitem);
			return 0;
		}
	} DLIST_EVERY_ITEM_END();
	return -1;
}

static int stringlistremovekey(stringitem* list, const char * key)
{
	int len;
	char *buf;
	buf = malloc(strlen(key) + 2);
	strcpy(buf, key);
	strcat(buf, "=");
	len = strlen(buf);
	DLIST_EVERY_ITEM_BEGIN(stringitem, list, pitem) {
		if (memcmp(pitem->string, buf, len)== 0) {
			pitem->pLast->pNext = pitem->pNext;
			pitem->pNext->pLast = pitem->pLast;
			ITEM_DESTROY(pitem);
			free(buf);
			return 0;
		}
	} DLIST_EVERY_ITEM_END();
	free(buf);
	return -1;
}

