/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <math.h>
#include <unicode/msgfmt.h>
#include <unicode/chariter.h>

extern "C" {
#include "php_intl.h"
#include "msgformat_class.h"
#include "msgformat_format.h"
#include "msgformat_helpers.h"
#include "intl_convert.h"
}

U_NAMESPACE_BEGIN
/**
 * This class isolates our access to private internal methods of
 * MessageFormat.  It is never instantiated; it exists only for C++
 * access management.
 */
class MessageFormatAdapter {
public:
    static const Formattable::Type* getArgTypeList(const MessageFormat& m,
                                                   int32_t& count);
};
const Formattable::Type*
MessageFormatAdapter::getArgTypeList(const MessageFormat& m,
                                     int32_t& count) {
    return m.getArgTypeList(count);
}
U_NAMESPACE_END

U_CFUNC int32_t umsg_format_arg_count(UMessageFormat *fmt) 
{
	int32_t fmt_count = 0;
	MessageFormatAdapter::getArgTypeList(*(const MessageFormat*)fmt, fmt_count);
	return fmt_count;
}

U_CFUNC void umsg_format_helper(UMessageFormat *fmt, int arg_count, zval **args, UChar **formatted, int *formatted_len, UErrorCode *status TSRMLS_DC)
{
	int fmt_count = 0;
    const Formattable::Type* argTypes =
		MessageFormatAdapter::getArgTypeList(*(const MessageFormat*)fmt, fmt_count);
	Formattable* fargs = new Formattable[fmt_count ? fmt_count : 1];

	for(int32_t i = 0; i < fmt_count; ++i) {
        UChar  *stringVal = NULL;
		int     stringLen = 0;
		int64_t tInt64 = 0;

		switch(argTypes[i]) {
			case Formattable::kDate:
				convert_to_long_ex(&args[i]);
				fargs[i].setDate(U_MILLIS_PER_SECOND * (double)Z_LVAL_P(args[i]));
				break;

			case Formattable::kDouble:
				convert_to_double_ex(&args[i]);
			    fargs[i].setDouble(Z_DVAL_P(args[i]));
				break;
            
	        case Formattable::kLong:
				convert_to_long_ex(&args[i]);
			    fargs[i].setLong(Z_LVAL_P(args[i]));
				break;

	        case Formattable::kInt64:
				if(Z_TYPE_P(args[i]) == IS_DOUBLE) {
					tInt64 = (int64_t)Z_DVAL_P(args[i]);
				} else if(Z_TYPE_P(args[i]) == IS_LONG) {
					tInt64 = (int64_t)Z_LVAL_P(args[i]);
				} else {
					SEPARATE_ZVAL_IF_NOT_REF(&args[i]);
					convert_scalar_to_number( args[i] TSRMLS_CC );
					tInt64 = (Z_TYPE_P(args[i]) == IS_DOUBLE)?(int64_t)Z_DVAL_P(args[i]):Z_LVAL_P(args[i]);
				}
			    fargs[i].setInt64(tInt64);
				break;
            
	        case Formattable::kString:
		        convert_to_string_ex(&args[i]);
				intl_convert_utf8_to_utf16(&stringVal, &stringLen, Z_STRVAL_P(args[i]), Z_STRLEN_P(args[i]), status);
				if(U_FAILURE(*status)){
					delete[] fargs;
					return;
				}
				fargs[i].setString(stringVal);
				efree(stringVal);
			    break;
            
			case Formattable::kArray:
			case Formattable::kObject:
				*status = U_UNSUPPORTED_ERROR;
				delete[] fargs;
				return;
        }		
	}

    UnicodeString resultStr;
    FieldPosition fieldPosition(0);
    
    /* format the message */
    ((const MessageFormat*)fmt)->format(fargs, fmt_count, resultStr, fieldPosition, *status);

    delete[] fargs;

    if(U_FAILURE(*status)){
        return;
    }

	*formatted_len = resultStr.length();
	*formatted = eumalloc(*formatted_len+1);
	resultStr.extract(*formatted, *formatted_len+1, *status);
}

#define cleanup_zvals() for(int j=i;j>=0;j--) { zval_ptr_dtor((*args)+i); }

U_CFUNC void umsg_parse_helper(UMessageFormat *fmt, int *count, zval ***args, UChar *source, int source_len, UErrorCode *status)
{
    UnicodeString srcString(source, source_len);
    Formattable *fargs = ((const MessageFormat*)fmt)->parse(srcString, *count, *status);

	if(U_FAILURE(*status)) {
		return;
	}

	*args = (zval **)safe_emalloc(*count, sizeof(zval *), 0);

    // assign formattables to varargs
    for(int32_t i = 0; i < *count; i++) {
	    int64_t aInt64;
		double aDate;
		UnicodeString temp;
		char *stmp;
		int stmp_len;

		ALLOC_INIT_ZVAL((*args)[i]);
		
		switch(fargs[i].getType()) {
        case Formattable::kDate:
			aDate = ((double)fargs[i].getDate())/U_MILLIS_PER_SECOND;
			if(aDate > LONG_MAX || aDate < -LONG_MAX) {
				ZVAL_DOUBLE((*args)[i], aDate<0?ceil(aDate):floor(aDate));
			} else {
				ZVAL_LONG((*args)[i], (long)aDate);
			}
            break;

        case Formattable::kDouble:
			ZVAL_DOUBLE((*args)[i], (double)fargs[i].getDouble());
            break;

        case Formattable::kLong:
			ZVAL_LONG((*args)[i], fargs[i].getLong());
            break;

        case Formattable::kInt64:
            aInt64 = fargs[i].getInt64();
			if(aInt64 > LONG_MAX || aInt64 < -LONG_MAX) {
				ZVAL_DOUBLE((*args)[i], (double)aInt64);
			} else {
				ZVAL_LONG((*args)[i], (long)aInt64);
			}
            break;

        case Formattable::kString:
            fargs[i].getString(temp);
			intl_convert_utf16_to_utf8(&stmp, &stmp_len, temp.getBuffer(), temp.length(), status);
			if(U_FAILURE(*status)) {
				cleanup_zvals();
				return;
			}
			ZVAL_STRINGL((*args)[i], stmp, stmp_len, 0);
            break;

        case Formattable::kObject:
        case Formattable::kArray:
            *status = U_ILLEGAL_ARGUMENT_ERROR;
			cleanup_zvals();
            break;
        }
    }
	delete[] fargs;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
