/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Wez Furlong <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

/* IClassFactory Implementation */

#include <unknwn.h>

// {A0AD8E7A-95EC-4819-986F-78D93895F2AE}
DEFINE_GUID(CLSID_PHPActiveScriptEngine, 
0xa0ad8e7a, 0x95ec, 0x4819, 0x98, 0x6f, 0x78, 0xd9, 0x38, 0x95, 0xf2, 0xae);

class TPHPClassFactory:
	public IClassFactory
{
protected:
	volatile LONG m_refcount;

	static volatile LONG factory_count;
	static volatile LONG object_count;
	
public: /* IUnknown */
	STDMETHODIMP QueryInterface(REFIID iid, void **ppvObject);
	STDMETHODIMP_(DWORD) AddRef(void);
	STDMETHODIMP_(DWORD) Release(void);
public: /* IClassFactory */
	STDMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID iid, void **ppvObject);
	STDMETHODIMP LockServer(BOOL fLock);

	TPHPClassFactory();
	~TPHPClassFactory();

	static void AddToObjectCount(void);
	static void RemoveFromObjectCount(void);
	static int CanUnload(void);
};

