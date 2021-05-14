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

#define BUFLEN  2048
typedef struct _sfilestack {
	struct _sfilestack * pNext;
	struct _sfilestack * pLast;

	char * filename;
	FILE * pFile;
	int lineno;
	int linepos;
	int filepos;
	
	char *buf;
	int  ungetch;
	int  bufindex;
	int  buflen;
	int  isbufonly;
}filestack;

static void filestack_init(filestack * stack)
{
	stack->pNext = stack->pLast = stack;
}

static void filestack_destroy(filestack * stack)
{
	filestack * item, *itemtemp;
	item = stack->pNext;
	while (item != stack) {
		itemtemp = item->pNext;
		if (item->filename)
			free(item->filename);
		if (item->buf)
			free(item->buf);
		free(item);
		item = itemtemp;
	}
	stack->pNext = stack->pLast = stack;
}

static filestack* filestack_push_file(filestack * stack, const char * filename)
{
	filestack * pitem;
	pitem = (filestack *)malloc(sizeof(filestack));
	if (pitem == NULL)
		return NULL;
	memset(pitem, 0, sizeof(filestack));
	pitem->filename = _strdup(filename);
	pitem->pNext = stack; 
	pitem->pLast = stack->pLast; 
	pitem->pNext->pLast = pitem; 
	pitem->pLast->pNext = pitem; 
	pitem->buf = (char *)malloc(BUFLEN);
	return pitem;
}

static filestack* filestack_push_string(filestack * stack, const char * string, int lineno, int filepos, int linepos)
{
	filestack * pitem;
	pitem = (filestack *)malloc(sizeof(filestack));
	if (pitem == NULL)
		return NULL;
	memset(pitem, 0, sizeof(filestack));
	pitem->lineno = lineno;
	pitem->linepos = linepos;
	pitem->filepos = filepos;
	pitem->pNext = stack; 
	pitem->pLast = stack->pLast; 
	pitem->pNext->pLast = pitem; 
	pitem->pLast->pNext = pitem; 
	pitem->buf = strdup(string);
	pitem->buflen = strlen(string);
	pitem->isbufonly = 1;
	return pitem;
}

static filestack* filestack_pop(filestack * stack)
{
	filestack * pitem;
	pitem = stack->pLast;
	if (pitem == stack)
		return NULL;
	pitem->pLast->pNext = pitem->pNext;
	pitem->pNext->pLast = pitem->pLast;
	if (pitem->filename)
		free(pitem->filename);
	if (pitem->buf)
		free(pitem->buf);
	free(pitem);
	pitem = stack->pLast;
	if (pitem == stack)
		return NULL;
	return pitem;
}

