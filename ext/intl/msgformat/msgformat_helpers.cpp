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

// Fix build on Windows / old versions of ICU
#include <stdio.h>

#include <math.h>
#include <unicode/msgfmt.h>
#include <unicode/chariter.h>
#include <unicode/messagepattern.h>

#include <map>

extern "C" {
#include "php_intl.h"
#include "msgformat_class.h"
#include "msgformat_format.h"
#include "msgformat_helpers.h"
#include "intl_convert.h"
/* avoid redefinition of int8_t, already defined in unicode/pwin32.h */
#define _MSC_STDINT_H_ 1
#include "ext/date/php_date.h"
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
    static const MessagePattern getMessagePattern(MessageFormat* m);
};
const Formattable::Type*
MessageFormatAdapter::getArgTypeList(const MessageFormat& m,
                                     int32_t& count) {
    return m.getArgTypeList(count);
}
const MessagePattern
MessageFormatAdapter::getMessagePattern(MessageFormat* m) {
    return m->msgPattern;
}
U_NAMESPACE_END

U_CFUNC int32_t umsg_format_arg_count(UMessageFormat *fmt)
{
	int32_t fmt_count = 0;
	MessageFormatAdapter::getArgTypeList(*(const MessageFormat*)fmt, fmt_count);
	return fmt_count;
}

double umsg_helper_zval_to_millis(zval *z, UErrorCode *status TSRMLS_DC) {
    double rv = 0.0;
    if (Z_TYPE_P(z) == IS_DOUBLE) {
        rv = U_MILLIS_PER_SECOND * Z_DVAL_P(z);
    }
    else if (Z_TYPE_P(z) == IS_LONG) {
        rv = U_MILLIS_PER_SECOND * (double) Z_LVAL_P(z);
    }
    else if (Z_TYPE_P(z) == IS_OBJECT) {
        /* Borrowed from datefmt_format() in intl/dateformat/dateformat_format.c */
        if (instanceof_function(Z_OBJCE_P(z), php_date_get_date_ce() TSRMLS_CC)) {
            zval retval;
            zval *zfuncname;
            INIT_ZVAL(retval);
            MAKE_STD_ZVAL(zfuncname);
            ZVAL_STRING(zfuncname, "getTimestamp", 1);
            if (call_user_function(NULL, &(z), zfuncname, &retval, 0, NULL TSRMLS_CC) != SUCCESS || Z_TYPE(retval) != IS_LONG) {
                *status = U_RESOURCE_TYPE_MISMATCH;
            } else {
                rv = U_MILLIS_PER_SECOND * (double) Z_LVAL(retval);
            }
            zval_ptr_dtor(&zfuncname);
        } else {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
    return rv;
}

U_CFUNC void umsg_format_helper(UMessageFormat *fmt, int arg_count, zval **args, char **arg_names, UChar **formatted, int *formatted_len, UErrorCode *status TSRMLS_DC)
{
	int fmt_count;
    int32_t i;
	Formattable* fargs;
    UnicodeString *farg_names;
    MessageFormat *mf = (MessageFormat *) fmt;
    MessagePattern mp = MessageFormatAdapter::getMessagePattern(mf);
    std::map<UnicodeString, Formattable::Type> argTypesNamed;
    std::map<int32_t, Formattable::Type> argTypesNumbered;

    int32_t usingNamedArguments = mf->usesNamedArguments();

    /*
      looking through the pattern, go to each arg_start part type.
      the arg-typeof that tells us the argument type (simple, complicated)
      then the next part is either the arg_name or arg number
      and then if it's simple after that there could be a part-type=arg-type whise substring will tell us number, spellout, etc
      if the next thing isn't an arg-type then assume string
      same name that appears more than once in a complicated pattern will appear more than once, we could
      -- ignore subsequent occurances
      -- complain if types differ?
    */

    int32_t parts_count = mp.countParts();

    for (i = 0; i < parts_count; i++) {
        MessagePattern::Part p = mp.getPart(i);
        if (p.getType() == UMSGPAT_PART_TYPE_ARG_START) {
            MessagePattern::Part name_part = mp.getPart(++i); /* Getting name, advancing i */
            UnicodeString argName;
            int32_t argNumber;
            if (name_part.getType() == UMSGPAT_PART_TYPE_ARG_NAME) {
                argName = mp.getSubstring(name_part);
            }
            else if (name_part.getType() == UMSGPAT_PART_TYPE_ARG_NUMBER) {
                argNumber = name_part.getValue();
            }
            /* If we haven't seen this arg name before */
            int seenBefore = usingNamedArguments ? argTypesNamed.count(argName) : argTypesNumbered.count(argNumber);
            if (0 == seenBefore) {
                Formattable::Type fargType;
                UMessagePatternArgType argType = p.getArgType();
                /* No type specified, treat it as a string */
                if (argType == UMSGPAT_ARG_TYPE_NONE) {
                    fargType = Formattable::kString;
                }
                /* Some type was specified, might be simple or complicated */
                else {
                    if (argType == UMSGPAT_ARG_TYPE_SIMPLE) {
                        /* For a SIMPLE arg, after the name part, there should be
                         * an ARG_TYPE part whose string value tells us what to do */
                        MessagePattern::Part type_part = mp.getPart(++i); /* Getting type, advancing i */
                        if (type_part.getType() == UMSGPAT_PART_TYPE_ARG_TYPE) {
                            UnicodeString typeString = mp.getSubstring(type_part);
                            /* This is all based on the rules in the docs for MessageFormat
                             * @see http://icu-project.org/apiref/icu4c/classMessageFormat.html */
                            if (typeString == "number") {
                                MessagePattern::Part style_part = mp.getPart(i + 1); /* Not advancing i */
                                if (style_part.getType() == UMSGPAT_PART_TYPE_ARG_STYLE) {
                                    UnicodeString styleString = mp.getSubstring(style_part);
                                    if (styleString == "integer") {
                                        fargType = Formattable::kInt64;
                                    }
                                    else if (styleString == "currency") {
                                        fargType = Formattable::kDouble;
                                    }
                                    else if (styleString == "percent") {
                                        fargType = Formattable::kDouble;
                                    }
                                }
                                // if missing style, part, make it a double
                                else {
                                    fargType = Formattable::kDouble;
                                }
                            }
                            else if ((typeString == "date") || (typeString == "time")) {
                                fargType = Formattable::kDate;
                            }
                            else if ((typeString == "spellout") || (typeString == "ordinal") || (typeString == "duration")) {
                                fargType = Formattable::kDouble;
                            }

                        }
                        else {
                            /* If there's no UMSGPAT_PART_TYPE_ARG_TYPE right after a
                             * UMSGPAT_ARG_TYPE_SIMPLE argument, then the pattern
                             * is broken. */
                            *status = U_PARSE_ERROR;
                            return;
                        }
                    }
                    else if (argType == UMSGPAT_ARG_TYPE_PLURAL) {
                        fargType = Formattable::kDouble;
                    }
                    else if (argType == UMSGPAT_ARG_TYPE_CHOICE) {
                        fargType = Formattable::kDouble;
                    }
                    else if (argType == UMSGPAT_ARG_TYPE_SELECT) {
                        fargType = Formattable::kString;
                    }
                    else {
                        fargType = Formattable::kString;
                    }
                } /* was type specified? */
                if (usingNamedArguments) {
                    argTypesNamed.insert(std::pair<UnicodeString, Formattable::Type>(argName, fargType));
                } else {
                    argTypesNumbered.insert(std::pair<int32_t, Formattable::Type>(argNumber, fargType));
                }
            } /* Haven't seen arg before? */
        } /* checking for ARG_START */
    } /* visiting each part */

#define CLEANUP_AND_RETURN_ON_ERROR(status) do { \
        if (U_FAILURE(*status)) {                \
            delete[] fargs;                      \
            if (usingNamedArguments) {           \
                delete[] farg_names;             \
            }                                    \
            return;                              \
        }                                        \
    } while (0)


    fmt_count = arg_count;
    fargs = new Formattable[fmt_count];
    if (usingNamedArguments) {
        farg_names = new UnicodeString[fmt_count];
    }
    for (int32_t i = 0; i < fmt_count; ++i) {
            UChar* text = NULL;
            int textLen = 0;
            int found = 0;
            Formattable::Type argType;

            if (usingNamedArguments) {
                intl_convert_utf8_to_utf16(&text, &textLen, arg_names[i], strlen(arg_names[i]), status);
                CLEANUP_AND_RETURN_ON_ERROR(status);
                farg_names[i].setTo(text, textLen);
                efree(text);
                text = NULL; textLen = 0;
                std::map<UnicodeString, Formattable::Type>::iterator it;
                it = argTypesNamed.find(farg_names[i]);
                if (it != argTypesNamed.end()) {
                    argType = it->second;
                    found = 1;
                }
            }
            else {
                std::map<int32_t, Formattable::Type>::iterator it;
                it = argTypesNumbered.find(i);
                if (it != argTypesNumbered.end()) {
                    argType = it->second;
                    found = 1;
                }
            }
            if (found) {
                switch (argType) {
                case Formattable::kString:
                    /* This implicitly converts objects by attempting to call __toString() */
                    convert_to_string_ex(&args[i]);
                    intl_convert_utf8_to_utf16(&text, &textLen, Z_STRVAL_P(args[i]), Z_STRLEN_P(args[i]), status);
                    CLEANUP_AND_RETURN_ON_ERROR(status);
                    fargs[i].setString(text);
                    efree(text);
                    text = NULL; textLen = 0;
                    break;
                case Formattable::kDouble:
                    {
                        double d;
                        if(Z_TYPE_P(args[i]) == IS_DOUBLE) {
                            d = Z_DVAL_P(args[i]);
                        } else if(Z_TYPE_P(args[i]) == IS_LONG) {
                            d = (double)Z_LVAL_P(args[i]);
                        } else {
                            SEPARATE_ZVAL_IF_NOT_REF(&args[i]);
                            convert_scalar_to_number( args[i] TSRMLS_CC );
                            d = (Z_TYPE_P(args[i]) == IS_DOUBLE)?Z_DVAL_P(args[i]):(double)Z_LVAL_P(args[i]);
                        }
                        fargs[i].setDouble(d);
                        break;
                    }
                case Formattable::kInt64:
                    {
                        int64_t tInt64;
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
                    }
                case Formattable::kDate:
                    {
                        double dd = umsg_helper_zval_to_millis(args[i], status TSRMLS_CC);
                        CLEANUP_AND_RETURN_ON_ERROR(status);
                        fargs[i].setDate(dd);
                        break;
                    }
                }
            }
            else {
                /* We couldn't find any information about the argument in the pattern, this
                 * means it's an extra argument. So convert it to a number if it's a number or
                 * bool or null and to a string if it's anything else. */
                switch (Z_TYPE_P(args[i])) {
                case IS_DOUBLE:
                    fargs[i].setDouble(Z_DVAL_P(args[i]));
                    break;
                case IS_BOOL:
                    convert_to_long_ex(&args[i]);
                    /* Intentional fallthrough */
                case IS_LONG:
                    fargs[i].setInt64((int64_t) Z_LVAL_P(args[i]));
                    break;
                case IS_NULL:
                    fargs[i].setInt64((int64_t) 0);
                    break;
                default:
                    convert_to_string_ex(&args[i]);
                    intl_convert_utf8_to_utf16(&text, &textLen, Z_STRVAL_P(args[i]), Z_STRLEN_P(args[i]), status);
                    CLEANUP_AND_RETURN_ON_ERROR(status);
                    fargs[i].setString(text);
                    efree(text);
                    text = NULL; textLen = 0;
                    break;
                }
            }
    } // visiting each argument argument

    UnicodeString resultStr;
    FieldPosition fieldPosition(0);

    /* format the message */
    if (usingNamedArguments) {
        mf->format(farg_names, fargs, fmt_count, resultStr, *status);
        delete[] farg_names;
    } else {
        mf->format(fargs, fmt_count, resultStr, fieldPosition, *status);
    }
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
