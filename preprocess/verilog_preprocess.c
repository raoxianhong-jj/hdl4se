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

#include "stdio.h"
#include "object.h"
#include "preprocess.h"
#include "string.h"
#include "stdlib.h"

#define PRINTF printf

#define IMPLEMENT_GUID
#include "verilog_preprocess.h"
#undef IMPLEMENT_GUID

#include "stringlist.h"
#include "filestack.h"

#define MAXIFSTACKDEPTH (32 * 32)

enum CDNameID {
	CDN_CELLDEFINE,
	CDN_ENDCELLDEFINE,
	CDN_DEFAULT_NETTYPE,
	CDN_DEFINE,
	CDN_UNDEF,
	CDN_IFDEF,
	CDN_ELSE,
	CDN_ELSIF,
	CDN_ENDIF,
	CDN_IFNDEF,
	CDN_INCLUDE,
	CDN_RESETALL,
	CDN_LINE,
	CDN_TIMESCALE,
	CDN_UNCONNECTED_DRIVE,
	CDN_NOUNCONNECTED_DRIVE,
	CDN_PRAGMA,
	CDN_BEGINKEYWORDS,
	CDN_ENDKEYWORDS,
	CDN_MACROIDENT,
};

static const char * cd_name[] = 
{
	"celldefine",
	"endcelldefine",
	"default_nettype",
	"define",
	"undef",
	"ifdef",
	"else",
	"elsif",
	"endif",
	"ifndef",
	"include",
	"resetall",
	"line",
	"timescale",
	"unconnected_drive",
	"nounconnected_drive",
	"pragma",
	"begin_keywords",
	"end_keywords",
	NULL
};

enum VPState {
	STATE_INITIAL = 0,
	STATE_CD_START,
	STATE_CD_NAME,
	STATE_CD_MACRO_PARAM,
	STATE_CD_MACRO_PARAM_TEXT,
	STATE_CD_DEFAULT_NETTYPE,
	STATE_CD_DEFAULT_NETTYPE_NAME,
	STATE_CD_DEFINE,
	STATE_CD_DEFINE_NAME,
	STATE_CD_DEFINE_PARAMORTEXT,
	STATE_CD_DEFINE_PARAM_NAME,
	STATE_CD_DEFINE_PARAM_NAME_PRO,
	STATE_CD_DEFINE_TEXT,
	STATE_CD_UNDEF,
	STATE_CD_UNDEF_NAME,
	STATE_CD_ELSEIF,
	STATE_CD_ELSEIF_NAME,
	STATE_CD_IFDEF,
	STATE_CD_IFDEF_NAME,
	STATE_CD_IFNDEF,
	STATE_CD_IFNDEF_NAME,
	STATE_CD_LINE,
	STATE_CD_LINE_NO,
	STATE_CD_LINE_FILENAME,
	STATE_CD_LINE_LEVEL,
	STATE_CD_INCLUDE,
	STATE_CD_INCLUDE_NAME,
	STATE_CD_TIMESCALE,
	STATE_CD_TIMESCALE_UNIT,
	STATE_CD_TIMESCALE_UNIT_NAME,
	STATE_CD_TIMESCALE_PRECISION_PRE,
	STATE_CD_TIMESCALE_PRECISION,
	STATE_CD_TIMESCALE_PRECISION_NAME,
	STATE_CD_PRAGMA,
	STATE_CD_BEGINKEYWORDS,
};

enum VPTokenState {
	TOKEN_NONE,
	TOKEN_COMMENT_START,
	TOKEN_IN_COMMENT,
	TOKEN_IN_COMMENTLINE,
	TOKEN_IN_NUMBER,
	TOKEN_IN_SIMIDENT,
	TOKEN_IN_ESCIDENT,
	TOKEN_IN_STRING,
	TOKEN_IN_STRING_SLASH,
	TOKEN_IN_MACRO_TEXT,
	TOKEN_IN_MACRO_TEXT_SLASH,
};

enum TokenType {
	TT_NONE,
	TT_STRING,
	TT_NUMBER,
	TT_SIMIDENT,
	TT_ESCIDENT,
	TT_SYMBOL,
	TT_MACRO_TEXT,
};

typedef struct _sVerilogPreprocess {
	OBJECT_HEADER
	INTERFACE_DECLARE(IPreprocess)
	PREPROCESS_VARDECLARE
	stringitem path_list;
	stringitem macro_list; /* name=[%3d]text */
	stringitem param_list; /* name=text */
	stringitem log_list; 

	filestack  file_stack;
	filestack *file_stack_top;
	filestack *last_file_stack_top;
	
	char *cwd;

	char * token;
	int tokenbuflen;
	int tokenlen;
	enum VPTokenState tokenstate;
	enum TokenType tokentype;

	int lastch;

	enum VPState state;

	/*cd line*/
	int		line_no;
	char *	line_filename;
	int		line_level;

	/*cd define*/
	char * define_name;
	stringitem define_param_list; 
	char * define_text;

	/* macro */
	char * macro_name;
	stringitem macro_param_list;

	/*timescale*/
	int time_unit;
	char * time_unitname;
	int time_precision;
	char * time_precisionname;

	int error;

	int emitenabled;
	int ifstacktop;
	int waitendif;
	unsigned int ifstack[MAXIFSTACKDEPTH];

}sVerilogPreprocess;

#define FREE_MEMBER(n) \
do { \
	if (pPreprocess->n != NULL) \
		free(pPreprocess->n); \
	pPreprocess->n = NULL; \
}while (0)

#define IFSTACK_PUSH() \
do { \
	if (pPreprocess->emitenabled) \
		pPreprocess->ifstack[pPreprocess->ifstacktop >> 5] |= (1 << (pPreprocess->ifstacktop & 31)); \
	else \
		pPreprocess->ifstack[pPreprocess->ifstacktop >> 5] &= ~((1 << (pPreprocess->ifstacktop & 31))); \
	pPreprocess->ifstacktop++; \
}while (0)

#define IFSTACK_POP() \
do { \
	if (pPreprocess->ifstacktop > 0) { \
		pPreprocess->ifstacktop--; \
		if (pPreprocess->ifstack[pPreprocess->ifstacktop >> 5] & (1 << (pPreprocess->ifstacktop & 31))) { \
			pPreprocess->emitenabled = 1; \
		} else {\
			pPreprocess->emitenabled = 0; \
		} \
	} \
}while (0)

OBJECT_FUNCDECLARE(verilog_preprocess, CLSID_PREPROCESS_VERILOG);
PREPROCESS_FUNCDECLARE(verilog_preprocess, CLSID_PREPROCESS_VERILOG, sVerilogPreprocess);

OBJECT_FUNCIMPL(verilog_preprocess, sVerilogPreprocess, CLSID_PREPROCESS_VERILOG);

QUERYINTERFACE_BEGIN(verilog_preprocess, CLSID_PREPROCESS_VERILOG)
QUERYINTERFACE_ITEM(IID_PREPROCESS, IPreprocess, sVerilogPreprocess)
QUERYINTERFACE_END

static const char *verilog_preprocessModuleInfo()
{
	return "1.0.0-20210423.1035 Verilog Preprocess";
}

static int verilog_preprocessCreate(const PARAMITEM * pParams, int paramcount, HOBJECT * pObject)
{
	sVerilogPreprocess * pobj;
	pobj = (sVerilogPreprocess *)malloc(sizeof(sVerilogPreprocess));
	if (pobj == NULL)
		return -1;
	memset(pobj, 0, sizeof(sVerilogPreprocess));
	DLIST_INIT(&pobj->path_list);
	DLIST_INIT(&pobj->macro_list);
	DLIST_INIT(&pobj->param_list);
	DLIST_INIT(&pobj->log_list);
	DLIST_INIT(&pobj->define_param_list);
	DLIST_INIT(&pobj->macro_param_list);
	filestack_init(&pobj->file_stack);
	
	pobj->token = malloc(1024);
	pobj->tokenlen = 0;
	pobj->tokenbuflen = 1024;

	*pObject = 0;
	PREPROCESS_VARINIT(pobj, CLSID_PREPROCESS_VERILOG);
	INTERFACE_INIT(IPreprocess, pobj, verilog_preprocess, preprocess);
  	
	/* 返回生成的对象 */
	OBJECT_RETURN_GEN(verilog_preprocess, pobj, pObject, CLSID_PREPROCESS_VERILOG);
	return EIID_OK;
}


static void verilog_preprocessReset(HOBJECT object)
{
	sVerilogPreprocess * pPreprocess;
	pPreprocess = (sVerilogPreprocess *)objectThis(object);
	DLIST_DESTROY(stringitem, &pPreprocess->path_list);
	DLIST_DESTROY(stringitem, &pPreprocess->macro_list);
	DLIST_DESTROY(stringitem, &pPreprocess->param_list);
	DLIST_DESTROY(stringitem, &pPreprocess->log_list);
	filestack_destroy(&pPreprocess->file_stack);

	FREE_MEMBER(cwd);

	FREE_MEMBER(define_name);
	DLIST_DESTROY(stringitem, &pPreprocess->define_param_list);
	FREE_MEMBER(define_text);

	FREE_MEMBER(macro_name);
	DLIST_DESTROY(stringitem, &pPreprocess->define_param_list);

	FREE_MEMBER(token);
	FREE_MEMBER(line_filename);
	FREE_MEMBER(time_unitname);
	FREE_MEMBER(time_precisionname);

	pPreprocess->state = STATE_INITIAL;
	pPreprocess->tokenstate = TOKEN_NONE;
	pPreprocess->tokentype = TT_NONE;
	pPreprocess->emitenabled = 1;
	pPreprocess->ifstacktop = 0;
	pPreprocess->waitendif = 0;
}

static void verilog_preprocessDestroy(HOBJECT object)
{
	sVerilogPreprocess * pPreprocess;
	pPreprocess = (sVerilogPreprocess *)objectThis(object);
	verilog_preprocessReset(object);
	free(pPreprocess);
}

static int verilog_preprocessValid(HOBJECT object)
{
    return 1;
}

static int verilog_preprocess_preprocess_SetFile(HOBJECT object, const char * filename, int reset)
{
	char buf[2048];
	int len;
	FILE * pFile;
	sVerilogPreprocess * pPreprocess;
	pPreprocess = (sVerilogPreprocess *)objectThis(object);
	if (reset) {
		verilog_preprocessReset(object);
	}
	pPreprocess->emitenabled = 1;
	pFile = fopen(filename, "rb");
	if (pFile == NULL) {
		if (pPreprocess->file_stack_top != NULL) {
			sprintf(buf, "%s:%d,%d:Can't find file %s\n",
				pPreprocess->last_file_stack_top->filename,
				pPreprocess->last_file_stack_top->lineno,
				pPreprocess->last_file_stack_top->linepos,
				filename);
		}
		else {
			sprintf(buf, "Can't find file %s\n",
				filename);
			printf(buf);
		}
		stringlistaddstring(&pPreprocess->log_list, buf);
		pPreprocess->error = 1;
		return -1;
	} else {
		strcpy(buf, filename);
		len = strlen(buf)-1;
		for (;len>=0;len--) {
			if (buf[len] == '\\' || buf[len] == '/') {
				buf[len+1] = '\0';
				break;
			}
		}
	
		FREE_MEMBER(cwd);
		if (len > 0) {
			pPreprocess->cwd = strdup(buf);
		}
	}
	pPreprocess->last_file_stack_top = 
	pPreprocess->file_stack_top = filestack_push_file(&pPreprocess->file_stack, filename);
	pPreprocess->file_stack_top->pFile = pFile;
	return 0;
}

static int verilog_preprocess_preprocess_IncludeFile(HOBJECT object, const char * filename)
{
	int len;
	char buf[2048];
	char filen[256];
	FILE * pFile;
	sVerilogPreprocess * pPreprocess;
	pPreprocess = (sVerilogPreprocess *)objectThis(object);
	if (pPreprocess->emitenabled == 0)
		return -1;
	if ( (*filename == '"') || (*filename == '<'))
		filename++;
	strcpy(filen, filename);
	len = strlen(filen);
	if (len < 2)
		return -1;
	if ( (filen[len-1] == '"') || (filen[len-1] == '>'))
		filen[len-1] = 0;
	strcpy(buf, filen);
	pFile = fopen(buf, "rb");
	if (pFile != NULL)
		goto switchtofile;
	if (pPreprocess->cwd != NULL) {
		strcpy(buf, pPreprocess->cwd);
		strcat(buf, filen);
		pFile = fopen(buf, "rb");
		if (pFile != NULL)
			goto switchtofile;
	}
	DLIST_EVERY_ITEM_BEGIN(stringitem, &pPreprocess->path_list, pitem) {
		strcpy(buf, pitem->string);
		strcat(buf, filen);
		pFile = fopen(buf, "rb");
		if (pFile != NULL)
			goto switchtofile;		
	} DLIST_EVERY_ITEM_END();
	return -1;
switchtofile:
	pPreprocess->last_file_stack_top = 
	pPreprocess->file_stack_top = filestack_push_file(&pPreprocess->file_stack, buf);
	pPreprocess->file_stack_top->pFile = pFile;
	return 0;
}

static int verilog_preprocess_preprocess_AddIncludePath(HOBJECT object, const char * filename)
{
	char buf[2048];
	int len;
	sVerilogPreprocess * pPreprocess;
	pPreprocess = (sVerilogPreprocess *)objectThis(object);
	if (filename == NULL)
		return -1;
	len = strlen(filename);
	if (len == 0)
		return -1;
	if ( (filename[len-1] != '\\') && (filename[len-1] != '/')) {
		strcpy(buf, filename);
		strcat(buf, "\\");
		return stringlistaddstring(&pPreprocess->path_list, buf);
	} else {
		return stringlistaddstring(&pPreprocess->path_list, filename);
	}
}

static int verilog_preprocess_preprocess_GetCurrentFile(HOBJECT object, char *filename, int buflen, int *lineno, int *linepos, int *filepos)
{
	int len;
	sVerilogPreprocess * pPreprocess;
	pPreprocess = (sVerilogPreprocess *)objectThis(object);
	if (pPreprocess->last_file_stack_top == NULL)
		return 0;
	if (lineno != NULL)
		*lineno = pPreprocess->last_file_stack_top->lineno;
	if (linepos != NULL)
		*linepos = pPreprocess->last_file_stack_top->linepos;
	if (filepos != NULL)
		*filepos = pPreprocess->last_file_stack_top->filepos;
	len = strlen(pPreprocess->last_file_stack_top->filename) + 1;
	if (filename == NULL || buflen < len) {
		return len;
	}
	strcpy(filename, pPreprocess->last_file_stack_top->filename);
	return len;
}

static int verilog_preprocess_preprocess_SetParam(HOBJECT object, const char * param, const char * value)
{
	sVerilogPreprocess * pPreprocess;
	pPreprocess = (sVerilogPreprocess *)objectThis(object);
	if (pPreprocess->emitenabled == 0)
		return 0;
	return 0;
}

static int verilog_preprocess_preprocess_GetParam(HOBJECT object, const char * param, char * value, int buflen)
{
	sVerilogPreprocess * pPreprocess;
	pPreprocess = (sVerilogPreprocess *)objectThis(object);
	return 0;
}

static int verilog_preprocess_EnterCellDefine(sVerilogPreprocess * pPreprocess)
{
	PRINTF("[CD]Enter CellDefine\n");
	return 0;
}

static int verilog_preprocess_LeaveCellDefine(sVerilogPreprocess * pPreprocess)
{
	PRINTF("[CD]Leave CellDefine\n");
	return 0;
}

static const char * defaultnettype[] = { 
	"wire", 
	"tri", 
	"tri0", 
	"tri1", 
	"wand", 
	"triand", 
	"wor", 
	"trior", 
	"trireg", 
	"uwire", 
	"none",
};

static int verilog_preprocess_GetCh(sVerilogPreprocess * pPreprocess);

static int verilog_preprocess_error_to_eol(sVerilogPreprocess * pPreprocess)
{
	int ch;
	pPreprocess->state = STATE_INITIAL;
	pPreprocess->tokenstate = TOKEN_NONE;
	pPreprocess->tokentype = TT_NONE;
	do {
		ch = verilog_preprocess_GetCh(pPreprocess);
	} while ( (ch != '\n') && (ch != 0) );
	return 0;
}


static int verilog_preprocess_DefaultNetType(sVerilogPreprocess * pPreprocess, const char * name)
{
	PRINTF("[CD] defaut_nettype %s\n", name);
	return 0;
}

static int verilog_preprocess_HandleCDName(sVerilogPreprocess * pPreprocess)
{
	if (pPreprocess->tokentype == TT_SIMIDENT) {
		int nameid;
		for (nameid = 0;nameid<CDN_MACROIDENT;nameid++) {
			if (strcmp(cd_name[nameid], pPreprocess->token) == 0) {
				switch (nameid) {
				case CDN_CELLDEFINE:
					pPreprocess->state = STATE_INITIAL;
					verilog_preprocess_EnterCellDefine(pPreprocess);
					verilog_preprocess_error_to_eol(pPreprocess);
					break;
				case CDN_ENDCELLDEFINE:
					pPreprocess->state = STATE_INITIAL;
					verilog_preprocess_LeaveCellDefine(pPreprocess);
					verilog_preprocess_error_to_eol(pPreprocess);
					break;				
				case CDN_DEFAULT_NETTYPE:
					pPreprocess->state = STATE_CD_DEFAULT_NETTYPE;
					break;
				case CDN_DEFINE:
					pPreprocess->state = STATE_CD_DEFINE;
					break;
				case CDN_LINE:
					pPreprocess->state = STATE_CD_LINE;
					break;
				case CDN_RESETALL:
					pPreprocess->state = STATE_INITIAL;
					break;

				case CDN_ELSE:
					pPreprocess->state = STATE_INITIAL;
					verilog_preprocess_preprocess_PreAction(pPreprocess, PA_ELSE, "", "");
					verilog_preprocess_error_to_eol(pPreprocess);
					break;
				case CDN_ELSIF:
					pPreprocess->state = STATE_CD_ELSEIF;
					break;
				case CDN_ENDIF:
					pPreprocess->state = STATE_INITIAL;
					verilog_preprocess_preprocess_PreAction(pPreprocess, PA_ENDIF, "", "");
					verilog_preprocess_error_to_eol(pPreprocess);
					break;
				case CDN_IFDEF:
					pPreprocess->state = STATE_CD_IFDEF;
					break;
				case CDN_IFNDEF:
					pPreprocess->state = STATE_CD_IFNDEF;
					break;
				case CDN_UNDEF:
					pPreprocess->state = STATE_CD_UNDEF;
					break;
				case CDN_NOUNCONNECTED_DRIVE:
					pPreprocess->state = STATE_INITIAL;
					verilog_preprocess_error_to_eol(pPreprocess);
					break;
				case CDN_UNCONNECTED_DRIVE:
					pPreprocess->state = STATE_INITIAL;
					verilog_preprocess_error_to_eol(pPreprocess);
					break;
				case CDN_INCLUDE:
					pPreprocess->state = STATE_CD_INCLUDE;
					break;
				case CDN_TIMESCALE:
					pPreprocess->state = STATE_CD_TIMESCALE;
					break;
				case CDN_PRAGMA:
					pPreprocess->state = STATE_CD_PRAGMA;
					break;
				case CDN_BEGINKEYWORDS:
					pPreprocess->state = STATE_CD_BEGINKEYWORDS;
					break;
				case CDN_ENDKEYWORDS:
					pPreprocess->state = STATE_INITIAL;
					verilog_preprocess_error_to_eol(pPreprocess);
					break;
				}
				break;
			}
		}
		if (nameid == CDN_MACROIDENT) {
			pPreprocess->state = STATE_CD_MACRO_PARAM;
			pPreprocess->tokenstate = TOKEN_NONE;
			pPreprocess->tokentype = TT_NONE;
			FREE_MEMBER(macro_name);
			pPreprocess->macro_name = strdup(pPreprocess->token);
			DLIST_DESTROY(stringitem, &pPreprocess->macro_param_list);
		}
	} else if (pPreprocess->tokentype == TT_ESCIDENT) {
		pPreprocess->state = STATE_CD_MACRO_PARAM;
		pPreprocess->tokenstate = TOKEN_NONE;
		pPreprocess->tokentype = TT_NONE;
		FREE_MEMBER(macro_name);
		pPreprocess->macro_name = strdup(pPreprocess->token);
		DLIST_DESTROY(stringitem, &pPreprocess->macro_param_list);
	}
	pPreprocess->tokentype = TT_NONE;
	return 0;
}

static int verilog_preprocess_GetCh(sVerilogPreprocess * pPreprocess)
{
	filestack * curfile;
	int ch;
	curfile = pPreprocess->file_stack_top;
	if (curfile == NULL) {
		ch = 0;
		goto returnch;
	}
	if (curfile->ungetch != 0) {
		ch = curfile->ungetch;
		curfile->ungetch = 0;
		goto returnch;
	} 
	/* 找到一个有字符的缓冲区 */
	do {
		while (curfile->bufindex == curfile->buflen) {
			if (curfile->isbufonly) {
				/* 此时前面入栈的是一个字符串，则打开的文件没有关闭，这样不需要重新打开文件 */
				pPreprocess->file_stack_top = filestack_pop(&pPreprocess->file_stack);
				if (pPreprocess->file_stack_top == NULL) {
					ch = 0;
					goto returnch;
				}
				if (pPreprocess->file_stack_top->isbufonly==0) {
					pPreprocess->last_file_stack_top = pPreprocess->file_stack_top;
				}
				curfile = pPreprocess->file_stack_top;
			} else {
				if (feof(curfile->pFile)) {
					fclose(curfile->pFile);
					curfile->pFile = NULL;
					
					pPreprocess->file_stack_top = filestack_pop(&pPreprocess->file_stack);
					if (pPreprocess->file_stack_top == NULL) {
						ch = 0;
						goto returnch;
					}
					curfile = pPreprocess->file_stack_top;
					if (curfile->isbufonly == 0) {	
						//pPreprocess->pFile = fopen(curfile->filename, "rb");
						//fseek(pPreprocess->pFile, 0, curfile->filepos);
						pPreprocess->last_file_stack_top = curfile;
					}
				} else {
					curfile->buflen = fread(curfile->buf, 1, BUFLEN, curfile->pFile);
					curfile->filepos += curfile->buflen;
					curfile->bufindex = 0;
				}
			}
		}
		while (curfile->bufindex < curfile->buflen) {
			ch = curfile->buf[curfile->bufindex]; 
			curfile->bufindex++;
			/* 滤掉非字符串中的\r字符 */
			if ( (ch == '\r') && (pPreprocess->tokenstate != TOKEN_IN_STRING) )
				continue;
			goto returnch;
		}
	} while (1);
returnch:
	if (pPreprocess->file_stack_top != NULL) {
		if (ch == '\n') {
			pPreprocess->file_stack_top->lineno ++;
			pPreprocess->file_stack_top->linepos = 0;
		}
		pPreprocess->file_stack_top->linepos++;
	}
	return ch;
}

#define isdigital(cc)    ((cc >= '0') && (cc <= '9'))
#define isblank(cc)      ((cc == ' ') || (cc == '\t') || (cc == 0) )
#define isidentstart(cc) ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch == '_') )
#define isident(cc)      (isidentstart(cc) || isdigital(cc) || (cc == '$') )

#define NEXT_NORMAL    0
#define NEXT_RETURNCH  1
#define NEXT_CONTINUE  2

static char logbuf[2048];

static int verilog_preprocess_GetToken(sVerilogPreprocess* pPreprocess, int ch, int *pch)
{
	if (pPreprocess->tokenstate == TOKEN_NONE)
		return NEXT_NORMAL;
	/*
		确保单词缓冲区至少有三个空位，预留一个给结尾的零字符，
		可能在下面的解析过程中一次生成两个字符，要确保tokenbuflen > 2 
	*/
	if (pPreprocess->tokenlen == pPreprocess->tokenbuflen - 2) {
		char * newbuf = malloc(pPreprocess->tokenbuflen * 2);
		if (newbuf == NULL) {
			*pch = 0;
			return NEXT_RETURNCH;
		}
		memcpy(newbuf, pPreprocess->token, pPreprocess->tokenlen);
		pPreprocess->tokenbuflen *= 2;
		free(pPreprocess->token);
		pPreprocess->token = newbuf;
	}

	/* 在获取token流程中 */
	if (pPreprocess->tokenstate == TOKEN_IN_NUMBER) {
		if (isdigital(ch)) {
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			return NEXT_CONTINUE;
		} else {
			pPreprocess->tokenstate = TOKEN_NONE;
			pPreprocess->tokentype = TT_NUMBER;
			pPreprocess->token[pPreprocess->tokenlen] = 0;
			pPreprocess->file_stack_top->ungetch = ch;
			*pch = ' ';
		}
	} else if (pPreprocess->tokenstate == TOKEN_IN_SIMIDENT) {
		/*IEEE 1364-2005 A.9.3 simple_identifier*/
		if ( isident(ch) ) {
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			return NEXT_CONTINUE;
		} else { /* 完成一个SIMIDENT */
			pPreprocess->file_stack_top->ungetch = ch;
			*pch = ' ';
			pPreprocess->tokenstate = TOKEN_NONE;
			pPreprocess->tokentype = TT_SIMIDENT;
			pPreprocess->token[pPreprocess->tokenlen] = 0;
		}
	} else if (pPreprocess->tokenstate == TOKEN_IN_ESCIDENT) {
		if ( !(isblank(ch) || (ch == '\n') ) ) {
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			return NEXT_CONTINUE;
		} else {
			pPreprocess->file_stack_top->ungetch = ch;
			*pch = ' ';
			pPreprocess->token[pPreprocess->tokenlen] = 0;
			pPreprocess->tokenstate = TOKEN_NONE;
			pPreprocess->tokentype = TT_ESCIDENT;
		}		
	} else if (pPreprocess->tokenstate == TOKEN_IN_STRING) {
		if (ch == '\\') {
			pPreprocess->tokenstate = TOKEN_IN_STRING_SLASH;
			pPreprocess->lastch = '\\';
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			if (pPreprocess->state == STATE_INITIAL)
				return NEXT_RETURNCH;
			return NEXT_CONTINUE;
		} else if (ch == '"') {
			pPreprocess->tokenstate = TOKEN_NONE;
			pPreprocess->tokentype = TT_STRING;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->token[pPreprocess->tokenlen] = 0;
			pPreprocess->lastch = ch;
			if (pPreprocess->state == STATE_INITIAL)
				return NEXT_RETURNCH;
		} else {
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->lastch = ch;
			if (pPreprocess->state == STATE_INITIAL)
				return NEXT_RETURNCH;
			return NEXT_CONTINUE;
		}
	} else if (pPreprocess->tokenstate == TOKEN_IN_STRING_SLASH) {
		pPreprocess->tokenstate = TOKEN_IN_STRING;
		pPreprocess->token[pPreprocess->tokenlen++] = ch;
		pPreprocess->lastch = ch;
		if (pPreprocess->state == STATE_INITIAL)
			return NEXT_RETURNCH;
		return NEXT_CONTINUE;
	} else if (pPreprocess->tokenstate == TOKEN_COMMENT_START) {
		if (ch == '/') {
			pPreprocess->tokenstate = TOKEN_IN_COMMENTLINE;
			pPreprocess->lastch = '/';
			return NEXT_CONTINUE;
		} else if (ch == '*') {
			pPreprocess->tokenstate = TOKEN_IN_COMMENT;
			pPreprocess->lastch = ' '; /* 这个*号不能作为结束注释的开始*号 */
			return NEXT_CONTINUE;
		} else {
			pPreprocess->file_stack_top->ungetch = ch;
			*pch = '/';
			pPreprocess->tokenstate = TOKEN_NONE;
			pPreprocess->tokentype = TT_SYMBOL; /*token text is in ch*/
			if (pPreprocess->state == STATE_INITIAL)
				return NEXT_RETURNCH;
		}
	} else if (pPreprocess->tokenstate == TOKEN_IN_COMMENT) {
		if ( (ch == '/') && (pPreprocess->lastch == '*') ) {
			pPreprocess->file_stack_top->ungetch = ' ';
			*pch = ' ';
			pPreprocess->tokenstate = TOKEN_NONE;
			pPreprocess->tokentype = TT_NONE;
			return NEXT_CONTINUE;
		} else {
			pPreprocess->lastch = ch;
			return NEXT_CONTINUE;
		}
	} else if (pPreprocess->tokenstate == TOKEN_IN_COMMENTLINE) {
		if (ch == '\n') {
			pPreprocess->tokenstate = TOKEN_NONE;
			pPreprocess->tokentype = TT_NONE;
			pPreprocess->file_stack_top->ungetch = '\n';
			pPreprocess->lastch = ch;
			return NEXT_CONTINUE;
		} else {
			pPreprocess->lastch = ch;
			return NEXT_CONTINUE;
		}
	} else if (pPreprocess->tokenstate == TOKEN_IN_MACRO_TEXT) {
		if (ch == '\n') {
			pPreprocess->tokenstate = TOKEN_NONE;
			pPreprocess->tokentype = TT_MACRO_TEXT;
			pPreprocess->token[pPreprocess->tokenlen] = 0;
			pPreprocess->file_stack_top->ungetch = '\n';
		} else if (ch == '\\') {
			pPreprocess->lastch = ch;
			pPreprocess->tokenstate = TOKEN_IN_MACRO_TEXT_SLASH;
			return NEXT_CONTINUE;
		} else {
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->lastch = ch;
			return NEXT_CONTINUE;
		}
	} else if (pPreprocess->tokenstate == TOKEN_IN_MACRO_TEXT_SLASH) {
		if (ch == '\n') {
			pPreprocess->lastch = ch;
			pPreprocess->token[pPreprocess->tokenlen++] = ' ';
		} else {
			pPreprocess->token[pPreprocess->tokenlen++] = '\\';
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->lastch = ch;
		}
		pPreprocess->tokenstate = TOKEN_IN_MACRO_TEXT;
		return NEXT_CONTINUE;
	}
	return NEXT_NORMAL;
}

static int verilog_preprocess_state_initial(sVerilogPreprocess * pPreprocess, int ch, int *pch)
{
	if (pPreprocess->state == STATE_INITIAL) {
		/*
		  不在compiler directive中，则处理CD开始符号，
		  注释开始和字符串开始
		*/
		if (ch == '`') {
			pPreprocess->state = STATE_CD_START;
			pPreprocess->tokenstate = TOKEN_NONE;
			pPreprocess->lastch = '`';
		} else if (ch == '/') {
			if (pPreprocess->tokentype == TT_SYMBOL) {
				return NEXT_RETURNCH;
			} else {
				pPreprocess->tokenstate = TOKEN_COMMENT_START;
				pPreprocess->lastch = '/';
			}
		} else if (ch == '"') {
			pPreprocess->tokenstate = TOKEN_IN_STRING;
			pPreprocess->tokenlen = 0;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			return NEXT_RETURNCH;
		} else {
			return NEXT_RETURNCH;
		}
		return NEXT_CONTINUE;
	}
	return NEXT_NORMAL;
}

static int verilog_preprocess_state_cd(sVerilogPreprocess * pPreprocess, int ch, int *pch)
{
	if (pPreprocess->state == STATE_CD_START) {
		if ( isidentstart(ch) ) { /*IEEE 1364-2005 A.9.3 simple_identifier*/
			pPreprocess->tokenlen = 0;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->state = STATE_CD_NAME;
			pPreprocess->tokenstate = TOKEN_IN_SIMIDENT;
		} else if (ch == '\\') { /*IEEE 1364-2005 A.9.3 escaped_identifier*/
			pPreprocess->tokenlen = 0;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->state = STATE_CD_NAME;
			pPreprocess->tokenstate = TOKEN_IN_ESCIDENT;
		} else if ( (ch == '/') && (pPreprocess->tokentype != TT_SYMBOL)) {
			pPreprocess->tokenstate = TOKEN_COMMENT_START;
			pPreprocess->lastch = '/';
		} else if ( isblank(ch) || (ch == '\n') ) {
		} else {
			sprintf(logbuf, "%d,%d: Expect Ident or compiler directive\n", 
						pPreprocess->last_file_stack_top->lineno,
						pPreprocess->last_file_stack_top->linepos);
			stringlistaddstring(&pPreprocess->log_list, logbuf);
			pPreprocess->state = STATE_INITIAL;
			verilog_preprocess_error_to_eol(pPreprocess);
			*pch = 0;
			return NEXT_RETURNCH;
		}
		return NEXT_CONTINUE;
	} else if (pPreprocess->state == STATE_CD_NAME) {
		verilog_preprocess_HandleCDName(pPreprocess);
		pPreprocess->tokenstate = TOKEN_NONE;
		pPreprocess->tokentype = TT_NONE;
		return NEXT_CONTINUE;
	}
	return NEXT_NORMAL;
}

static int verilog_preprocess_state_default_nettype(sVerilogPreprocess * pPreprocess, int ch, int *pch)
{
	if (pPreprocess->state == STATE_CD_DEFAULT_NETTYPE) {
		if (isidentstart(ch)) {
			pPreprocess->tokenlen = 0;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->state = STATE_CD_DEFAULT_NETTYPE_NAME;
			pPreprocess->tokenstate = TOKEN_IN_SIMIDENT;
		} else if (isblank(ch) || (ch == '\n') ) {
		} else if ( (ch == '/') && (pPreprocess->tokentype != TT_SYMBOL)) {
			pPreprocess->tokenstate = TOKEN_COMMENT_START;
			pPreprocess->lastch = '/';
		} else {
			sprintf(logbuf, "%d,%d: Expect nettype ident\n", 
						pPreprocess->last_file_stack_top->lineno,
						pPreprocess->last_file_stack_top->linepos);
			stringlistaddstring(&pPreprocess->log_list, logbuf);
			pPreprocess->state = STATE_INITIAL;
			verilog_preprocess_error_to_eol(pPreprocess);
			*pch = 0;
			return 1;
		}
		return 2;
	} else if (pPreprocess->state == STATE_CD_DEFAULT_NETTYPE_NAME) {
		verilog_preprocess_DefaultNetType(pPreprocess, pPreprocess->token);
		pPreprocess->state = STATE_INITIAL;
		verilog_preprocess_error_to_eol(pPreprocess);
		return 2;
	}
	return 0;
}

static int verilog_preprocess_state_undef(sVerilogPreprocess * pPreprocess, int ch, int *pch)
{
	if (pPreprocess->state == STATE_CD_UNDEF) {
		if (isidentstart(ch)) {
			pPreprocess->tokenlen = 0;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->state = STATE_CD_UNDEF_NAME;
			pPreprocess->tokenstate = TOKEN_IN_SIMIDENT;
		} else if (ch == '\\') {
			pPreprocess->tokenlen = 0;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->state = STATE_CD_UNDEF_NAME;
			pPreprocess->tokenstate = TOKEN_IN_ESCIDENT;
		} else if (isblank(ch) || (ch == '\n') ) {
		} else if ( (ch == '/') && (pPreprocess->tokentype != TT_SYMBOL)) {
			pPreprocess->tokenstate = TOKEN_COMMENT_START;
			pPreprocess->lastch = '/';
		} else {
			sprintf(logbuf, "%d,%d: Expect an ident\n", 
						pPreprocess->last_file_stack_top->lineno,
						pPreprocess->last_file_stack_top->linepos);
			stringlistaddstring(&pPreprocess->log_list, logbuf);
			pPreprocess->state = STATE_INITIAL;
			*pch = 0;
			pPreprocess->tokentype = TT_NONE;
			pPreprocess->tokenstate = TOKEN_NONE;
			verilog_preprocess_error_to_eol(pPreprocess);
			return NEXT_RETURNCH;
		}
		pPreprocess->tokentype = TT_NONE;
		return NEXT_CONTINUE;
	} else if (pPreprocess->state == STATE_CD_UNDEF_NAME) {
		if (pPreprocess->tokentype == TT_SIMIDENT ||
			pPreprocess->tokentype == TT_ESCIDENT) {
			FREE_MEMBER(define_name);
			pPreprocess->define_name = strdup(pPreprocess->token);
			DLIST_DESTROY(stringitem, &pPreprocess->define_param_list);
			pPreprocess->tokentype = TT_NONE;
			pPreprocess->tokenstate = TOKEN_NONE;
			pPreprocess->state = STATE_INITIAL;
			verilog_preprocess_preprocess_PreAction(pPreprocess, PA_UNDEF, pPreprocess->define_name, "");
			verilog_preprocess_error_to_eol(pPreprocess);
		}
		return NEXT_CONTINUE;
	}
	return 0;
}

static int verilog_preprocess_state_elseif(sVerilogPreprocess * pPreprocess, int ch, int *pch)
{
	if (pPreprocess->state == STATE_CD_ELSEIF) {
		if (isidentstart(ch)) {
			pPreprocess->tokenlen = 0;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->state = STATE_CD_ELSEIF_NAME;
			pPreprocess->tokenstate = TOKEN_IN_SIMIDENT;
		} else if (ch == '\\') {
			pPreprocess->tokenlen = 0;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->state = STATE_CD_ELSEIF_NAME;
			pPreprocess->tokenstate = TOKEN_IN_ESCIDENT;
		} else if (isblank(ch) || (ch == '\n') ) {
		} else if ( (ch == '/') && (pPreprocess->tokentype != TT_SYMBOL)) {
			pPreprocess->tokenstate = TOKEN_COMMENT_START;
			pPreprocess->lastch = '/';
		} else {
			sprintf(logbuf, "%d,%d: Expect an ident\n", 
						pPreprocess->last_file_stack_top->lineno,
						pPreprocess->last_file_stack_top->linepos);
			stringlistaddstring(&pPreprocess->log_list, logbuf);
			pPreprocess->state = STATE_INITIAL;
			*pch = 0;
			pPreprocess->tokentype = TT_NONE;
			pPreprocess->tokenstate = TOKEN_NONE;
			verilog_preprocess_error_to_eol(pPreprocess);
			return NEXT_RETURNCH;
		}
		pPreprocess->tokentype = TT_NONE;
		return NEXT_CONTINUE;
	} else if (pPreprocess->state == STATE_CD_ELSEIF_NAME) {
		if (pPreprocess->tokentype == TT_SIMIDENT ||
			pPreprocess->tokentype == TT_ESCIDENT) {
			FREE_MEMBER(define_name);
			pPreprocess->define_name = strdup(pPreprocess->token);
			DLIST_DESTROY(stringitem, &pPreprocess->define_param_list);
			pPreprocess->tokentype = TT_NONE;
			pPreprocess->tokenstate = TOKEN_NONE;
			pPreprocess->state = STATE_INITIAL;
			verilog_preprocess_preprocess_PreAction(pPreprocess, PA_ELSEIFDEF, pPreprocess->define_name, "");
			verilog_preprocess_error_to_eol(pPreprocess);
		}
		return NEXT_CONTINUE;
	}
	return 0;
}

static int verilog_preprocess_state_ifdef(sVerilogPreprocess * pPreprocess, int ch, int *pch)
{
	if (pPreprocess->state == STATE_CD_IFDEF) {
		if (isidentstart(ch)) {
			pPreprocess->tokenlen = 0;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->state = STATE_CD_IFDEF_NAME;
			pPreprocess->tokenstate = TOKEN_IN_SIMIDENT;
		} else if (ch == '\\') {
			pPreprocess->tokenlen = 0;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->state = STATE_CD_IFDEF_NAME;
			pPreprocess->tokenstate = TOKEN_IN_ESCIDENT;
		} else if (isblank(ch) || (ch == '\n') ) {
		} else if ( (ch == '/') && (pPreprocess->tokentype != TT_SYMBOL)) {
			pPreprocess->tokenstate = TOKEN_COMMENT_START;
			pPreprocess->lastch = '/';
		} else {
			sprintf(logbuf, "%d,%d: Expect an ident\n", 
						pPreprocess->last_file_stack_top->lineno,
						pPreprocess->last_file_stack_top->linepos);
			stringlistaddstring(&pPreprocess->log_list, logbuf);
			pPreprocess->state = STATE_INITIAL;
			*pch = 0;
			pPreprocess->tokentype = TT_NONE;
			pPreprocess->tokenstate = TOKEN_NONE;
			verilog_preprocess_error_to_eol(pPreprocess);
			return NEXT_RETURNCH;
		}
		pPreprocess->tokentype = TT_NONE;
		return NEXT_CONTINUE;
	} else if (pPreprocess->state == STATE_CD_IFDEF_NAME) {
		if (pPreprocess->tokentype == TT_SIMIDENT ||
			pPreprocess->tokentype == TT_ESCIDENT) {
			FREE_MEMBER(define_name);
			pPreprocess->define_name = strdup(pPreprocess->token);
			DLIST_DESTROY(stringitem, &pPreprocess->define_param_list);
			pPreprocess->tokentype = TT_NONE;
			pPreprocess->tokenstate = TOKEN_NONE;
			pPreprocess->state = STATE_INITIAL;
			verilog_preprocess_preprocess_PreAction(pPreprocess, PA_IFDEF, pPreprocess->define_name, "");
			verilog_preprocess_error_to_eol(pPreprocess);
		}
		return NEXT_CONTINUE;
	}
	return 0;
}

static int verilog_preprocess_state_ifndef(sVerilogPreprocess * pPreprocess, int ch, int *pch)
{
	if (pPreprocess->state == STATE_CD_IFNDEF) {
		if (isidentstart(ch)) {
			pPreprocess->tokenlen = 0;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->state = STATE_CD_IFNDEF_NAME;
			pPreprocess->tokenstate = TOKEN_IN_SIMIDENT;
		} else if (ch == '\\') {
			pPreprocess->tokenlen = 0;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->state = STATE_CD_IFNDEF_NAME;
			pPreprocess->tokenstate = TOKEN_IN_ESCIDENT;
		} else if (isblank(ch) || (ch == '\n') ) {
		} else if ( (ch == '/') && (pPreprocess->tokentype != TT_SYMBOL)) {
			pPreprocess->tokenstate = TOKEN_COMMENT_START;
			pPreprocess->lastch = '/';
		} else {
			sprintf(logbuf, "%d,%d: Expect an ident\n", 
						pPreprocess->last_file_stack_top->lineno,
						pPreprocess->last_file_stack_top->linepos);
			stringlistaddstring(&pPreprocess->log_list, logbuf);
			pPreprocess->state = STATE_INITIAL;
			*pch = 0;
			pPreprocess->tokentype = TT_NONE;
			pPreprocess->tokenstate = TOKEN_NONE;
			verilog_preprocess_error_to_eol(pPreprocess);
			return NEXT_RETURNCH;
		}
		pPreprocess->tokentype = TT_NONE;
		return NEXT_CONTINUE;
	} else if (pPreprocess->state == STATE_CD_IFNDEF_NAME) {
		if (pPreprocess->tokentype == TT_SIMIDENT ||
			pPreprocess->tokentype == TT_ESCIDENT) {
			FREE_MEMBER(define_name);
			pPreprocess->define_name = strdup(pPreprocess->token);
			DLIST_DESTROY(stringitem, &pPreprocess->define_param_list);
			pPreprocess->tokentype = TT_NONE;
			pPreprocess->tokenstate = TOKEN_NONE;
			pPreprocess->state = STATE_INITIAL;
			verilog_preprocess_preprocess_PreAction(pPreprocess, PA_IFNDEF, pPreprocess->define_name, "");
			verilog_preprocess_error_to_eol(pPreprocess);
		}
		return NEXT_CONTINUE;
	}
	return 0;
}

static int verilog_preprocess_state_include(sVerilogPreprocess * pPreprocess, int ch, int *pch)
{
	if (pPreprocess->state == STATE_CD_INCLUDE) {
		if (ch == '"') {
			pPreprocess->tokenlen = 0;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->state = STATE_CD_INCLUDE_NAME;
			pPreprocess->tokenstate = TOKEN_IN_STRING;
		} else if (isblank(ch) || (ch == '\n') ) {
		} else if ( (ch == '/') && (pPreprocess->tokentype != TT_SYMBOL)) {
			pPreprocess->tokenstate = TOKEN_COMMENT_START;
			pPreprocess->lastch = '/';
		} else {
			sprintf(logbuf, "%d,%d: Expect an string as filename \n", 
						pPreprocess->last_file_stack_top->lineno,
						pPreprocess->last_file_stack_top->linepos);
			stringlistaddstring(&pPreprocess->log_list, logbuf);
			pPreprocess->state = STATE_INITIAL;
			*pch = 0;
			pPreprocess->tokentype = TT_NONE;
			pPreprocess->tokenstate = TOKEN_NONE;
			verilog_preprocess_error_to_eol(pPreprocess);
			return NEXT_RETURNCH;
		}
		pPreprocess->tokentype = TT_NONE;
		return NEXT_CONTINUE;
	} else if (pPreprocess->state == STATE_CD_INCLUDE_NAME) {
		if (pPreprocess->tokentype == TT_STRING) {
			pPreprocess->tokentype = TT_NONE;
			pPreprocess->tokenstate = TOKEN_NONE;
			pPreprocess->state = STATE_INITIAL;
			verilog_preprocess_preprocess_PreAction(pPreprocess, PA_INCLUDE, pPreprocess->token, "");
			verilog_preprocess_error_to_eol(pPreprocess);
		}
		return NEXT_CONTINUE;
	}
	return 0;
}

static int verilog_preprocess_state_timescale(sVerilogPreprocess * pPreprocess, int ch, int *pch)
{
	if (pPreprocess->state == STATE_CD_TIMESCALE) {
		if (ch == '1') {
			pPreprocess->tokenlen = 0;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->state = STATE_CD_TIMESCALE_UNIT;
			pPreprocess->tokenstate = TOKEN_IN_NUMBER;
		} else if (isblank(ch) || (ch == '\n') ) {
		} else if ( (ch == '/') && (pPreprocess->tokentype != TT_SYMBOL)) {
			pPreprocess->tokenstate = TOKEN_COMMENT_START;
			pPreprocess->lastch = '/';
		} else {
			sprintf(logbuf, "%d,%d: expect 1, 10 or 100 \n", 
						pPreprocess->last_file_stack_top->lineno,
						pPreprocess->last_file_stack_top->linepos);
			stringlistaddstring(&pPreprocess->log_list, logbuf);
			pPreprocess->state = STATE_INITIAL;
			*pch = 0;
			pPreprocess->tokentype = TT_NONE;
			pPreprocess->tokenstate = TOKEN_NONE;
			verilog_preprocess_error_to_eol(pPreprocess);
			return NEXT_RETURNCH;
		}
		pPreprocess->tokentype = TT_NONE;
		return NEXT_CONTINUE;
	} else if (pPreprocess->state == STATE_CD_TIMESCALE_UNIT) {
		if (pPreprocess->tokentype == TT_NUMBER) {
			pPreprocess->time_unit = atoi(pPreprocess->token);
			if (pPreprocess->time_unit != 1 &&
				pPreprocess->time_unit != 10 &&
				pPreprocess->time_unit != 100) {
				sprintf(logbuf, "%d,%d: expect 1, 10 or 100 \n", 
						pPreprocess->last_file_stack_top->lineno,
						pPreprocess->last_file_stack_top->linepos);
				stringlistaddstring(&pPreprocess->log_list, logbuf);
			}
			pPreprocess->tokentype = TT_NONE;
			pPreprocess->tokenstate = TOKEN_NONE;
		} else if (isidentstart(ch)) {
			pPreprocess->tokenlen = 0;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->state = STATE_CD_TIMESCALE_UNIT_NAME;
			pPreprocess->tokenstate = TOKEN_IN_SIMIDENT;
		} else if (isblank(ch) || (ch == '\n') ) {
		} else if ( (ch == '/') && (pPreprocess->tokentype != TT_SYMBOL)) {
			pPreprocess->tokenstate = TOKEN_COMMENT_START;
			pPreprocess->lastch = '/';
		} else {
			sprintf(logbuf, "%d,%d: expect s, ms, us, ns or ps \n", 
						pPreprocess->last_file_stack_top->lineno,
						pPreprocess->last_file_stack_top->linepos);
			stringlistaddstring(&pPreprocess->log_list, logbuf);
			pPreprocess->state = STATE_INITIAL;
			*pch = 0;
			pPreprocess->tokentype = TT_NONE;
			pPreprocess->tokenstate = TOKEN_NONE;
			verilog_preprocess_error_to_eol(pPreprocess);
			return NEXT_RETURNCH;
		}
		pPreprocess->tokentype = TT_NONE;
		return NEXT_CONTINUE;
	} else if (pPreprocess->state == STATE_CD_TIMESCALE_UNIT_NAME) {
		if (pPreprocess->tokentype == TT_SIMIDENT) {
			FREE_MEMBER(time_unitname);
			pPreprocess->time_unitname = strdup(pPreprocess->token);
			pPreprocess->tokentype = TT_NONE;
			pPreprocess->tokenstate = TOKEN_NONE;
		}
		if (isblank(ch) || (ch == '\n') ) {
		} else if ( (ch == '/') ) {
			if (pPreprocess->tokentype == TT_SYMBOL) {
				pPreprocess->state = STATE_CD_TIMESCALE_PRECISION_PRE;
				pPreprocess->tokenstate = TOKEN_NONE;
			} else {
				pPreprocess->tokenstate = TOKEN_COMMENT_START;
			}
			pPreprocess->lastch = '/';
		} else {
			sprintf(logbuf, "%d,%d: expect / \n", 
						pPreprocess->last_file_stack_top->lineno,
						pPreprocess->last_file_stack_top->linepos);
			stringlistaddstring(&pPreprocess->log_list, logbuf);
			pPreprocess->state = STATE_INITIAL;
			*pch = 0;
			pPreprocess->tokentype = TT_NONE;
			pPreprocess->tokenstate = TOKEN_NONE;
			verilog_preprocess_error_to_eol(pPreprocess);
			return NEXT_RETURNCH;
		}
		pPreprocess->tokentype = TT_NONE;
		return NEXT_CONTINUE;
	} else if (pPreprocess->state == STATE_CD_TIMESCALE_PRECISION_PRE) {
		if (ch == '1') {
			pPreprocess->tokenlen = 0;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->state = STATE_CD_TIMESCALE_PRECISION;
			pPreprocess->tokenstate = TOKEN_IN_NUMBER;
		} else if (isblank(ch) || (ch == '\n') ) {
		} else if ( (ch == '/') && (pPreprocess->tokentype != TT_SYMBOL)) {
			pPreprocess->tokenstate = TOKEN_COMMENT_START;
			pPreprocess->lastch = '/';
		} else {
			sprintf(logbuf, "%d,%d: expect 1, 10 or 100 \n", 
						pPreprocess->last_file_stack_top->lineno,
						pPreprocess->last_file_stack_top->linepos);
			stringlistaddstring(&pPreprocess->log_list, logbuf);
			pPreprocess->state = STATE_INITIAL;
			*pch = 0;
			pPreprocess->tokentype = TT_NONE;
			pPreprocess->tokenstate = TOKEN_NONE;
			verilog_preprocess_error_to_eol(pPreprocess);
			return NEXT_RETURNCH;
		}
		pPreprocess->tokentype = TT_NONE;
		return NEXT_CONTINUE;
	} else if (pPreprocess->state == STATE_CD_TIMESCALE_PRECISION) {
		if (pPreprocess->tokentype == TT_NUMBER) {
			pPreprocess->time_precision = atoi(pPreprocess->token);
			if (pPreprocess->time_unit != 1 &&
				pPreprocess->time_unit != 10 &&
				pPreprocess->time_unit != 100) {
				sprintf(logbuf, "%d,%d: expect 1, 10 or 100 \n", 
						pPreprocess->last_file_stack_top->lineno,
						pPreprocess->last_file_stack_top->linepos);
				stringlistaddstring(&pPreprocess->log_list, logbuf);
			}
			pPreprocess->tokentype = TT_NONE;
			pPreprocess->tokenstate = TOKEN_NONE;
		} else if (isidentstart(ch)) {
			pPreprocess->tokenlen = 0;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->state = STATE_CD_TIMESCALE_PRECISION_NAME;
			pPreprocess->tokenstate = TOKEN_IN_SIMIDENT;
		} else if (isblank(ch) || (ch == '\n') ) {
		} else if ( (ch == '/') && (pPreprocess->tokentype != TT_SYMBOL)) {
			pPreprocess->tokenstate = TOKEN_COMMENT_START;
			pPreprocess->lastch = '/';
		} else {
			sprintf(logbuf, "%d,%d: expect s, ms, us, ns or ps \n", 
						pPreprocess->last_file_stack_top->lineno,
						pPreprocess->last_file_stack_top->linepos);
			stringlistaddstring(&pPreprocess->log_list, logbuf);
			pPreprocess->state = STATE_INITIAL;
			*pch = 0;
			pPreprocess->tokentype = TT_NONE;
			pPreprocess->tokenstate = TOKEN_NONE;
			verilog_preprocess_error_to_eol(pPreprocess);
			return NEXT_RETURNCH;
		}
		pPreprocess->tokentype = TT_NONE;
		return NEXT_CONTINUE;
	} else if (pPreprocess->state == STATE_CD_TIMESCALE_PRECISION_NAME) {
		if (pPreprocess->tokentype == TT_SIMIDENT) {
			FREE_MEMBER(time_precisionname);
			pPreprocess->time_precisionname = strdup(pPreprocess->token);
			PRINTF("[CD] timescale %d %s/%d %s\n", 
				pPreprocess->time_unit,
				pPreprocess->time_unitname,
				pPreprocess->time_precision,
				pPreprocess->time_precisionname
				);
		}
		pPreprocess->tokenstate = TOKEN_NONE;
		verilog_preprocess_error_to_eol(pPreprocess);
		pPreprocess->state = STATE_INITIAL;
		pPreprocess->tokentype = TT_NONE;
		return NEXT_CONTINUE;
	}
	return NEXT_NORMAL;
}

static int verilog_preprocess_state_pragma(sVerilogPreprocess * pPreprocess, int ch, int *pch)
{
	if (pPreprocess->state == STATE_CD_PRAGMA) {
		pPreprocess->state = STATE_INITIAL;
		pPreprocess->tokenstate = TOKEN_NONE;
		pPreprocess->tokentype = TT_NONE;
		verilog_preprocess_error_to_eol(pPreprocess);
	}
	return NEXT_NORMAL;
}

static int verilog_preprocess_state_begin_keywords(sVerilogPreprocess * pPreprocess, int ch, int *pch)
{
	if (pPreprocess->state == STATE_CD_BEGINKEYWORDS) {
		pPreprocess->state = STATE_INITIAL;
		pPreprocess->tokenstate = TOKEN_NONE;
		pPreprocess->tokentype = TT_NONE;
		verilog_preprocess_error_to_eol(pPreprocess);
	}
	return NEXT_NORMAL;
}

static int verilog_preprocess_GetMacroParamText(sVerilogPreprocess * pPreprocess)
{
	int ch, lastch;
	int needright = 1;
	int tokenstate = TOKEN_NONE;
	pPreprocess->tokenlen = 0;
	do {
		if (pPreprocess->tokenlen == pPreprocess->tokenbuflen - 2) {
			char * newbuf = malloc(pPreprocess->tokenbuflen * 2);
			if (newbuf == NULL) {
				return 0;
			}
			if (pPreprocess->tokenlen > 0)
				memcpy(newbuf, pPreprocess->token, pPreprocess->tokenlen);
			pPreprocess->tokenbuflen *= 2;
			free(pPreprocess->token);
			pPreprocess->token = newbuf;
		}
		ch = verilog_preprocess_GetCh(pPreprocess);
		if (ch == 0)
			break;
		if (tokenstate == TOKEN_IN_STRING) {
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			if (ch == '"')
				tokenstate = TOKEN_NONE;
			if (ch == '\\')
				tokenstate = TOKEN_IN_STRING_SLASH;
		} else if (tokenstate == TOKEN_IN_STRING_SLASH) {
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			tokenstate = TOKEN_IN_STRING;
		} else if (tokenstate == TOKEN_COMMENT_START) {
			if (ch == '/') {
				tokenstate = TOKEN_IN_COMMENTLINE; 
			} else if (ch == '*') {
				ch = ' '; /* lastch=' ' */
				tokenstate = TOKEN_IN_COMMENT;
			} else {
				pPreprocess->token[pPreprocess->tokenlen++] = '/';
				pPreprocess->token[pPreprocess->tokenlen++] = ch;
			}
		} else if (tokenstate == TOKEN_IN_COMMENT) {
			if ( (ch == '/') && (lastch == '*') ) {
				tokenstate = TOKEN_NONE;
				pPreprocess->token[pPreprocess->tokenlen++] = ' ';
			}
		} else if (tokenstate == TOKEN_IN_COMMENTLINE) {
			if (ch == '\n') {
				tokenstate = TOKEN_NONE;
				pPreprocess->token[pPreprocess->tokenlen++] = ' ';
			}
		} else {
			if (ch == '(') {
				needright++;
				pPreprocess->token[pPreprocess->tokenlen++] = ch;
			} else if (ch == '/') {
				tokenstate = TOKEN_COMMENT_START;
			} else if (ch == '"') {
				tokenstate = TOKEN_IN_STRING;
				pPreprocess->token[pPreprocess->tokenlen++] = ch;
			} else if (ch == ')') {
				needright--;
				if (needright == 0)
					break;
				else 
					pPreprocess->token[pPreprocess->tokenlen++] = ch;
			} else if ( (ch == ',') && (needright == 1) ) {
				break;
			} else {
				pPreprocess->token[pPreprocess->tokenlen++] = ch;
			}
		}
		lastch = ch;
	} while (1);
	pPreprocess->token[pPreprocess->tokenlen] = 0;
	return ch;
}

static int verilog_preprocess_state_macro(sVerilogPreprocess * pPreprocess, int ch, int *pch)
{
	if (pPreprocess->state == STATE_CD_MACRO_PARAM) {
		if (ch == '(') {
			pPreprocess->state = STATE_CD_MACRO_PARAM_TEXT;
			pPreprocess->tokenlen = 0;
			DLIST_DESTROY(stringitem, &pPreprocess->macro_param_list);
		} else if (isblank(ch) || (ch == '\n') ) {
		} else if ( (ch == '/') && (pPreprocess->tokentype != TT_SYMBOL)) {
			pPreprocess->tokenstate = TOKEN_COMMENT_START;
			pPreprocess->lastch = '/';
		} else {
			verilog_preprocess_preprocess_PreAction(pPreprocess, PA_MACRO, pPreprocess->macro_name, "");
			pPreprocess->file_stack_top->ungetch = ch;
			pPreprocess->state = STATE_INITIAL;
			pPreprocess->tokenstate = TOKEN_NONE;
			pPreprocess->tokentype = TT_NONE;
		}
		return NEXT_CONTINUE;
	} else if (pPreprocess->state == STATE_CD_MACRO_PARAM_TEXT) {
		pPreprocess->file_stack_top->ungetch = ch;
		ch = verilog_preprocess_GetMacroParamText(pPreprocess);
		pPreprocess->token[pPreprocess->tokenlen] = 0;
		stringlistaddstring(&pPreprocess->macro_param_list, pPreprocess->token);
		if (ch == ')') {
			pPreprocess->state = STATE_INITIAL;
			pPreprocess->tokenstate = TOKEN_NONE;
			verilog_preprocess_preprocess_PreAction(pPreprocess, PA_MACRO, pPreprocess->macro_name, "");
		}
		return NEXT_CONTINUE;
	}
	return NEXT_NORMAL;
}

static int verilog_preprocess_setdefineparam(sVerilogPreprocess * pPreprocess)
{
	int buflen;
	int strsize;
	int ch;
	int tokenstate;
	int tokenlen;
	char * expandstr;
	char * token;
	char * definetext;
	if (stringlistcount(&pPreprocess->define_param_list) == 0)
		return 0;
	buflen = strlen(pPreprocess->define_text) * 2;
	if (buflen == 0) 
		return 0;
	expandstr = malloc(buflen);
	strsize = 0;
	tokenlen = 0;
	tokenstate = TOKEN_NONE;
	definetext = pPreprocess->define_text;
	ch = *definetext;
	while (1) {
		if (strsize == buflen) {
			char * temp = malloc(buflen * 2);
			memcpy(temp, expandstr, strsize);
			buflen *= 2;
			free(expandstr);
			expandstr = temp;
		}
		if (tokenstate == TOKEN_IN_SIMIDENT) {
			if (isident(ch)) {
				tokenlen++;
			} else {
				int index;
				int len;
				int i;
				char temp[40];
				int tempch = *definetext;
				*definetext = 0;
				if (stringlistfindstring(&pPreprocess->define_param_list, token, &index) != NULL) {
					sprintf(temp, "`%d", index);
					token = temp;
				}
				len = strlen(token);
				while (strsize + len + 2 >= buflen) {
					char * tempstr = malloc(buflen * 2);
					memcpy(tempstr, expandstr, strsize);
					buflen *= 2;
					free(expandstr);
					expandstr = tempstr;
				}
				expandstr[strsize++] = ' ';
				for (i = 0;i<len;i++) {
					expandstr[strsize++] = token[i];
				}
				expandstr[strsize++] = ' ';
				*definetext = tempch;
				expandstr[strsize++] = ch;
				if (ch == '"')
					tokenstate = TOKEN_IN_STRING;
				else
					tokenstate = TOKEN_NONE;
			}
		} else if (tokenstate == TOKEN_IN_STRING) {
			if (ch == '\\')
				tokenstate = TOKEN_IN_STRING_SLASH;
			else if (ch == '"')
				tokenstate = TOKEN_NONE;
			expandstr[strsize++] = ch;
		} else if (tokenstate == TOKEN_IN_STRING_SLASH) {
			tokenstate = TOKEN_IN_STRING;
			expandstr[strsize++] = ch;
		} else {
			if (isidentstart(ch)) {
				token = definetext;
				tokenstate = TOKEN_IN_SIMIDENT;
				tokenlen = 1;
			} else {
				expandstr[strsize++] = ch;
				if (ch == '"')
					tokenstate = TOKEN_IN_STRING;
			}
		}
		if (ch == 0)
			break;
		definetext++;
		ch = *definetext;
	}
	expandstr[strsize] = 0;
	free(pPreprocess->define_text);
	pPreprocess->define_text = expandstr;
	return 0;
}

static int verilog_preprocess_state_define(sVerilogPreprocess * pPreprocess, int ch, int *pch)
{
	if (pPreprocess->state == STATE_CD_DEFINE) {
		if (isidentstart(ch)) {
			pPreprocess->tokenlen = 0;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->state = STATE_CD_DEFINE_NAME;
			pPreprocess->tokenstate = TOKEN_IN_SIMIDENT;
		} else if (ch == '\\') {
			pPreprocess->tokenlen = 0;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->state = STATE_CD_DEFINE_NAME;
			pPreprocess->tokenstate = TOKEN_IN_ESCIDENT;
		} else if (isblank(ch) || (ch == '\n') ) {
		} else if ( (ch == '/') && (pPreprocess->tokentype != TT_SYMBOL)) {
			pPreprocess->tokenstate = TOKEN_COMMENT_START;
			pPreprocess->lastch = '/';
		} else {
			sprintf(logbuf, "%d,%d: Expect an ident\n", 
						pPreprocess->last_file_stack_top->lineno,
						pPreprocess->last_file_stack_top->linepos);
			stringlistaddstring(&pPreprocess->log_list, logbuf);
			pPreprocess->state = STATE_INITIAL;
			*pch = 0;
			pPreprocess->tokentype = TT_NONE;
			verilog_preprocess_error_to_eol(pPreprocess);
		}
		pPreprocess->tokentype = TT_NONE;
		return NEXT_CONTINUE;
	} else if (pPreprocess->state == STATE_CD_DEFINE_NAME) {
		if (pPreprocess->tokentype == TT_SIMIDENT ||
			pPreprocess->tokentype == TT_ESCIDENT) {
			FREE_MEMBER(define_name);
			pPreprocess->define_name = strdup(pPreprocess->token);
			DLIST_DESTROY(stringitem, &pPreprocess->define_param_list);
			pPreprocess->tokentype = TT_NONE;
		}
		if (isblank(ch)) {
		} else if (ch == '(') {
			pPreprocess->state = STATE_CD_DEFINE_PARAM_NAME;
		} else if (ch == '\n') {
			pPreprocess->state = STATE_INITIAL;
			verilog_preprocess_preprocess_PreAction(pPreprocess, PA_DEFINE, pPreprocess->define_name, "");
		} else if (ch == '\\') {
			pPreprocess->state = STATE_CD_DEFINE_TEXT;
			pPreprocess->tokenstate = TOKEN_IN_MACRO_TEXT_SLASH;
			pPreprocess->tokenlen = 0;
		} else {
			pPreprocess->state = STATE_CD_DEFINE_TEXT;
			pPreprocess->tokenstate = TOKEN_IN_MACRO_TEXT;
			pPreprocess->tokenlen = 0;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
		}
		return NEXT_CONTINUE;
	} else if (pPreprocess->state == STATE_CD_DEFINE_PARAM_NAME) {
		if (ch == ')') {
			pPreprocess->state = STATE_CD_DEFINE_TEXT;
			pPreprocess->tokenstate = TOKEN_IN_MACRO_TEXT;
			pPreprocess->tokenlen = 0;
		} else if (isidentstart(ch)) {
			pPreprocess->state = STATE_CD_DEFINE_PARAM_NAME_PRO;
			pPreprocess->tokenstate = TOKEN_IN_SIMIDENT;
			pPreprocess->tokenlen = 0;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
		} else if (isblank(ch) || (ch == '\n') ) {
		} else {
			sprintf(logbuf, "%d,%d: Expect an ident or ')'\n", 
						pPreprocess->last_file_stack_top->lineno,
						pPreprocess->last_file_stack_top->linepos);
			stringlistaddstring(&pPreprocess->log_list, logbuf);
			pPreprocess->state = STATE_INITIAL;
			*pch = 0;
			pPreprocess->tokentype = TT_NONE;
			verilog_preprocess_error_to_eol(pPreprocess);
		}
		return NEXT_CONTINUE;
	} else if (pPreprocess->state == STATE_CD_DEFINE_TEXT) {
		if (pPreprocess->tokentype == TT_MACRO_TEXT) {
			FREE_MEMBER(define_text);
			pPreprocess->define_text = strdup(pPreprocess->token);
			pPreprocess->state = STATE_INITIAL;
			verilog_preprocess_setdefineparam(pPreprocess);
			verilog_preprocess_preprocess_PreAction(pPreprocess, PA_DEFINE, pPreprocess->define_name, pPreprocess->define_text);
			verilog_preprocess_error_to_eol(pPreprocess);
		}
		return NEXT_CONTINUE;
	} else if (pPreprocess->state == STATE_CD_DEFINE_PARAM_NAME_PRO) {
		if (pPreprocess->tokentype == TT_SIMIDENT) {
			stringlistaddstring(&pPreprocess->define_param_list, pPreprocess->token);
			pPreprocess->tokentype = TT_NONE;
			pPreprocess->tokenstate = TOKEN_NONE;
		}
		if (ch == ',') {
			pPreprocess->state = STATE_CD_DEFINE_PARAM_NAME;
			pPreprocess->tokentype = TT_NONE;
			pPreprocess->tokenstate = TOKEN_NONE;
		} else if (ch == ')') {
			pPreprocess->state = STATE_CD_DEFINE_TEXT;
			pPreprocess->tokenstate = TOKEN_IN_MACRO_TEXT;
			pPreprocess->tokenlen = 0;
		} else if (isblank(ch) || (ch == '\n')) {
		} else if ( (ch == '/') && (pPreprocess->tokentype != TT_SYMBOL)) {
			pPreprocess->tokenstate = TOKEN_COMMENT_START;
			pPreprocess->lastch = '/';
		} else {
			sprintf(logbuf, "%d,%d: Expect a ',' or ')'\n", 
						pPreprocess->last_file_stack_top->lineno,
						pPreprocess->last_file_stack_top->linepos);
			stringlistaddstring(&pPreprocess->log_list, logbuf);
			pPreprocess->state = STATE_INITIAL;
			*pch = 0;
			pPreprocess->tokentype = TT_NONE;
			verilog_preprocess_error_to_eol(pPreprocess);
		}
		return NEXT_CONTINUE;
	}
	return 0;
}

static int verilog_preprocess_state_line(sVerilogPreprocess * pPreprocess, int ch, int *pch)
{
	if (pPreprocess->state == STATE_CD_LINE) {
		pPreprocess->tokentype = TT_NONE;
		if (isdigital(ch)) { /*macro parameter*/
			pPreprocess->tokenlen = 0;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->state = STATE_CD_LINE_NO;
			pPreprocess->tokenstate = TOKEN_IN_NUMBER;
		} else if (isblank(ch) || (ch == '\n') ) {

		} else if ( (ch == '/') && (pPreprocess->tokentype != TT_SYMBOL)) {
			pPreprocess->tokenstate = TOKEN_COMMENT_START;
			pPreprocess->lastch = '/';
		} else {
			sprintf(logbuf, "%d,%d: Expect number\n", 
						pPreprocess->last_file_stack_top->lineno,
						pPreprocess->last_file_stack_top->linepos);
			stringlistaddstring(&pPreprocess->log_list, logbuf);
			pPreprocess->state = STATE_INITIAL;
			*pch = 0;
			verilog_preprocess_error_to_eol(pPreprocess);
			return NEXT_RETURNCH;
		}
		return NEXT_CONTINUE;
	} else if (pPreprocess->state == STATE_CD_LINE_NO) {
		if (pPreprocess->tokentype == TT_NUMBER) {
			pPreprocess->line_no = atoi(pPreprocess->token);
			pPreprocess->state = STATE_CD_LINE_FILENAME;
			pPreprocess->tokentype = TT_NONE;
		}
		return 2;
	} else if (pPreprocess->state == STATE_CD_LINE_FILENAME) {
		if (pPreprocess->tokentype == TT_STRING) {
			pPreprocess->tokentype = TT_NONE;
			FREE_MEMBER(line_filename);
			pPreprocess->line_filename = strdup(pPreprocess->token);
			pPreprocess->state = STATE_CD_LINE_LEVEL;
		} else if (ch == '"') {
			pPreprocess->tokenlen = 0;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->tokenstate = TOKEN_IN_STRING;
		} else if (isblank(ch) || (ch == '\n') ) {
			return 2;
		} else {
			sprintf(logbuf, "%d,%d: Expect filename\n", 
						pPreprocess->last_file_stack_top->lineno,
						pPreprocess->last_file_stack_top->linepos);
			stringlistaddstring(&pPreprocess->log_list, logbuf);
			pPreprocess->state = STATE_INITIAL;
			*pch = 0;
			verilog_preprocess_error_to_eol(pPreprocess);
			return 1;
		}
		return 2;
	} else if (pPreprocess->state == STATE_CD_LINE_LEVEL) {
		if (pPreprocess->tokentype == TT_NUMBER) {
			pPreprocess->tokentype = TT_NONE;
			pPreprocess->line_level = atoi(pPreprocess->token);
			pPreprocess->state = STATE_INITIAL;
			PRINTF("[CD %d, %d]`line %d  %s %d\n", 
				pPreprocess->last_file_stack_top->lineno,
				pPreprocess->last_file_stack_top->linepos,
				pPreprocess->line_no, 
				pPreprocess->line_filename,
				pPreprocess->line_level);
		} else if (isdigital(ch)) {
			pPreprocess->tokenlen = 0;
			pPreprocess->token[pPreprocess->tokenlen++] = ch;
			pPreprocess->tokenstate = TOKEN_IN_NUMBER;
		} else if (isblank(ch) || (ch == '\n') ) {
			return 2;
		} else {
			sprintf(logbuf, "%d,%d: Expect level 0, 1 or 2\n", 
						pPreprocess->last_file_stack_top->lineno,
						pPreprocess->last_file_stack_top->linepos);
			stringlistaddstring(&pPreprocess->log_list, logbuf);
			pPreprocess->state = STATE_INITIAL;
			*pch = 0;
			verilog_preprocess_error_to_eol(pPreprocess);
			return 1;
		}
		return 2;
	}
	return 0;
}

static int verilog_preprocess_GetChar(sVerilogPreprocess * pPreprocess)
{	
#define state_do(st) \
{ \
	if (ret == 0) { \
		ret = verilog_preprocess_state_##st(pPreprocess, ch, &ch); \
		if (ret == 1) { \
			goto returnch; \
		} \
		if (ret == 2) { \
			continue; \
		} \
	} \
}

	int ch;
	do {
		int ret;
		ch = verilog_preprocess_GetCh(pPreprocess);
		if (ch == 0)
			goto returnch;
		ret = verilog_preprocess_GetToken(pPreprocess, ch, &ch);
		if (ret == NEXT_RETURNCH) {
			goto returnch;
		} else if (ret == NEXT_CONTINUE) {
			continue;
		}

		state_do(initial);
		state_do(cd);
		/* state_do(celldefine); */
		/* state_do(endcelldefine); */
		state_do(default_nettype);
		state_do(define);
		state_do(undef);
		state_do(ifdef);
		/*state_do(else);*/
		state_do(elseif);
		/*state_do(endif);*/
		state_do(ifndef);
		state_do(include);
		/*state_do(resetall);*/
		state_do(line);
		state_do(timescale);
		/*state_do(unconnected_drive);*/
		/*state_do(nounconnected_drive);*/
		state_do(pragma);
		state_do(begin_keywords);
		/*state_do(end_keywords);*/
		state_do(macro);

		pPreprocess->lastch = ch;
		pPreprocess->tokentype = TT_NONE;
		continue;
returnch:
		pPreprocess->lastch = ch;
		pPreprocess->tokentype = TT_NONE;
		if (pPreprocess->emitenabled) {
			return ch;
		}
	} while (ch != 0);
	return 0;
}

static int verilog_preprocess_preprocess_GetText(HOBJECT object, char * buf, int maxsize)
{
	int len;
	sVerilogPreprocess * pPreprocess;
	pPreprocess = (sVerilogPreprocess *)objectThis(object);
	for (len = 0;len<maxsize;len++) {
		int ch = verilog_preprocess_GetChar(pPreprocess);
		if (ch == 0)
			break;
		*buf++ = ch;
	}
	*buf = 0;
	return len;
}

static int verilog_preprocess_preprocess_GetLog(HOBJECT object, char * value, int vlen)
{
	sVerilogPreprocess * pPreprocess;
	pPreprocess = (sVerilogPreprocess *)objectThis(object);
	return getstringlist(&pPreprocess->log_list, '\n', value, vlen);
}

static int verilog_preprocess_preprocess_SymbolEmitEnabled(HOBJECT object)
{
	sVerilogPreprocess * pPreprocess;
	pPreprocess = (sVerilogPreprocess *)objectThis(object);
	return pPreprocess->emitenabled;
}

char * verilog_preprocess_expand_macro(sVerilogPreprocess * pPreprocess, char * macro)
{
	char * macroexpand = 0;
	int expandlen;
	int buflen;
	int macrolen;
	int i;
	int ch;
	int index;
	int state = 0;
	buflen = strlen(macro);
	if (buflen == 0)
		return NULL;
	macrolen = buflen;
	buflen ++;
	macroexpand = malloc(buflen);
	if (macroexpand == NULL)
		return NULL;
	expandlen = 0;
	for (i = 0;i<macrolen;i++) {
		while (expandlen > buflen-2) {
			char * temp;
			buflen *= 2;
			temp = malloc(buflen);
			if (temp == NULL) {
				free(macroexpand);
				return NULL;
			}
			memcpy(temp, macroexpand, expandlen);
			free(macroexpand);
			macroexpand = temp;
		}
		ch = *macro++;
		if (ch == '`') {
			state = 1;
		} else if (state == 1) {
			if (isdigit(ch)) {
				index = ch - '0';
				state = 2;
			} else {
				macroexpand[expandlen++] = '`';
				macroexpand[expandlen++] = ch;
				state = 0;
			}
		} else if (state == 2) {
			if (isdigit(ch)) {
				index = index * 10 + ch - '0';
			} else {
				int ind;
				char * param = NULL;
				state = 0;
				ind = 0;
				DLIST_EVERY_ITEM_BEGIN(stringitem, &pPreprocess->macro_param_list, pitem);
				if (ind == index) {
					param = pitem->string;
					break;
				}
				ind++;
				DLIST_EVERY_ITEM_END();
				if (param != NULL) {
					int len;
					len = strlen(param);
					while (expandlen + len >= buflen-1) {
						char * temp;
						buflen *= 2;
						temp = malloc(buflen);
						if (temp == NULL) {
							free(macroexpand);
							return NULL;
						}
						memcpy(temp, macroexpand, expandlen);
						free(macroexpand);
						macroexpand = temp;
					}
					while (*param) {
						macroexpand[expandlen++] = *param++;
					}
				}
				macroexpand[expandlen++] = ch;
				state = 0;
			}
		} else {
			macroexpand[expandlen++] = ch;
		}
	}
	macroexpand[expandlen] = 0;
	return macroexpand;
}

static int verilog_preprocess_preprocess_PreAction(HOBJECT object, int action, const char * name, const char * value)
{
	sVerilogPreprocess * pPreprocess;
	pPreprocess = (sVerilogPreprocess *)objectThis(object);
	switch (action) {
	case PA_INCLUDE:
		PRINTF("[CD] include %s\n", name);
		if (pPreprocess->emitenabled == 0)
			return 0;
		return verilog_preprocess_preprocess_IncludeFile(object, name);
		break;
	case PA_UNDEF:
		PRINTF("[CD] undef %s\n", name);
		if (pPreprocess->emitenabled == 0) {
			return 0;
		} else {
			stringlistremovekey(&pPreprocess->macro_list, name);
		}
		break;
	case PA_DEFINE:
		PRINTF("[CD] define(%d) %s %s\n", stringlistcount(&pPreprocess->define_param_list), name, value);
		if (pPreprocess->emitenabled == 0)
			return 0;
		return stringlistaddstringpair(&pPreprocess->macro_list, name, value);
		break;
	case PA_MACRO:
		if (pPreprocess->emitenabled == 0) {
			return 0;
		} else {
			stringitem * pItem;
			char * macroexpand;
			pItem = stringlistfindkey(&pPreprocess->macro_list, name);
			if (pItem == NULL)
				return -1;
			macroexpand = verilog_preprocess_expand_macro(pPreprocess, &pItem->string[strlen(name)+1]);
			if (macroexpand != NULL) {
				pPreprocess->file_stack_top = filestack_push_string(&pPreprocess->file_stack, macroexpand, 0, 0, 0);	
				free(macroexpand);
			}
		}
		break;
	case PA_IFDEF:
	case PA_IFNDEF:
		PRINTF("[CD] ifdef/ifndef %s\n", name);
		IFSTACK_PUSH();
		if (pPreprocess->emitenabled) {
			stringitem * pItem;
			pItem = stringlistfindkey(&pPreprocess->macro_list, name);
			if (pItem == NULL)
				pPreprocess->emitenabled = (action==PA_IFDEF)?0:1;
			else
				pPreprocess->emitenabled = (action==PA_IFDEF)?1:0;
			pPreprocess->waitendif = 0;
		} else {
			pPreprocess->waitendif++;
		}
		break;
	case PA_ELSE:
		PRINTF("[CD] else %s\n", name);
		if (pPreprocess->emitenabled) {
			pPreprocess->emitenabled = 0;
		} else {
			if (pPreprocess->waitendif == 0)
				pPreprocess->emitenabled = 1;
		}
		break;
	case PA_ENDIF:
		PRINTF("[CD] endif %s\n", name);
		if (pPreprocess->emitenabled == 0) {
			pPreprocess->waitendif--;
		}
		IFSTACK_POP();
		break;
	case PA_ELSEIFDEF:
		PRINTF("[CD] elseifdef %s\n", name);
		if (pPreprocess->emitenabled) {
			stringitem * pItem;
			pItem = stringlistfindkey(&pPreprocess->macro_list, name);
			if (pItem == NULL)
				pPreprocess->emitenabled = 0;
			else
				pPreprocess->emitenabled = 1;
			pPreprocess->waitendif = 0;
		}
		break;
	}
	return 0;
}

int preprocessVerilogCreate(IPreprocess ***pProcess)
{
	int ret;
	A_u_t_o_registor_verilog_preprocess();
	ret = objectCreateEx(CLSID_PREPROCESS_VERILOG, NULL, 0, IID_PREPROCESS, (const void **)pProcess);
	return ret;
}

