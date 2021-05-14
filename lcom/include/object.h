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


/* 基本对象接口的标识符 */

#ifndef __OBJECT_H
#define __OBJECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdlib.h"

typedef unsigned int  Ouint32_t;
typedef unsigned char Ouint8_t;
typedef void * PTR;

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef objectERROR
#define objectERROR (-1)
#endif

#ifndef OK
#define OK 0
#endif

typedef int (*OFUNCPTR)();
typedef void (*OVOIDFUNCPTR)();

#include "guid.h"

#ifndef _ASMLANGUAGE

typedef const Ouint32_t* IIDTYPE;


/* 判断GUID相等时，应比较它们的16字节是否一致 */
#define isGUIDEqual(n1, n2) (((n1)[0] == (n2)[0]) \
                          && ((n1)[1] == (n2)[1]) \
                          && ((n1)[2] == (n2)[2]) \
                          && ((n1)[3] == (n2)[3]))

DEFINE_GUID(IID_OBJECT, 0xae6e4041, 0x3353, 0x4fd7, 0x9e, 0xce, 0xc8, 0x8b, 0xe, 0x3a, 0xb9, 0x7c)

#include "eiid.h"

typedef void * HOBJECT;
struct sIObject;
typedef const struct sIObject * HINTERFACE;

/*************************** 断言 ******************************/
#ifndef ASSERT
#ifdef _DEBUG
void _Assert(const char * pszExpr, const char *pszFileName, unsigned int nLine);
#define ASSERT(f) \
	do { \
		if ( 0 == (f) ) { \
			_Assert(#f, __FILE__ , __LINE__); \
		} \
	} while (0)
#define VERIFY(f)          ASSERT(f)
#else
#define ASSERT(f)          ((void)0)
#define VERIFY(f)          ((void)(f))
#endif
#endif

/**************************************************************/

typedef struct sIObject {
	int __thisoffset;
/*
	功能：对象是否支持指定的接口
	参数：
	    object -- 对象数据指针
	    iid -- 接口编号
	    pInterface -- 存放返回的接口指针
	返回值：
	    EIID_OK: 成功
	    EIID_INVALIDOBJECT：object 是无效对象
	    EIID_INVALIDPARAM: 参数错误
	    EIID_UNSUPPORTEDINTERFACE : 对象不支持该接口
*/	
	int (*QueryInterface)(HOBJECT object, IIDTYPE iid, const void **pInterface);
/*
	功能：增加对象的引用计数
	参数：
	    object -- 对象数据指针
	返回值：
	    >=0: 增加后的引用计数 
	    EIID_INVALIDOBJECT：object 是无效对象
*/		
	int (*AddRef)(HOBJECT object); 
/*
	功能：减少对象的引用计数，引用计数为0时删除该对象
	参数：
	    object -- 对象数据指针
	返回值：
	    >= 0: 减少后的引用计数
	    EIID_INVALIDOBJECT：object 是无效对象
*/		
	int (*Release)(HOBJECT object);
/*
	功能：判断对象是否是一个有效对象
	参数：
	    object -- 对象数据指针
	返回值：
	    0 -- 对象是无效的
	    1 -- 对象是有效的
*/	
	int (*IsValid)(HOBJECT object);
}IObject;

/*
	每个实现的对象数据中，应该使用下面的OBJECT_HEADER定义前四个数据变量。
	其中
		    __object_refcount是对象的引用计数，一个对象生成时，引用计数为1，
	          使用AddRef调用可以增加对象引用计数，使用Release调用可以减少
	          引用计数，当引用计数为零时，会自动调用对象实现的Destroy函数，
	          释放对象占用的资源。
	并初始化成该对象的object接口表指针
*/
#define OBJECT_HEADER   \
	const IObject * __IObject_ptr; \
	int __object_refcount; \
	IIDTYPE __object_clsid; \

#define OBJECT_INTERFACE  \
	int __thisoffset; \
	int (*QueryInterface)(HOBJECT object, IIDTYPE iid, const void **pInterface); \
	int (*AddRef)(HOBJECT object); \
	int (*Release)(HOBJECT object); \
	int (*IsValid)(HOBJECT object); \

typedef struct _sObjectHeader {
	OBJECT_HEADER
}sObjectHeader;

#define objectThis(_obj) (((Ouint8_t *)(_obj))-((*(IObject **)(_obj))->__thisoffset))

/*
	调用对象接口的函数
*/
#if !defined(_MSC_VER)
/*VC 6.0以下不支持args...*/
#define objectCallReturn(_obj, _func, _ret, args...) ( objectIsValid(_obj) ? ((*_obj))->_func((HOBJECT)(_obj), ##args) : (_ret) )
#endif

#define objectCallVoidReturn(_obj, _func, _ret) ( objectIsValid(_obj) ? ((*_obj))->_func((HOBJECT)(_obj)) : (_ret) )

#if !defined(_MSC_VER)
#define objectCall(_obj, _func, args...) ( objectIsValid(_obj) ? ((*_obj))->_func((HOBJECT)(_obj), ##args) : (objectERROR) )
#endif
#define objectCall0(_obj, _func) ( objectIsValid(_obj) ? \
	((*_obj))->_func((HOBJECT)(_obj)) : (objectERROR) )
#define objectCall1(_obj, _func, arg0) ( objectIsValid(_obj) ? \
	((*_obj))->_func((HOBJECT)(_obj), arg0) : (objectERROR) )
#define objectCall2(_obj, _func, arg0, arg1) ( objectIsValid(_obj) ? \
	((*_obj))->_func((HOBJECT)(_obj), arg0, arg1) : (objectERROR) )
#define objectCall3(_obj, _func, arg0, arg1, arg2) ( objectIsValid(_obj) ? \
	((*_obj))->_func((HOBJECT)(_obj), arg0, arg1, arg2) : (objectERROR) )
#define objectCall4(_obj, _func, arg0, arg1, arg2, arg3) ( objectIsValid(_obj) ? \
	((*_obj))->_func((HOBJECT)(_obj), arg0, arg1, arg2, arg3) : (objectERROR) )
#define objectCall5(_obj, _func, arg0, arg1, arg2, arg3, arg4) ( objectIsValid(_obj) ? \
	((*_obj))->_func((HOBJECT)(_obj), arg0, arg1, arg2, arg3, arg4) : (objectERROR) )
#define objectCall6(_obj, _func, arg0, arg1, arg2, arg3, arg4, arg5) ( objectIsValid(_obj) ? \
	((*_obj))->_func((HOBJECT)(_obj), arg0, arg1, arg2, arg3, arg4, arg5) : (objectERROR) )
#define objectCall7(_obj, _func, arg0, arg1, arg2, arg3, arg4, arg5, arg6) ( objectIsValid(_obj) ? \
	((*_obj))->_func((HOBJECT)(_obj), arg0, arg1, arg2, arg3, arg4, arg5, arg6) : (objectERROR) )
#define objectCall8(_obj, _func, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7) ( objectIsValid(_obj) ? \
	((*_obj))->_func((HOBJECT)(_obj), arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7) : (objectERROR) )
#define objectCall9(_obj, _func, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) ( objectIsValid(_obj) ? \
	((*_obj))->_func((HOBJECT)(_obj), arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) : (objectERROR) )
#define objectCall10(_obj, _func, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) ( objectIsValid(_obj) ? \
	((*_obj))->_func((HOBJECT)(_obj), arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) : (objectERROR) )

#define objectCallVoid(_obj, _func) ( objectIsValid(_obj) ? ((*_obj))->_func((HOBJECT)(_obj)) : (objectERROR) )


/* 下面的宏简化对象基本接口的调用 */

#define objectPtrValid(object) ( (NULL != (object)) && (NULL != (*(void **)(object))) )
#define objectIsClass(object, classid) (objectPtrValid(object) \
                                     && isGUIDEqual(((sObjectHeader *)(objectThis(object)))->__object_clsid, classid) )
#define objectQueryInterface(object, iid, pInterface) \
		(objectPtrValid(object)? \
		 (*(IObject **)(object))->QueryInterface((HOBJECT)(object), (iid), pInterface): \
		 	EIID_INVALIDOBJECT)
#define objectAddRef(object) (objectPtrValid(object)? \
                              (*(IObject **)(object))->AddRef((HOBJECT)(object)): \
                              	EIID_INVALIDOBJECT) 
#define objectRelease(object) (objectPtrValid(object)? \
                              (*(IObject **)(object))->Release((HOBJECT)(object)): \
                              EIID_INVALIDOBJECT)
#define objectIsValid(object) (objectPtrValid(object)? \
                               (*(IObject **)(object))->IsValid((HOBJECT)(object)): \
                               	0)

/* 下面是对象生成的一致性实现支持 */
typedef struct _sParamItem {
	IIDTYPE				name; /* 参数编号，由对象实现规定每个编号的含义，对象实现文档中应该说明对象生成参数表的详细内容 */
	Ouint32_t     		i32value; /* 32位一般参数 */
	double				fvalue; /* 浮点参数 */
	void *				pvalue; 
}PARAMITEM;

/*
	对象生成函数
	功能：对象实现模块生成一个新的对象
	参数：pParams -- 对象生成参数表，当paramcount>0是，pParams必须不为NULL
	      paramcount -- 参数表中的参数个数，必须不小于零，为零时，pParams可以为NULL
	      pObject -- 存放返回的对象句柄的指针，不能为NULL
  返回值：
     EIID_OK -- 成功
     EIID_INVALIDPARAM -- 参数无效
     EIID_MEMORYNOTENOUGH -- 内存不足
     EIID_GENERALERROR -- 其他错误
*/
typedef int (*FObjectCreate)(const PARAMITEM * pParams, int paramcount, HOBJECT * pObject);

/*
	对象信息函数
	功能：返回对象实现模块的版本信息
	参数：无
  返回值：
     对象实现模块的版本信息
*/
typedef const char * (*FModuleVersionInfo)();


/*
	对象类管理模块初始化
	功能：初始化内部资源，这个函数只在初始化时调用一次
	参数：
	返回值：
	   EIID_OK -- 成功
*/

int objectRegisterInit();

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
int objectCreateRegister(IIDTYPE clsid, FObjectCreate objectCreator, FModuleVersionInfo moduleinfo);

/*
	功能：打印所有注册的对象实现模块的版本信息
	参数：data -- 传到funcoutput函数的第一个参数，
	      funcoutput -- 字符串输出函数，如果为0，则调用printf输出到标准输出上。
	返回值：
*/
int objectPrintInfo(PTR data, OFUNCPTR funcoutput);

/*
	对象类注销
	功能：注销所有的注册类，应用程序结束时应调用此例程以释放资源
	参数：
	返回值：
	   EIID_OK -- 成功
*/
int objectCreateUnRegisterAll(void);

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
int objectCreate(IIDTYPE clsid, const PARAMITEM * pParams, int paramcount, HOBJECT * pObject);
int objectCreateEx(IIDTYPE clsid, const PARAMITEM * pParams, int paramcount, IIDTYPE iid, const void **pInterface);

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
int objectQueryCreator(IIDTYPE clsid, FObjectCreate * pObjectCreator);

/* 
  以下代码辅助实现一个Object 
*/

#define INTERFACE_VAR(__interface) __##__interface##_ptr
#define INTERFACE_DECLARE(__interface) \
 const __interface * INTERFACE_VAR(__interface);
#define INTERFACE_INIT(__interface, objptr, _obj, interfacename) \
    objptr->INTERFACE_VAR(__interface) = &_obj##_##interfacename##_interface;

#define OBJECT_FUNCDECLARE(_obj, _clsid)\
	static int _obj##QueryInterface(HOBJECT object, IIDTYPE iid, const void **pInterface); \
	static int _obj##AddRef(HOBJECT object); \
	static int _obj##Release(HOBJECT object); \
	static int _obj##IsValid(HOBJECT object); \
	static void _obj##Destroy(HOBJECT object); \
	static int _obj##Create(const PARAMITEM * pParams, int paramcount, HOBJECT* pObject); \
	static int _obj##Valid(HOBJECT object);\
	static const IObject _obj##_object_interface = { \
		0, \
		_obj##QueryInterface, \
		_obj##AddRef, \
		_obj##Release, \
		_obj##IsValid \
	}; \
	static const char * _obj##ModuleInfo();\
	static int _obj##Register() {\
		return objectCreateRegister(_clsid, _obj##Create, _obj##ModuleInfo); \
	}\
	OFUNCPTR A_u_t_o_registor_##_obj = (OFUNCPTR)_obj##Register;
	
#define INTERFACE_HEADER(_obj, __interface, _localstruct) \
		(int)&(((const _localstruct*)0)->__##__interface##_ptr), \
		_obj##QueryInterface, \
		_obj##AddRef, \
		_obj##Release, \
		_obj##IsValid,

#define QUERYINTERFACE_BEGIN(_obj, _clsid) \
static int _obj##QueryInterface(HOBJECT object, IIDTYPE iid, const void **pInterface) \
{ \
	if (!objectIsClass(object, _clsid)) \
		return EIID_INVALIDOBJECT; \
  if (!objectIsValid(object)) \
		return EIID_INVALIDOBJECT; \
	if (pInterface == 0) \
		return EIID_INVALIDPARAM; \
	*pInterface = 0; \
	if (isGUIDEqual(iid ,IID_OBJECT)) { \
		*pInterface = objectThis(object); \
		objectAddRef(object); \
		return EIID_OK; \
	}

#define QUERYINTERFACE_ITEM(_iid, __interface, _localstruct) \
	else if (isGUIDEqual(iid, _iid)) { \
		*pInterface = &((_localstruct *)(objectThis(object)))->__##__interface##_ptr; \
		objectAddRef(object); \
		return EIID_OK; \
	} \

#define QUERYINTERFACE_END \
	return EIID_UNSUPPORTEDINTERFACE; \
}

#define OBJECT_RETURN_GEN(_obj, _objptr, _retvar, _sid) \
do \
{ \
	_objptr->__IObject_ptr = &_obj##_object_interface; \
	_objptr->__object_refcount = 1; \
	_objptr->__object_clsid = _sid;	\
	*_retvar = (HOBJECT)&_objptr->__IObject_ptr; \
}while (0)

#define OBJECT_FUNCIMPL(_obj, _localstruct, _sid) \
static int _obj##AddRef(HOBJECT object) \
{ \
	_localstruct * pD; \
  if (!objectIsValid(object)) \
		return EIID_INVALIDOBJECT; \
  pD = (_localstruct *)objectThis(object); \
	pD->__object_refcount++; \
	return pD->__object_refcount; \
} \
 \
static int _obj##Release(HOBJECT object) \
{ \
	_localstruct * pD; \
	int ret; \
  if (!objectIsValid(object)) \
		return EIID_INVALIDOBJECT; \
  pD = (_localstruct *)objectThis(object); \
	pD->__object_refcount--; \
	ret = pD->__object_refcount; \
	if (pD->__object_refcount <= 0) { \
	  pD->__object_refcount = 1; /* 为了保证在Destroy过程中不出现递归调用，这里将引用记数设置为1 */ \
		_obj##Destroy(object); \
	} \
	return ret; \
} \
\
static int _obj##IsValid(HOBJECT object) \
{ \
	_localstruct * pD; \
	if (object == 0) \
		return 0; \
    pD = (_localstruct *)objectThis(object); \
	if (pD->__object_clsid != _sid) \
		return 0; \
	if (pD->__object_refcount < 1) \
		return 0; \
	return _obj##Valid(object); \
}

#endif /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif
