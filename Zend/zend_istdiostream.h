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
