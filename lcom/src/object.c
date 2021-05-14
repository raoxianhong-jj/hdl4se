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

#define MAXCLASSES						64

#define IMPLEMENT_GUID
#include "../include/object.h"
#undef IMPLEMENT_GUID

#define DEBUGLINE	do{ printf("%s, %d\n", __FILE__, __LINE__); gljosTaskDelay(1); } while (0)

/*
我们用一个链表实现对象类注册表
*/

typedef struct _objectCreateItem
{
	struct _objectCreateItem * pNext;
	IIDTYPE clsid;
	FObjectCreate objectcreator;
	FModuleVersionInfo moduleinfo;
}objectCreateItem;

static objectCreateItem objectclsheader = {0};
static objectCreateItem objectItems[MAXCLASSES];
static int objectIndex = 0;
#define ALLOCITEM(itemptr) \
do { \
	if (objectIndex < MAXCLASSES) \
		itemptr = &objectItems[objectIndex++]; \
	else \
		itemptr = 0; \
} while (0)

#if defined(__linux__) || defined(WIN32)

static int objectRegisterAllClass()
{
	return -1;
}
#include "stdio.h"
#define logMsg printf
#else
#ifdef _VXWORKS_
#include "loadLib.h"
#include "symLib.h"
extern SYMTAB_ID sysSymTbl;

static BOOL registerSym
        (
        char      *name,  /* entry name                  */
        int       val,    /* value associated with entry */
        SYM_TYPE  type,   /* entry type                  */
        int       arg,    /* arbitrary user-supplied arg */
        UINT16    group   /* group number                */
        )
{
	if (memcmp("A_u_t_o_registor_", name, 17) == 0) {
			/*if (type == (SYM_GLOBAL | SYM_DATA) ) */{
				(*(FUNCPTR*)val)();
				printf("Call %s\n", name);
			}
		}	
	return 1;
}

static int objectRegisterAllClass()
{
	symEach(sysSymTbl, (FUNCPTR)registerSym, 0);
#if INCLUDE_PRINTF	
	printf("Registered modules:\n");
#endif
	objectPrintInfo(0, 0);
	return 0;
}
#else
static int objectRegisterAllClass()
{
	int i;
	int start;
	start = 0;
	for (i = 0;i<standTblSize;i++) {
		if (memcmp("A_u_t_o_registor_", standTbl[i].name, 17) == 0) {
			start = 1;
			if (standTbl[i].type == (SYM_GLOBAL | SYM_DATA) ) {
				(*(FUNCPTR*)standTbl[i].value)();
			}
		}	else if (start) {
			break; /* 假定standTbl表是按照名字排序了的，所有的类注册函数都连续排在一起 */
		}
	}
}
#endif

#endif 
/*
	对象类管理模块初始化
	功能：初始化内部资源，这个函数只在初始化时调用一次
	参数：无
	返回值：
	   EIID_OK -- 成功
*/

static int objectRegisterInited = 0;

int objectRegisterInit()
{
	if (objectRegisterInited)
		return EIID_OK;
	objectIndex = 0;
	objectRegisterInited = 1;
	objectclsheader.pNext = 0;
	objectRegisterAllClass();
	return EIID_OK;
}

/*
	对象类注册
	功能：注册一个对象类到对象生成支持模块中
	参数：clsid -- 对象类编号
	      objectCreator -- 对象生成函数
	      moduleinfo -- 对象版本信息函数
	返回值：
	   EIID_OK -- 成功
	   EIID_INVALIDPARAM -- 参数非法
	   EIID_MEMORYNOTENOUGH -- 内存不足	  
	   EIID_GENERALERROR -- 该对象类已经注册
*/
int objectCreateRegister(IIDTYPE clsid, FObjectCreate objectCreator, FModuleVersionInfo moduleinfo)
{
	objectCreateItem * pItem;
	FObjectCreate temp;
	objectRegisterInit();
	if (NULL == objectCreate) { /* 生成函数不能为空 */
		return EIID_INVALIDPARAM;
	}
	if (clsid == IID_OBJECT) { /* OBJECT为虚类，不能重定义 */
		return EIID_INVALIDPARAM;
	}

	/* 看该类是否已经注册 */
	if (EIID_OK == objectQueryCreator(clsid, &temp))
		return EIID_GENERALERROR;
		
	/* 生成一个对象类 */
	if (objectIndex >= MAXCLASSES) 
		return EIID_MEMORYNOTENOUGH;
		
	pItem = &objectItems[objectIndex++];
		
	/* 连接到类表中 */
	pItem->clsid = clsid;
	pItem->objectcreator = objectCreator;
	pItem->moduleinfo = moduleinfo;
	pItem->pNext = objectclsheader.pNext;
	objectclsheader.pNext = pItem;
	
	return EIID_OK;
}

/*
	功能：打印所有注册的对象实现模块的版本信息
	参数：data -- 传到funcoutput函数的参数，
	      funcoutput -- 字符串输出函数，如果为0，则调用printf输出到标准输出上。
	返回值：无
*/

int objectPrintInfo(PTR data, OFUNCPTR funcoutput)
{
	int ret;
	objectCreateItem * pItem;
	objectRegisterInit();
	if (NULL == funcoutput) {
#if INCLUDE_PRINTF		
		data = (PTR)stdout;
		funcoutput = (OFUNCPTR)fprintf;
#else
		return EIID_GENERALERROR;
#endif
	}
	funcoutput(data, "  %8s %35s  MODULE INFO\n", "DRIVER", "CLSID               ");
	/* 遍历注册链表输出各个模块的信息 */
	pItem = objectclsheader.pNext;
	ret = 0;
	while (pItem != 0) {
		const char * moduleinfo;
		moduleinfo = 0;
		if (pItem->moduleinfo)
			moduleinfo = pItem->moduleinfo();
		if (NULL == moduleinfo) {
			moduleinfo = "No module info";
		}
		funcoutput(data, "  %08x %08x-%04x-%04x-%08x%08x  %s\n", 
				pItem->objectcreator, 
				pItem->clsid[0], 
				pItem->clsid[1] & 0xffff, 
				pItem->clsid[1] >> 16, 
				pItem->clsid[2], 
				pItem->clsid[3], 
				moduleinfo);
		ret++;
		pItem = pItem->pNext;
	}	
	return ret;
}

/*
	对象类注销
	功能：注销所有的注册类，应用程序结束时应调用此例程以释放资源
	参数：
	返回值：
	   EIID_OK -- 成功
*/
int objectCreateUnRegisterAll()
{
	return EIID_OK;
}

/*
	对象生成
	功能：查询已经注册的对象类的对象生成函数，生成对象
	参数：clsid -- 对象类编号
	      pParams -- 对象生成参数表，如果该类已经注册，将传到注册的对象生成函数中
	      paramcount -- 对象生成参数个数
	      pObject -- 存放返回的对象句柄的指针
  返回值：
     EIID_OK -- 成功
     EIID_INVALIDPARAM -- 参数非法
     EIID_MEMORYNOTENOUGH -- 内存不足
     EIID_GENERALERROR -- 对象生成出错
     EIID_INVALIDCLSID -- 该类没有注册
*/
int objectCreate(IIDTYPE clsid, const PARAMITEM * pParams, int paramcount, HOBJECT * pObject)
{
	FObjectCreate objectCreator;
	objectRegisterInit();
	/* 看该类是否已经注册 */
	if (EIID_OK != objectQueryCreator(clsid, &objectCreator)) {
#if INCLUDE_PRINTF
		/* 这应该是某个模块没有连接到系统中，一般不应该出现这各问题，我们打印出来供调试用 */
#endif		
		return EIID_INVALIDCLSID;
	}

	return objectCreator(pParams, paramcount, pObject);
}

int objectCreateEx(IIDTYPE clsid, const PARAMITEM * pParams, int paramcount, IIDTYPE iid, const void **pInterface)
{
	int ret;
	HOBJECT obj;
	ret = objectCreate(clsid, pParams, paramcount, &obj);
	if (ret != EIID_OK)
		return ret;
	ret = objectQueryInterface(obj, iid, pInterface);
	objectRelease(obj);
	return ret;
}

/*
  查询对象生成函数
  功能：返回指定类编号的对象生成函数
  参数：clsid -- 对象类编号
        pOBjectCreator -- 存放对象生成函数的指针
  返回值：
      EIID_OK -- 成功
      EIID_INVALIDPARAM -- 参数非法
      EIID_INVALIDCLSID -- 该类没有注册
*/
int objectQueryCreator(IIDTYPE clsid, FObjectCreate * pObjectCreator)
{
	objectCreateItem * pItem;
	objectRegisterInit();
	if (pObjectCreator == 0)
		return EIID_INVALIDPARAM;
	
	/* 在注册链表中查询对象类 */
	pItem = objectclsheader.pNext;
	while (pItem != 0) {
		if (isGUIDEqual(pItem->clsid, clsid)) {
			*pObjectCreator = pItem->objectcreator;
			return EIID_OK;
		}
		pItem = pItem->pNext;
	}
	return EIID_INVALIDCLSID;
}

#ifdef _DEBUG
void _Assert(const char * pszExpr, const char *pszFileName, unsigned int nLine)
{
	logMsg("Assertion Failed: \"%s\"\nFile=%s, Line=%d\n", pszExpr, pszFileName, nLine);
}
#endif
