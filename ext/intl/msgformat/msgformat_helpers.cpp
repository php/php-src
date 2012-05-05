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
#include <unicode/ustdio.h>

#include <vector>

#include "../intl_convertcpp.h"

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
	} else if (Z_TYPE_P(z) == IS_LONG) {
		rv = U_MILLIS_PER_SECOND * (double)Z_LVAL_P(z);
	} else if (Z_TYPE_P(z) == IS_OBJECT) {
		/* Borrowed from datefmt_format() in intl/dateformat/dateformat_format.c */
		if (instanceof_function(Z_OBJCE_P(z), php_date_get_date_ce() TSRMLS_CC)) {
			zval retval;
			zval *zfuncname;
			INIT_ZVAL(retval);
			MAKE_STD_ZVAL(zfuncname);
			ZVAL_STRING(zfuncname, "getTimestamp", 1);
			if (call_user_function(NULL, &(z), zfuncname, &retval, 0, NULL TSRMLS_CC)
					!= SUCCESS || Z_TYPE(retval) != IS_LONG) {
				*status = U_RESOURCE_TYPE_MISMATCH;
			} else {
				rv = U_MILLIS_PER_SECOND * (double)Z_LVAL(retval);
			}
			zval_ptr_dtor(&zfuncname);
		} else {
			*status = U_ILLEGAL_ARGUMENT_ERROR;
		}
	}
	return rv;
}

static HashTable *umsg_parse_format(const MessagePattern& mp, UErrorCode& uec)
{
	HashTable *ret;
	int32_t parts_count;

	if (U_FAILURE(uec)) {
		return NULL;
	}

	/* Hash table will store Formattable::Type objects directly,
	 * so no need for destructor */
	ALLOC_HASHTABLE(ret);
	zend_hash_init(ret, 32, NULL, NULL, 0);

	parts_count = mp.countParts();

	// See MessageFormat::cacheExplicitFormats()
	/*
	 * Looking through the pattern, go to each arg_start part type.
	 * The arg-typeof that tells us the argument type (simple, complicated)
	 * then the next part is either the arg_name or arg number
	 * and then if it's simple after that there could be a part-type=arg-type
	 * while substring will tell us number, spellout, etc.
	 * If the next thing isn't an arg-type then assume string.
	*/
	/* The last two "parts" can at most be ARG_LIMIT and MSG_LIMIT
	 * which we need not examine. */
	for (int32_t i = 0; i < parts_count - 2 && U_SUCCESS(uec); i++) {
        MessagePattern::Part p = mp.getPart(i);

        if (p.getType() != UMSGPAT_PART_TYPE_ARG_START) {
			continue;
		}

        MessagePattern::Part name_part = mp.getPart(++i); /* Getting name, advancing i */
		Formattable::Type type,
						  *storedType;

        if (name_part.getType() == UMSGPAT_PART_TYPE_ARG_NAME) {
            UnicodeString argName = mp.getSubstring(name_part);
			if (zend_hash_find(ret, (char*)argName.getBuffer(), argName.length(),
					(void**)&storedType) == FAILURE) {
				/* not found already; create new entry in HT */
				Formattable::Type bogusType = Formattable::kObject;
				if (zend_hash_update(ret, (char*)argName.getBuffer(), argName.length(),
						(void*)&bogusType, sizeof(bogusType), (void**)&storedType) == FAILURE) {
					uec = U_MEMORY_ALLOCATION_ERROR;
					continue;
				}
			}
        } else if (name_part.getType() == UMSGPAT_PART_TYPE_ARG_NUMBER) {
            int32_t argNumber = name_part.getValue();
			if (argNumber < 0) {
				uec = U_INVALID_FORMAT_ERROR;
				continue;
			}
			if (zend_hash_index_find(ret, (ulong)argNumber, (void**)&storedType)
					== FAILURE) {
				/* not found already; create new entry in HT */
				Formattable::Type bogusType = Formattable::kObject;
				if (zend_hash_index_update(ret, (ulong)argNumber, (void*)&bogusType,
						sizeof(bogusType), (void**)&storedType) == FAILURE) {
					uec = U_MEMORY_ALLOCATION_ERROR;
					continue;
				}
			}
        }

        UMessagePatternArgType argType = p.getArgType();
        /* No type specified, treat it as a string */
        if (argType == UMSGPAT_ARG_TYPE_NONE) {
            type = Formattable::kString;
		} else { /* Some type was specified, might be simple or complicated */
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
                                type = Formattable::kInt64;
                            } else if (styleString == "currency") {
                                type = Formattable::kDouble;
                            } else if (styleString == "percent") {
                                type = Formattable::kDouble;
                            }
                        } else { // if missing style, part, make it a double
                            type = Formattable::kDouble;
                        }
                    } else if ((typeString == "date") || (typeString == "time")) {
                        type = Formattable::kDate;
                    } else if ((typeString == "spellout") || (typeString == "ordinal")
							|| (typeString == "duration")) {
                        type = Formattable::kDouble;
                    }
                } else {
                    /* If there's no UMSGPAT_PART_TYPE_ARG_TYPE right after a
                     * UMSGPAT_ARG_TYPE_SIMPLE argument, then the pattern
                     * is broken. */
                    uec = U_PARSE_ERROR;
                    continue;
                }
            } else if (argType == UMSGPAT_ARG_TYPE_PLURAL) {
                type = Formattable::kDouble;
            } else if (argType == UMSGPAT_ARG_TYPE_CHOICE) {
                type = Formattable::kDouble;
            } else if (argType == UMSGPAT_ARG_TYPE_SELECT) {
                type = Formattable::kString;
            } else {
                type = Formattable::kString;
            }
        } /* was type specified? */

		/* We found a different type for the same arg! */
		if (*storedType != Formattable::kObject && *storedType != type) {
			uec = U_ARGUMENT_TYPE_MISMATCH;
			continue;
		}

		*storedType = type;
	} /* visiting each part */

	if (U_FAILURE(uec)) {
		zend_hash_destroy(ret);
		efree(ret);

		return NULL;
	}

	return ret;
}

U_CFUNC void umsg_format_helper(UMessageFormat *fmt, HashTable *args, UChar **formatted, int *formatted_len, UErrorCode *status TSRMLS_DC)
{
	int arg_count = zend_hash_num_elements(args);
	std::vector<Formattable> fargs;
	std::vector<UnicodeString> farg_names;
    MessageFormat *mf = (MessageFormat *) fmt;
    const MessagePattern mp = MessageFormatAdapter::getMessagePattern(mf);
	HashTable *types;

	fargs.resize(arg_count);
	farg_names.resize(arg_count);

	types = umsg_parse_format(mp, *status);
	if (U_FAILURE(*status)) {
		return;
	}

	int				argNum = 0;
	HashPosition	pos;
	zval			**elem;

	// Key related variables
	int				key_type;
	char			*str_index;
	uint			str_len;
	ulong			num_index;

	for (zend_hash_internal_pointer_reset_ex(args, &pos);
		U_SUCCESS(*status) &&
			(key_type = zend_hash_get_current_key_ex(
					args, &str_index, &str_len, &num_index, 0, &pos),
				zend_hash_get_current_data_ex(args, (void **)&elem, &pos)
			) == SUCCESS;
		zend_hash_move_forward_ex(args, &pos), argNum++)
	{
		Formattable& formattable = fargs[argNum];
		UnicodeString& key = farg_names[argNum];
		Formattable::Type argType = Formattable::kObject, //unknown
						  *storedArgType = NULL;

		/* Process key and retrieve type */
		if (key_type == HASH_KEY_IS_LONG) {
			/* includes case where index < 0 because it's exposed as unsigned */
			if (num_index > INT32_MAX) {
				*status = U_ILLEGAL_ARGUMENT_ERROR;
				continue;
			}

           UChar temp[16];
		   int32_t len = u_sprintf(temp, "%u", (uint32_t)num_index);
           key.append(temp, len);

		   zend_hash_index_find(types, (ulong)num_index, (void**)&storedArgType);
		} else { //string; assumed to be in UTF-8
			intl_stringFromChar(key, str_index, str_len-1, status);
			if (U_FAILURE(*status)) {
				   continue;
			}

			zend_hash_find(types, (char*)key.getBuffer(), key.length(),
				(void**)&storedArgType);
		}

		if (storedArgType != NULL) {
			argType = *storedArgType;
		}

		/* Convert zval to formattable according to message format type
		 * or (as a fallback) the zval type */
		if (argType != Formattable::kObject) {
            switch (argType) {
			case Formattable::kString:
				{
string_arg: //XXX: make function
					/* This implicitly converts objects */
					convert_to_string_ex(elem);

					UnicodeString *text = new UnicodeString();
					intl_stringFromChar(*text, Z_STRVAL_PP(elem), Z_STRLEN_PP(elem), status);
					if (U_FAILURE(*status)) {
						delete text;
						continue;
					}
					formattable.adoptString(text);
					break;
				}
            case Formattable::kDouble:
                {
                    double d;
                    if (Z_TYPE_PP(elem) == IS_DOUBLE) {
                        d = Z_DVAL_PP(elem);
                    } else if (Z_TYPE_PP(elem) == IS_LONG) {
                        d = (double)Z_LVAL_PP(elem);
                    } else {
                        SEPARATE_ZVAL_IF_NOT_REF(elem);
                        convert_scalar_to_number(*elem TSRMLS_CC);
                        d = (Z_TYPE_PP(elem) == IS_DOUBLE)
							? Z_DVAL_PP(elem)
							: (double)Z_LVAL_PP(elem);
                    }
					formattable.setDouble(d);
                    break;
                }
            case Formattable::kInt64:
                {
                    int64_t tInt64;
                    if (Z_TYPE_PP(elem) == IS_DOUBLE) {
                        tInt64 = (int64_t)Z_DVAL_PP(elem);
                    } else if (Z_TYPE_PP(elem) == IS_LONG) {
                        tInt64 = (int64_t)Z_LVAL_PP(elem);
                    } else {
                        SEPARATE_ZVAL_IF_NOT_REF(elem);
                        convert_scalar_to_number(*elem TSRMLS_CC);
                        tInt64 = (Z_TYPE_PP(elem) == IS_DOUBLE)
							? (int64_t)Z_DVAL_PP(elem)
							: Z_LVAL_PP(elem);
                    }
                    formattable.setInt64(tInt64);
                    break;
                }
            case Formattable::kDate:
                {
                    double dd = umsg_helper_zval_to_millis(*elem, status TSRMLS_CC);
					if (U_FAILURE(*status)) {
						continue;
					}
                    formattable.setDate(dd);
                    break;
                }
            }
        } else {
            /* We couldn't find any information about the argument in the pattern, this
             * means it's an extra argument. So convert it to a number if it's a number or
             * bool or null and to a string if it's anything else. */
            switch (Z_TYPE_PP(elem)) {
            case IS_DOUBLE:
                formattable.setDouble(Z_DVAL_PP(elem));
                break;
            case IS_BOOL:
                convert_to_long_ex(elem);
                /* Intentional fallthrough */
            case IS_LONG:
                formattable.setInt64((int64_t)Z_LVAL_PP(elem));
                break;
            case IS_NULL:
                formattable.setInt64((int64_t)0);
                break;
            default:
                goto string_arg;
            }
		}
    } // visiting each argument

	zend_hash_destroy(types);
	efree(types);

    if (U_FAILURE(*status)){
        return;
    }

	UnicodeString resultStr;
	FieldPosition fieldPosition(0);

    /* format the message */
	mf->format(farg_names.empty() ? NULL : &farg_names[0],
		fargs.empty() ? NULL : &fargs[0], arg_count, resultStr, *status);

    if (U_FAILURE(*status)) {
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
