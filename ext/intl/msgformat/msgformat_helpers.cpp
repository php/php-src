/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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

#include "../intl_cppshims.h"

#include <limits.h>
#include <unicode/msgfmt.h>
#include <unicode/chariter.h>
#include <unicode/ustdio.h>
#include <unicode/timezone.h>
#include <unicode/datefmt.h>
#include <unicode/calendar.h>

#include <vector>

#include "../intl_convertcpp.h"
#include "../common/common_date.h"

extern "C" {
#include "php_intl.h"
#include "msgformat_class.h"
#include "msgformat_format.h"
#include "msgformat_helpers.h"
#include "intl_convert.h"
#define USE_TIMEZONE_POINTER
#include "../timezone/timezone_class.h"
}

#if U_ICU_VERSION_MAJOR_NUM * 10 + U_ICU_VERSION_MINOR_NUM >= 48
#define HAS_MESSAGE_PATTERN 1
#endif

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
#ifdef HAS_MESSAGE_PATTERN
    static const MessagePattern getMessagePattern(MessageFormat* m);
#endif
};

const Formattable::Type*
MessageFormatAdapter::getArgTypeList(const MessageFormat& m,
                                     int32_t& count) {
    return m.getArgTypeList(count);
}

#ifdef HAS_MESSAGE_PATTERN
const MessagePattern
MessageFormatAdapter::getMessagePattern(MessageFormat* m) {
    return m->msgPattern;
}
#endif
U_NAMESPACE_END

U_CFUNC int32_t umsg_format_arg_count(UMessageFormat *fmt)
{
	int32_t fmt_count = 0;
	MessageFormatAdapter::getArgTypeList(*(const MessageFormat*)fmt, fmt_count);
	return fmt_count;
}

static void arg_types_dtor(zval *el) {
	efree(Z_PTR_P(el));
}

static HashTable *umsg_get_numeric_types(MessageFormatter_object *mfo,
										 intl_error& err)
{
	HashTable *ret;
	int32_t parts_count;

	if (U_FAILURE(err.code)) {
		return NULL;
	}

	if (mfo->mf_data.arg_types) {
		/* already cached */
		return mfo->mf_data.arg_types;
	}

	const Formattable::Type *types = MessageFormatAdapter::getArgTypeList(
		*(MessageFormat*)mfo->mf_data.umsgf, parts_count);

	/* Hash table will store Formattable::Type objects directly,
	 * so no need for destructor */
	ALLOC_HASHTABLE(ret);
	zend_hash_init(ret, parts_count, NULL, arg_types_dtor, 0);

	for (int i = 0; i < parts_count; i++) {
		const Formattable::Type t = types[i];
		if (zend_hash_index_update_mem(ret, (zend_ulong)i, (void*)&t, sizeof(t)) == NULL) {
			intl_errors_set(&err, U_MEMORY_ALLOCATION_ERROR,
				"Write to argument types hash table failed", 0);
			break;
		}
	}

	if (U_FAILURE(err.code)) {
		zend_hash_destroy(ret);
		efree(ret);

		return NULL;
	}

	mfo->mf_data.arg_types = ret;

	return ret;
}

#ifdef HAS_MESSAGE_PATTERN
static HashTable *umsg_parse_format(MessageFormatter_object *mfo,
									const MessagePattern& mp,
									intl_error& err)
{
	HashTable *ret;
	int32_t parts_count;

	if (U_FAILURE(err.code)) {
		return NULL;
	}

	if (!((MessageFormat *)mfo->mf_data.umsgf)->usesNamedArguments()) {
		return umsg_get_numeric_types(mfo, err);
	}

	if (mfo->mf_data.arg_types) {
		/* already cached */
		return mfo->mf_data.arg_types;
	}

	/* Hash table will store Formattable::Type objects directly,
	 * so no need for destructor */
	ALLOC_HASHTABLE(ret);
	zend_hash_init(ret, 32, NULL, arg_types_dtor, 0);

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
	for (int32_t i = 0; i < parts_count - 2 && U_SUCCESS(err.code); i++) {
		MessagePattern::Part p = mp.getPart(i);

		if (p.getType() != UMSGPAT_PART_TYPE_ARG_START) {
			continue;
		}

		MessagePattern::Part name_part = mp.getPart(++i); /* Getting name, advancing i */
		Formattable::Type type,
						  *storedType;

		if (name_part.getType() == UMSGPAT_PART_TYPE_ARG_NAME) {
			UnicodeString argName = mp.getSubstring(name_part);
			if ((storedType = (Formattable::Type*)zend_hash_str_find_ptr(ret, (char*)argName.getBuffer(), argName.length())) == NULL) {
				/* not found already; create new entry in HT */
				Formattable::Type bogusType = Formattable::kObject;
				if ((storedType = (Formattable::Type*)zend_hash_str_update_mem(ret, (char*)argName.getBuffer(), argName.length(),
						(void*)&bogusType, sizeof(bogusType))) == NULL) {
					intl_errors_set(&err, U_MEMORY_ALLOCATION_ERROR,
						"Write to argument types hash table failed", 0);
					continue;
				}
			}
		} else if (name_part.getType() == UMSGPAT_PART_TYPE_ARG_NUMBER) {
			int32_t argNumber = name_part.getValue();
			if (argNumber < 0) {
				intl_errors_set(&err, U_INVALID_FORMAT_ERROR,
					"Found part with negative number", 0);
				continue;
			}
			if ((storedType = (Formattable::Type*)zend_hash_index_find_ptr(ret, (zend_ulong)argNumber)) == NULL) {
				/* not found already; create new entry in HT */
				Formattable::Type bogusType = Formattable::kObject;
				if ((storedType = (Formattable::Type*)zend_hash_index_update_mem(ret, (zend_ulong)argNumber, (void*)&bogusType, sizeof(bogusType))) == NULL) {
					intl_errors_set(&err, U_MEMORY_ALLOCATION_ERROR,
						"Write to argument types hash table failed", 0);
					continue;
				}
			}
		} else {
			intl_errors_set(&err, U_INVALID_FORMAT_ERROR, "Invalid part type encountered", 0);
			continue;
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
							} else { /* some style invalid/unknown to us */
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
					intl_errors_set(&err, U_PARSE_ERROR,
						"Expected UMSGPAT_PART_TYPE_ARG_TYPE part following "
						"UMSGPAT_ARG_TYPE_SIMPLE part", 0);
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
			intl_errors_set(&err, U_ARGUMENT_TYPE_MISMATCH,
				"Inconsistent types declared for an argument", 0);
			continue;
		}

		*storedType = type;
	} /* visiting each part */

	if (U_FAILURE(err.code)) {
		zend_hash_destroy(ret);
		efree(ret);

		return NULL;
	}

	mfo->mf_data.arg_types = ret;

	return ret;
}
#endif

static HashTable *umsg_get_types(MessageFormatter_object *mfo,
								 intl_error& err)
{
	MessageFormat *mf = (MessageFormat *)mfo->mf_data.umsgf;

#ifdef HAS_MESSAGE_PATTERN
	const MessagePattern mp = MessageFormatAdapter::getMessagePattern(mf);

	return umsg_parse_format(mfo, mp, err);
#else
	if (mf->usesNamedArguments()) {
			intl_errors_set(&err, U_UNSUPPORTED_ERROR,
				"This extension supports named arguments only on ICU 4.8+",
				0);
		return NULL;
	}
	return umsg_get_numeric_types(mfo, err);
#endif
}

static void umsg_set_timezone(MessageFormatter_object *mfo,
							  intl_error& err)
{
	MessageFormat *mf = (MessageFormat *)mfo->mf_data.umsgf;
	TimeZone	  *used_tz = NULL;
	const Format  **formats;
	int32_t		  count;

	/* Unfortanely, this cannot change the time zone for arguments that
	 * appear inside complex formats because ::getFormats() returns NULL
	 * for all uncached formats, which is the case for complex formats
	 * unless they were set via one of the ::setFormat() methods */

	if (mfo->mf_data.tz_set) {
		return; /* already done */
	}

	formats = mf->getFormats(count);

	if (formats == NULL) {
		intl_errors_set(&err, U_MEMORY_ALLOCATION_ERROR,
			"Out of memory retrieving subformats", 0);
	}

	for (int i = 0; U_SUCCESS(err.code) && i < count; i++) {
		DateFormat* df = dynamic_cast<DateFormat*>(
			const_cast<Format *>(formats[i]));
		if (df == NULL) {
			continue;
		}

		if (used_tz == NULL) {
			zval nullzv, *zvptr = &nullzv;
			ZVAL_NULL(zvptr);
			used_tz = timezone_process_timezone_argument(zvptr, &err, "msgfmt_format");
			if (used_tz == NULL) {
				continue;
			}
		}

		df->setTimeZone(*used_tz);
	}

	if (U_SUCCESS(err.code)) {
		mfo->mf_data.tz_set = 1;
	}
}

U_CFUNC void umsg_format_helper(MessageFormatter_object *mfo,
								HashTable *args,
								UChar **formatted,
								int32_t *formatted_len)
{
	int arg_count = zend_hash_num_elements(args);
	std::vector<Formattable> fargs;
	std::vector<UnicodeString> farg_names;
	MessageFormat *mf = (MessageFormat *)mfo->mf_data.umsgf;
	HashTable *types;
	intl_error& err = INTL_DATA_ERROR(mfo);

	if (U_FAILURE(err.code)) {
		return;
	}

	types = umsg_get_types(mfo, err);

	umsg_set_timezone(mfo, err);

	fargs.resize(arg_count);
	farg_names.resize(arg_count);

	int				argNum = 0;
	zval			*elem;

	// Key related variables
	zend_string		*str_index;
	zend_ulong		 num_index;

	ZEND_HASH_FOREACH_KEY_VAL(args, num_index, str_index, elem) {
		Formattable& formattable = fargs[argNum];
		UnicodeString& key = farg_names[argNum];
		Formattable::Type argType = Formattable::kObject, //unknown
						  *storedArgType = NULL;
		if (!U_SUCCESS(err.code)) {
			break;
		}
		/* Process key and retrieve type */
		if (str_index == NULL) {
			/* includes case where index < 0 because it's exposed as unsigned */
			if (num_index > (zend_ulong)INT32_MAX) {
				intl_errors_set(&err, U_ILLEGAL_ARGUMENT_ERROR,
					"Found negative or too large array key", 0);
				continue;
			}

		   UChar temp[16];
		   int32_t len = u_sprintf(temp, "%u", (uint32_t)num_index);
		   key.append(temp, len);

		   storedArgType = (Formattable::Type*)zend_hash_index_find_ptr(types, (zend_ulong)num_index);
		} else { //string; assumed to be in UTF-8
			intl_stringFromChar(key, ZSTR_VAL(str_index), ZSTR_LEN(str_index), &err.code);

			if (U_FAILURE(err.code)) {
				char *message;
				spprintf(&message, 0,
					"Invalid UTF-8 data in argument key: '%s'", ZSTR_VAL(str_index));
				intl_errors_set(&err, err.code,	message, 1);
				efree(message);
				continue;
			}

			storedArgType = (Formattable::Type*)zend_hash_str_find_ptr(types, (char*)key.getBuffer(), key.length());
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
	string_arg:
					/* This implicitly converts objects
					 * Note that our vectors will leak if object conversion fails
					 * and PHP ends up with a fatal error and calls longjmp
					 * as a result of that.
					 */
					convert_to_string_ex(elem);

					UnicodeString *text = new UnicodeString();
					intl_stringFromChar(*text,
						Z_STRVAL_P(elem), Z_STRLEN_P(elem), &err.code);

					if (U_FAILURE(err.code)) {
						char *message;
						spprintf(&message, 0, "Invalid UTF-8 data in string argument: "
							"'%s'", Z_STRVAL_P(elem));
						intl_errors_set(&err, err.code, message, 1);
						efree(message);
						delete text;
						continue;
					}
					formattable.adoptString(text);
					break;
				}
			case Formattable::kDouble:
				{
					double d;
					if (Z_TYPE_P(elem) == IS_DOUBLE) {
						d = Z_DVAL_P(elem);
					} else if (Z_TYPE_P(elem) == IS_LONG) {
						d = (double)Z_LVAL_P(elem);
					} else {
						SEPARATE_ZVAL_IF_NOT_REF(elem);
						convert_scalar_to_number(elem);
						d = (Z_TYPE_P(elem) == IS_DOUBLE)
							? Z_DVAL_P(elem)
							: (double)Z_LVAL_P(elem);
					}
					formattable.setDouble(d);
					break;
				}
			case Formattable::kLong:
				{
					int32_t tInt32 = 0;
retry_klong:
					if (Z_TYPE_P(elem) == IS_DOUBLE) {
						if (Z_DVAL_P(elem) > (double)INT32_MAX ||
								Z_DVAL_P(elem) < (double)INT32_MIN) {
							intl_errors_set(&err, U_ILLEGAL_ARGUMENT_ERROR,
								"Found PHP float with absolute value too large for "
								"32 bit integer argument", 0);
						} else {
							tInt32 = (int32_t)Z_DVAL_P(elem);
						}
					} else if (Z_TYPE_P(elem) == IS_LONG) {
						if (Z_LVAL_P(elem) > INT32_MAX ||
								Z_LVAL_P(elem) < INT32_MIN) {
							intl_errors_set(&err, U_ILLEGAL_ARGUMENT_ERROR,
								"Found PHP integer with absolute value too large "
								"for 32 bit integer argument", 0);
						} else {
							tInt32 = (int32_t)Z_LVAL_P(elem);
						}
					} else {
						SEPARATE_ZVAL_IF_NOT_REF(elem);
						convert_scalar_to_number(elem);
						goto retry_klong;
					}
					formattable.setLong(tInt32);
					break;
				}
			case Formattable::kInt64:
				{
					int64_t tInt64 = 0;
retry_kint64:
					if (Z_TYPE_P(elem) == IS_DOUBLE) {
						if (Z_DVAL_P(elem) > (double)U_INT64_MAX ||
								Z_DVAL_P(elem) < (double)U_INT64_MIN) {
							intl_errors_set(&err, U_ILLEGAL_ARGUMENT_ERROR,
								"Found PHP float with absolute value too large for "
								"64 bit integer argument", 0);
						} else {
							tInt64 = (int64_t)Z_DVAL_P(elem);
						}
					} else if (Z_TYPE_P(elem) == IS_LONG) {
						/* assume long is not wider than 64 bits */
						tInt64 = (int64_t)Z_LVAL_P(elem);
					} else {
						SEPARATE_ZVAL_IF_NOT_REF(elem);
						convert_scalar_to_number(elem);
						goto retry_kint64;
					}
					formattable.setInt64(tInt64);
					break;
				}
			case Formattable::kDate:
				{
					double dd = intl_zval_to_millis(elem, &err, "msgfmt_format");
					if (U_FAILURE(err.code)) {
						char *message;
						zend_string *u8key;
						UErrorCode status = UErrorCode();
						u8key = intl_charFromString(key, &status);
						if (u8key) {
							spprintf(&message, 0, "The argument for key '%s' "
								"cannot be used as a date or time", ZSTR_VAL(u8key));
							intl_errors_set(&err, err.code, message, 1);
							zend_string_release(u8key);
							efree(message);
						}
						continue;
					}
					formattable.setDate(dd);
					break;
				}
			default:
				intl_errors_set(&err, U_ILLEGAL_ARGUMENT_ERROR,
					"Found unsupported argument type", 0);
				break;
			}
		} else {
			/* We couldn't find any information about the argument in the pattern, this
			 * means it's an extra argument. So convert it to a number if it's a number or
			 * bool or null and to a string if it's anything else except arrays . */
			switch (Z_TYPE_P(elem)) {
			case IS_DOUBLE:
				formattable.setDouble(Z_DVAL_P(elem));
				break;
			case IS_TRUE:
			case IS_FALSE:
				convert_to_long_ex(elem);
				/* Intentional fallthrough */
			case IS_LONG:
				formattable.setInt64((int64_t)Z_LVAL_P(elem));
				break;
			case IS_NULL:
				formattable.setInt64((int64_t)0);
				break;
			case IS_STRING:
			case IS_OBJECT:
				goto string_arg;
			default:
				{
					char *message;
					zend_string *u8key;
					UErrorCode status = UErrorCode();
					u8key = intl_charFromString(key, &status);
					if (u8key) {
						spprintf(&message, 0, "No strategy to convert the "
							"value given for the argument with key '%s' "
							"is available", ZSTR_VAL(u8key));
						intl_errors_set(&err,
							U_ILLEGAL_ARGUMENT_ERROR, message, 1);
						zend_string_release(u8key);
						efree(message);
					}
				}
			}
		}
		argNum++;
	} ZEND_HASH_FOREACH_END(); // visiting each argument

	if (U_FAILURE(err.code)) {
		return;
	}

	UnicodeString resultStr;
	FieldPosition fieldPosition(0);

	/* format the message */
	mf->format(farg_names.empty() ? NULL : &farg_names[0],
		fargs.empty() ? NULL : &fargs[0], arg_count, resultStr, err.code);

	if (U_FAILURE(err.code)) {
		intl_errors_set(&err, err.code,
			"Call to ICU MessageFormat::format() has failed", 0);
		return;
	}

	*formatted_len = resultStr.length();
	*formatted = eumalloc(*formatted_len+1);
	resultStr.extract(*formatted, *formatted_len+1, err.code);
	if (U_FAILURE(err.code)) {
		intl_errors_set(&err, err.code,
			"Error copying format() result", 0);
		return;
	}
}

#define cleanup_zvals() for(int j=i;j>=0;j--) { zval_ptr_dtor((*args)+i); }

U_CFUNC void umsg_parse_helper(UMessageFormat *fmt, int *count, zval **args, UChar *source, int32_t source_len, UErrorCode *status)
{
    UnicodeString srcString(source, source_len);
    Formattable *fargs = ((const MessageFormat*)fmt)->parse(srcString, *count, *status);

	if(U_FAILURE(*status)) {
		return;
	}

	*args = (zval *)safe_emalloc(*count, sizeof(zval), 0);

    // assign formattables to varargs
    for(int32_t i = 0; i < *count; i++) {
	    int64_t aInt64;
		double aDate;
		UnicodeString temp;
		zend_string *u8str;

		switch(fargs[i].getType()) {
        case Formattable::kDate:
			aDate = ((double)fargs[i].getDate())/U_MILLIS_PER_SECOND;
			ZVAL_DOUBLE(&(*args)[i], aDate);
            break;

        case Formattable::kDouble:
			ZVAL_DOUBLE(&(*args)[i], (double)fargs[i].getDouble());
            break;

        case Formattable::kLong:
			ZVAL_LONG(&(*args)[i], fargs[i].getLong());
            break;

        case Formattable::kInt64:
            aInt64 = fargs[i].getInt64();
			if(aInt64 > ZEND_LONG_MAX || aInt64 < -ZEND_LONG_MAX) {
				ZVAL_DOUBLE(&(*args)[i], (double)aInt64);
			} else {
				ZVAL_LONG(&(*args)[i], (zend_long)aInt64);
			}
            break;

        case Formattable::kString:
            fargs[i].getString(temp);
			u8str = intl_convert_utf16_to_utf8(temp.getBuffer(), temp.length(), status);
			if(!u8str) {
				cleanup_zvals();
				return;
			}
			ZVAL_NEW_STR(&(*args)[i], u8str);
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
