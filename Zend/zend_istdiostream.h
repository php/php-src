/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifndef _ZEND_STDIOSTREAM
#define _ZEND_STDIOSTREAM

#if defined(ZTS) && !defined(HAVE_CLASS_ISTDIOSTREAM)
class istdiostream : public istream
{
private:
	stdiobuf _file;
public:
	istdiostream (FILE* __f) : istream(), _file(__f) { init(&_file); }
	stdiobuf* rdbuf()/* const */ { return &_file; }
};
#endif

#endif
