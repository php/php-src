/*
 * schemastypes.c : implementation of the XML Schema Datatypes
 *             definition and validity checking
 *
 * See Copyright for the status of this software.
 *
 * Daniel Veillard <veillard@redhat.com>
 */

#define IN_LIBXML
#include "libxml.h"

#ifdef LIBXML_SCHEMAS_ENABLED

#include <string.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/hash.h>
#include <libxml/valid.h>

#include <libxml/xmlschemas.h>
#include <libxml/schemasInternals.h>
#include <libxml/xmlschemastypes.h>

#ifdef HAVE_MATH_H
#include <math.h>
#endif

#define DEBUG

#define TODO 								\
    xmlGenericError(xmlGenericErrorContext,				\
	    "Unimplemented block at %s:%d\n",				\
            __FILE__, __LINE__);

#define XML_SCHEMAS_NAMESPACE_NAME \
    (const xmlChar *)"http://www.w3.org/2001/XMLSchema"

typedef enum {
    XML_SCHEMAS_UNKNOWN = 0,
    XML_SCHEMAS_STRING,
    XML_SCHEMAS_NMTOKEN,
    XML_SCHEMAS_DECIMAL,
    XML_SCHEMAS_TIME,
    XML_SCHEMAS_GDAY,
    XML_SCHEMAS_GMONTH,
    XML_SCHEMAS_GMONTHDAY,
    XML_SCHEMAS_GYEAR,
    XML_SCHEMAS_GYEARMONTH,
    XML_SCHEMAS_DATE,
    XML_SCHEMAS_DATETIME,
    XML_SCHEMAS_DURATION,
    XML_SCHEMAS_FLOAT,
    XML_SCHEMAS_DOUBLE,
    XML_SCHEMAS_,
    XML_SCHEMAS_XXX
} xmlSchemaValType;

unsigned long powten[10] = {
    1, 10, 100, 1000, 10000, 100000, 1000000, 10000000L,
    100000000L, 1000000000L
};

/* Date value */
typedef struct _xmlSchemaValDate xmlSchemaValDate;
typedef xmlSchemaValDate *xmlSchemaValDatePtr;
struct _xmlSchemaValDate {
    long		year;
    unsigned int	mon	:4;	/* 1 <=  mon    <= 12   */
    unsigned int	day	:5;	/* 1 <=  day    <= 31   */
    unsigned int	hour	:5;	/* 0 <=  hour   <= 23   */
    unsigned int	min	:6;	/* 0 <=  min    <= 59	*/
    double		sec;
    int			tz_flag	:1;	/* is tzo explicitely set? */
    int			tzo	:11;	/* -1440 <= tzo <= 1440 */
};

/* Duration value */
typedef struct _xmlSchemaValDuration xmlSchemaValDuration;
typedef xmlSchemaValDuration *xmlSchemaValDurationPtr;
struct _xmlSchemaValDuration {
    long	        mon;		/* mon stores years also */
    long        	day;
    double		sec;            /* sec stores min and hour also */
};

typedef struct _xmlSchemaValDecimal xmlSchemaValDecimal;
typedef xmlSchemaValDecimal *xmlSchemaValDecimalPtr;
struct _xmlSchemaValDecimal {
    /* would use long long but not portable */
    unsigned long base;
    unsigned int extra;
    unsigned int sign:1;
    int frac:7;
    int total:8;
};

struct _xmlSchemaVal {
    xmlSchemaValType type;
    union {
	xmlSchemaValDecimal     decimal;
        xmlSchemaValDate        date;
        xmlSchemaValDuration    dur;
	float			f;
	double			d;
    } value;
};

static int xmlSchemaTypesInitialized = 0;
static xmlHashTablePtr xmlSchemaTypesBank = NULL;

static xmlSchemaTypePtr xmlSchemaTypeStringDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeAnyTypeDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeAnySimpleTypeDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeDecimalDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeDatetimeDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeDateDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeTimeDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeGYearDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeGYearMonthDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeGDayDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeGMonthDayDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeGMonthDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeDurationDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypePositiveIntegerDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeNonNegativeIntegerDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeNmtoken = NULL;
static xmlSchemaTypePtr xmlSchemaTypeFloatDef = NULL;
static xmlSchemaTypePtr xmlSchemaTypeDoubleDef = NULL;

/*
 * xmlSchemaInitBasicType:
 * @name:  the type name
 *
 * Initialize one default type
 */
static xmlSchemaTypePtr
xmlSchemaInitBasicType(const char *name) {
    xmlSchemaTypePtr ret;

    ret = (xmlSchemaTypePtr) xmlMalloc(sizeof(xmlSchemaType));
    if (ret == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		"Could not initilize type %s: out of memory\n", name);
	return(NULL);
    }
    memset(ret, 0, sizeof(xmlSchemaType));
    ret->name = xmlStrdup((const xmlChar *)name);
    ret->type = XML_SCHEMA_TYPE_BASIC;
    ret->contentType = XML_SCHEMA_CONTENT_BASIC;
    xmlHashAddEntry2(xmlSchemaTypesBank, ret->name,
	             XML_SCHEMAS_NAMESPACE_NAME, ret);
    return(ret);
}

/*
 * xmlSchemaInitTypes:
 *
 * Initialize the default XML Schemas type library
 */
void
xmlSchemaInitTypes(void) {
    if (xmlSchemaTypesInitialized != 0)
	return;
    xmlSchemaTypesBank = xmlHashCreate(40);
    
    xmlSchemaTypeStringDef = xmlSchemaInitBasicType("string");
    xmlSchemaTypeAnyTypeDef = xmlSchemaInitBasicType("anyType");
    xmlSchemaTypeAnySimpleTypeDef = xmlSchemaInitBasicType("anySimpleType");
    xmlSchemaTypeDecimalDef = xmlSchemaInitBasicType("decimal");
    xmlSchemaTypeDateDef = xmlSchemaInitBasicType("date");
    xmlSchemaTypeDatetimeDef = xmlSchemaInitBasicType("dateTime");
    xmlSchemaTypeTimeDef = xmlSchemaInitBasicType("time");
    xmlSchemaTypeGYearDef = xmlSchemaInitBasicType("gYear");
    xmlSchemaTypeGYearMonthDef = xmlSchemaInitBasicType("gYearMonth");
    xmlSchemaTypeGMonthDef = xmlSchemaInitBasicType("gMonth");
    xmlSchemaTypeGMonthDayDef = xmlSchemaInitBasicType("gMonthDay");
    xmlSchemaTypeGDayDef = xmlSchemaInitBasicType("gDay");
    xmlSchemaTypeDurationDef = xmlSchemaInitBasicType("duration");
    xmlSchemaTypePositiveIntegerDef = xmlSchemaInitBasicType("positiveInteger");
    xmlSchemaTypeNonNegativeIntegerDef =
	xmlSchemaInitBasicType("nonNegativeInteger");
    xmlSchemaTypeNmtoken = xmlSchemaInitBasicType("NMTOKEN");
    xmlSchemaTypeFloatDef = xmlSchemaInitBasicType("float");
    xmlSchemaTypeDoubleDef = xmlSchemaInitBasicType("double");

    xmlSchemaTypesInitialized = 1;
}

/**
 * xmlSchemaCleanupTypes:
 *
 * Cleanup the default XML Schemas type library
 */
void	
xmlSchemaCleanupTypes(void) {
    if (xmlSchemaTypesInitialized == 0)
	return;
    xmlHashFree(xmlSchemaTypesBank, (xmlHashDeallocator) xmlSchemaFreeType);
    xmlSchemaTypesInitialized = 0;
}

/**
 * xmlSchemaNewValue:
 * @type:  the value type
 *
 * Allocate a new simple type value
 *
 * Returns a pointer to the new value or NULL in case of error
 */
static xmlSchemaValPtr
xmlSchemaNewValue(xmlSchemaValType type) {
    xmlSchemaValPtr value;

    value = (xmlSchemaValPtr) xmlMalloc(sizeof(xmlSchemaVal));
    if (value == NULL) {
	return(NULL);
    }
    memset(value, 0, sizeof(xmlSchemaVal));
    value->type = type;
    return(value);
}

/**
 * xmlSchemaFreeValue:
 * @value:  the value to free
 *
 * Cleanup the default XML Schemas type library
 */
void	
xmlSchemaFreeValue(xmlSchemaValPtr value) {
    if (value == NULL)
	return;
    xmlFree(value);
}

/**
 * xmlSchemaGetPredefinedType:
 * @name: the type name
 * @ns:  the URI of the namespace usually "http://www.w3.org/2001/XMLSchema"
 *
 * Lookup a type in the default XML Schemas type library
 *
 * Returns the type if found, NULL otherwise
 */
xmlSchemaTypePtr
xmlSchemaGetPredefinedType(const xmlChar *name, const xmlChar *ns) {
    if (xmlSchemaTypesInitialized == 0)
	xmlSchemaInitTypes();
    if (name == NULL)
	return(NULL);
    return((xmlSchemaTypePtr) xmlHashLookup2(xmlSchemaTypesBank, name, ns));
}

/****************************************************************
 *								*
 *		Convenience macros and functions		*
 *								*
 ****************************************************************/

#define IS_TZO_CHAR(c)						\
	((c == 0) || (c == 'Z') || (c == '+') || (c == '-'))

#define VALID_YEAR(yr)          (yr != 0)
#define VALID_MONTH(mon)        ((mon >= 1) && (mon <= 12))
/* VALID_DAY should only be used when month is unknown */
#define VALID_DAY(day)          ((day >= 1) && (day <= 31))
#define VALID_HOUR(hr)          ((hr >= 0) && (hr <= 23))
#define VALID_MIN(min)          ((min >= 0) && (min <= 59))
#define VALID_SEC(sec)          ((sec >= 0) && (sec < 60))
#define VALID_TZO(tzo)          ((tzo > -1440) && (tzo < 1440))
#define IS_LEAP(y)						\
	(((y % 4 == 0) && (y % 100 != 0)) || (y % 400 == 0))

static const long daysInMonth[12] =
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static const long daysInMonthLeap[12] =
	{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

#define MAX_DAYINMONTH(yr,mon)                                  \
        (IS_LEAP(yr) ? daysInMonthLeap[mon - 1] : daysInMonth[mon - 1])

#define VALID_MDAY(dt)						\
	(IS_LEAP(dt->year) ?				        \
	    (dt->day <= daysInMonthLeap[dt->mon - 1]) :	        \
	    (dt->day <= daysInMonth[dt->mon - 1]))

#define VALID_DATE(dt)						\
	(VALID_YEAR(dt->year) && VALID_MONTH(dt->mon) && VALID_MDAY(dt))

#define VALID_TIME(dt)						\
	(VALID_HOUR(dt->hour) && VALID_MIN(dt->min) &&		\
	 VALID_SEC(dt->sec) && VALID_TZO(dt->tzo))

#define VALID_DATETIME(dt)					\
	(VALID_DATE(dt) && VALID_TIME(dt))

#define SECS_PER_MIN            (60)
#define SECS_PER_HOUR           (60 * SECS_PER_MIN)
#define SECS_PER_DAY            (24 * SECS_PER_HOUR)

static const long dayInYearByMonth[12] =
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
static const long dayInLeapYearByMonth[12] =
	{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };

#define DAY_IN_YEAR(day, month, year)				\
        ((IS_LEAP(year) ?					\
                dayInLeapYearByMonth[month - 1] :		\
                dayInYearByMonth[month - 1]) + day)

#ifdef DEBUG
#define DEBUG_DATE(dt)                                                  \
    xmlGenericError(xmlGenericErrorContext,                             \
        "type=%o %04ld-%02u-%02uT%02u:%02u:%03f",                       \
        dt->type,dt->value.date.year,dt->value.date.mon,                \
        dt->value.date.day,dt->value.date.hour,dt->value.date.min,      \
        dt->value.date.sec);                                            \
    if (dt->value.date.tz_flag)                                         \
        if (dt->value.date.tzo != 0)                                    \
            xmlGenericError(xmlGenericErrorContext,                     \
                "%+05d\n",dt->value.date.tzo);                          \
        else                                                            \
            xmlGenericError(xmlGenericErrorContext, "Z\n");             \
    else                                                                \
        xmlGenericError(xmlGenericErrorContext,"\n")
#else
#define DEBUG_DATE(dt)
#endif

/**
 * _xmlSchemaParseGYear:
 * @dt:  pointer to a date structure
 * @str: pointer to the string to analyze
 *
 * Parses a xs:gYear without time zone and fills in the appropriate
 * field of the @dt structure. @str is updated to point just after the
 * xs:gYear. It is supposed that @dt->year is big enough to contain
 * the year.
 *
 * Returns 0 or the error code
 */
static int
_xmlSchemaParseGYear (xmlSchemaValDatePtr dt, const xmlChar **str) {
    const xmlChar *cur = *str, *firstChar;
    int isneg = 0, digcnt = 0;

    if (((*cur < '0') || (*cur > '9')) &&
	(*cur != '-') && (*cur != '+'))
	return -1;

    if (*cur == '-') {
	isneg = 1;
	cur++;
    }

    firstChar = cur;

    while ((*cur >= '0') && (*cur <= '9')) {
	dt->year = dt->year * 10 + (*cur - '0');
	cur++;
	digcnt++;
    }

    /* year must be at least 4 digits (CCYY); over 4
     * digits cannot have a leading zero. */
    if ((digcnt < 4) || ((digcnt > 4) && (*firstChar == '0')))
	return 1;

    if (isneg)
	dt->year = - dt->year;

    if (!VALID_YEAR(dt->year))
	return 2;

    *str = cur;
    return 0;
}

/**
 * PARSE_2_DIGITS:
 * @num:  the integer to fill in
 * @cur:  an #xmlChar *
 * @invalid: an integer
 *
 * Parses a 2-digits integer and updates @num with the value. @cur is
 * updated to point just after the integer.
 * In case of error, @invalid is set to %TRUE, values of @num and
 * @cur are undefined.
 */
#define PARSE_2_DIGITS(num, cur, invalid)			\
	if ((cur[0] < '0') || (cur[0] > '9') ||			\
	    (cur[1] < '0') || (cur[1] > '9'))			\
	    invalid = 1;					\
	else							\
	    num = (cur[0] - '0') * 10 + (cur[1] - '0');		\
	cur += 2;

/**
 * PARSE_FLOAT:
 * @num:  the double to fill in
 * @cur:  an #xmlChar *
 * @invalid: an integer
 *
 * Parses a float and updates @num with the value. @cur is
 * updated to point just after the float. The float must have a
 * 2-digits integer part and may or may not have a decimal part.
 * In case of error, @invalid is set to %TRUE, values of @num and
 * @cur are undefined.
 */
#define PARSE_FLOAT(num, cur, invalid)				\
	PARSE_2_DIGITS(num, cur, invalid);			\
	if (!invalid && (*cur == '.')) {			\
	    double mult = 1;				        \
	    cur++;						\
	    if ((*cur < '0') || (*cur > '9'))			\
		invalid = 1;					\
	    while ((*cur >= '0') && (*cur <= '9')) {		\
		mult /= 10;					\
		num += (*cur - '0') * mult;			\
		cur++;						\
	    }							\
	}

/**
 * _xmlSchemaParseGMonth:
 * @dt:  pointer to a date structure
 * @str: pointer to the string to analyze
 *
 * Parses a xs:gMonth without time zone and fills in the appropriate
 * field of the @dt structure. @str is updated to point just after the
 * xs:gMonth.
 *
 * Returns 0 or the error code
 */
static int
_xmlSchemaParseGMonth (xmlSchemaValDatePtr dt, const xmlChar **str) {
    const xmlChar *cur = *str;
    int ret = 0;

    PARSE_2_DIGITS(dt->mon, cur, ret);
    if (ret != 0)
	return ret;

    if (!VALID_MONTH(dt->mon))
	return 2;

    *str = cur;
    return 0;
}

/**
 * _xmlSchemaParseGDay:
 * @dt:  pointer to a date structure
 * @str: pointer to the string to analyze
 *
 * Parses a xs:gDay without time zone and fills in the appropriate
 * field of the @dt structure. @str is updated to point just after the
 * xs:gDay.
 *
 * Returns 0 or the error code
 */
static int
_xmlSchemaParseGDay (xmlSchemaValDatePtr dt, const xmlChar **str) {
    const xmlChar *cur = *str;
    int ret = 0;

    PARSE_2_DIGITS(dt->day, cur, ret);
    if (ret != 0)
	return ret;

    if (!VALID_DAY(dt->day))
	return 2;

    *str = cur;
    return 0;
}

/**
 * _xmlSchemaParseTime:
 * @dt:  pointer to a date structure
 * @str: pointer to the string to analyze
 *
 * Parses a xs:time without time zone and fills in the appropriate
 * fields of the @dt structure. @str is updated to point just after the
 * xs:time.
 * In case of error, values of @dt fields are undefined.
 *
 * Returns 0 or the error code
 */
static int
_xmlSchemaParseTime (xmlSchemaValDatePtr dt, const xmlChar **str) {
    const xmlChar *cur = *str;
    unsigned int hour = 0; /* use temp var in case str is not xs:time */
    int ret = 0;

    PARSE_2_DIGITS(hour, cur, ret);
    if (ret != 0)
	return ret;

    if (*cur != ':')
	return 1;
    cur++;

    /* the ':' insures this string is xs:time */
    dt->hour = hour;

    PARSE_2_DIGITS(dt->min, cur, ret);
    if (ret != 0)
	return ret;

    if (*cur != ':')
	return 1;
    cur++;

    PARSE_FLOAT(dt->sec, cur, ret);
    if (ret != 0)
	return ret;

    if (!VALID_TIME(dt))
	return 2;

    *str = cur;
    return 0;
}

/**
 * _xmlSchemaParseTimeZone:
 * @dt:  pointer to a date structure
 * @str: pointer to the string to analyze
 *
 * Parses a time zone without time zone and fills in the appropriate
 * field of the @dt structure. @str is updated to point just after the
 * time zone.
 *
 * Returns 0 or the error code
 */
static int
_xmlSchemaParseTimeZone (xmlSchemaValDatePtr dt, const xmlChar **str) {
    const xmlChar *cur = *str;
    int ret = 0;

    if (str == NULL)
	return -1;

    switch (*cur) {
    case 0:
	dt->tz_flag = 0;
	dt->tzo = 0;
	break;

    case 'Z':
	dt->tz_flag = 1;
	dt->tzo = 0;
	cur++;
	break;

    case '+':
    case '-': {
	int isneg = 0, tmp = 0;
	isneg = (*cur == '-');

	cur++;

	PARSE_2_DIGITS(tmp, cur, ret);
	if (ret != 0)
	    return ret;
	if (!VALID_HOUR(tmp))
	    return 2;

	if (*cur != ':')
	    return 1;
	cur++;

	dt->tzo = tmp * 60;

	PARSE_2_DIGITS(tmp, cur, ret);
	if (ret != 0)
	    return ret;
	if (!VALID_MIN(tmp))
	    return 2;

	dt->tzo += tmp;
	if (isneg)
	    dt->tzo = - dt->tzo;

	if (!VALID_TZO(dt->tzo))
	    return 2;

	dt->tz_flag = 1;
	break;
      }
    default:
	return 1;
    }

    *str = cur;
    return 0;
}

/****************************************************************
 *								*
 *	XML Schema Dates/Times Datatypes Handling		*
 *								*
 ****************************************************************/

/**
 * PARSE_DIGITS:
 * @num:  the integer to fill in
 * @cur:  an #xmlChar *
 * @num_type: an integer flag
 *
 * Parses a digits integer and updates @num with the value. @cur is
 * updated to point just after the integer.
 * In case of error, @num_type is set to -1, values of @num and
 * @cur are undefined.
 */
#define PARSE_DIGITS(num, cur, num_type)	                \
	if ((*cur < '0') || (*cur > '9'))			\
	    num_type = -1;					\
        else                                                    \
	    while ((*cur >= '0') && (*cur <= '9')) {		\
	        num = num * 10 + (*cur - '0');		        \
	        cur++;                                          \
            }

/**
 * PARSE_NUM:
 * @num:  the double to fill in
 * @cur:  an #xmlChar *
 * @num_type: an integer flag
 *
 * Parses a float or integer and updates @num with the value. @cur is
 * updated to point just after the number. If the number is a float,
 * then it must have an integer part and a decimal part; @num_type will
 * be set to 1. If there is no decimal part, @num_type is set to zero.
 * In case of error, @num_type is set to -1, values of @num and
 * @cur are undefined.
 */
#define PARSE_NUM(num, cur, num_type)				\
        num = 0;                                                \
	PARSE_DIGITS(num, cur, num_type);	                \
	if (!num_type && (*cur == '.')) {			\
	    double mult = 1;				        \
	    cur++;						\
	    if ((*cur < '0') || (*cur > '9'))			\
		num_type = -1;					\
            else                                                \
                num_type = 1;                                   \
	    while ((*cur >= '0') && (*cur <= '9')) {		\
		mult /= 10;					\
		num += (*cur - '0') * mult;			\
		cur++;						\
	    }							\
	}

/**
 * xmlSchemaValidateDates:
 * @type: the predefined type
 * @dateTime:  string to analyze
 * @val:  the return computed value
 *
 * Check that @dateTime conforms to the lexical space of one of the date types.
 * if true a value is computed and returned in @val.
 *
 * Returns 0 if this validates, a positive error code number otherwise
 *         and -1 in case of internal or API error.
 */
static int
xmlSchemaValidateDates (xmlSchemaTypePtr type ATTRIBUTE_UNUSED,
	                const xmlChar *dateTime, xmlSchemaValPtr *val) {
    xmlSchemaValPtr dt;
    int ret;
    const xmlChar *cur = dateTime;

#define RETURN_TYPE_IF_VALID(t)					\
    if (IS_TZO_CHAR(*cur)) {					\
	ret = _xmlSchemaParseTimeZone(&(dt->value.date), &cur);	\
	if (ret == 0) {						\
	    if (*cur != 0)					\
		goto error;					\
	    dt->type = t;					\
            if (val != NULL)                                    \
                *val = dt;                                      \
	    return 0;						\
	}							\
    }

    if (dateTime == NULL)
	return -1;

    if ((*cur != '-') && (*cur < '0') && (*cur > '9'))
	return 1;

    dt = xmlSchemaNewValue(XML_SCHEMAS_UNKNOWN);
    if (dt == NULL)
	return -1;

    if ((cur[0] == '-') && (cur[1] == '-')) {
	/*
	 * It's an incomplete date (xs:gMonthDay, xs:gMonth or
	 * xs:gDay)
	 */
	cur += 2;

	/* is it an xs:gDay? */
	if (*cur == '-') {
	  ++cur;
	    ret = _xmlSchemaParseGDay(&(dt->value.date), &cur);
	    if (ret != 0)
		goto error;

	    RETURN_TYPE_IF_VALID(XML_SCHEMAS_GDAY);

	    goto error;
	}

	/*
	 * it should be an xs:gMonthDay or xs:gMonth
	 */
	ret = _xmlSchemaParseGMonth(&(dt->value.date), &cur);
	if (ret != 0)
	    goto error;

	if (*cur != '-')
	    goto error;
	cur++;

	/* is it an xs:gMonth? */
	if (*cur == '-') {
	    cur++;
	    RETURN_TYPE_IF_VALID(XML_SCHEMAS_GMONTH);
	    goto error;
	}

	/* it should be an xs:gMonthDay */
	ret = _xmlSchemaParseGDay(&(dt->value.date), &cur);
	if (ret != 0)
	    goto error;

	RETURN_TYPE_IF_VALID(XML_SCHEMAS_GMONTHDAY);

	goto error;
    }

    /*
     * It's a right-truncated date or an xs:time.
     * Try to parse an xs:time then fallback on right-truncated dates.
     */
    if ((*cur >= '0') && (*cur <= '9')) {
	ret = _xmlSchemaParseTime(&(dt->value.date), &cur);
	if (ret == 0) {
	    /* it's an xs:time */
	    RETURN_TYPE_IF_VALID(XML_SCHEMAS_TIME);
	}
    }

    /* fallback on date parsing */
    cur = dateTime;

    ret = _xmlSchemaParseGYear(&(dt->value.date), &cur);
    if (ret != 0)
	goto error;

    /* is it an xs:gYear? */
    RETURN_TYPE_IF_VALID(XML_SCHEMAS_GYEAR);

    if (*cur != '-')
	goto error;
    cur++;

    ret = _xmlSchemaParseGMonth(&(dt->value.date), &cur);
    if (ret != 0)
	goto error;

    /* is it an xs:gYearMonth? */
    RETURN_TYPE_IF_VALID(XML_SCHEMAS_GYEARMONTH);

    if (*cur != '-')
	goto error;
    cur++;

    ret = _xmlSchemaParseGDay(&(dt->value.date), &cur);
    if ((ret != 0) || !VALID_DATE((&(dt->value.date))))
	goto error;

    /* is it an xs:date? */
    RETURN_TYPE_IF_VALID(XML_SCHEMAS_DATE);

    if (*cur != 'T')
	goto error;
    cur++;

    /* it should be an xs:dateTime */
    ret = _xmlSchemaParseTime(&(dt->value.date), &cur);
    if (ret != 0)
	goto error;

    ret = _xmlSchemaParseTimeZone(&(dt->value.date), &cur);
    if ((ret != 0) || (*cur != 0) || !VALID_DATETIME((&(dt->value.date))))
	goto error;

    dt->type = XML_SCHEMAS_DATETIME;

    if (val != NULL)
        *val = dt;

    return 0;

error:
    if (dt != NULL)
	xmlSchemaFreeValue(dt);
    return 1;
}

/**
 * xmlSchemaValidateDuration:
 * @type: the predefined type
 * @duration:  string to analyze
 * @val:  the return computed value
 *
 * Check that @duration conforms to the lexical space of the duration type.
 * if true a value is computed and returned in @val.
 *
 * Returns 0 if this validates, a positive error code number otherwise
 *         and -1 in case of internal or API error.
 */
static int
xmlSchemaValidateDuration (xmlSchemaTypePtr type ATTRIBUTE_UNUSED,
	                   const xmlChar *duration, xmlSchemaValPtr *val) {
    const xmlChar  *cur = duration;
    xmlSchemaValPtr dur;
    int isneg = 0;
    unsigned int seq = 0;

    if (duration == NULL)
	return -1;

    if (*cur == '-') {
        isneg = 1;
        cur++;
    }

    /* duration must start with 'P' (after sign) */
    if (*cur++ != 'P')
	return 1;

    dur = xmlSchemaNewValue(XML_SCHEMAS_DURATION);
    if (dur == NULL)
	return -1;

    while (*cur != 0) {
        double         num;
        int            num_type = 0;  /* -1 = invalid, 0 = int, 1 = floating */
        const xmlChar  desig[] = {'Y', 'M', 'D', 'H', 'M', 'S'};
        const double   multi[] = { 0.0, 0.0, 86400.0, 3600.0, 60.0, 1.0, 0.0};

        /* input string should be empty or invalid date/time item */
        if (seq >= sizeof(desig))
            goto error;

        /* T designator must be present for time items */
        if (*cur == 'T') {
            if (seq <= 3) {
                seq = 3;
                cur++;
            } else
                return 1;
        } else if (seq == 3)
            goto error;

        /* parse the number portion of the item */
        PARSE_NUM(num, cur, num_type);

        if ((num_type == -1) || (*cur == 0))
            goto error;

        /* update duration based on item type */
        while (seq < sizeof(desig)) {
            if (*cur == desig[seq]) {

                /* verify numeric type; only seconds can be float */
                if ((num_type != 0) && (seq < (sizeof(desig)-1)))
                    goto error;

                switch (seq) {
                    case 0:
                        dur->value.dur.mon = (long)num * 12;
                        break;
                    case 1:
                        dur->value.dur.mon += (long)num;
                        break;
                    default:
                        /* convert to seconds using multiplier */
                        dur->value.dur.sec += num * multi[seq];
                        seq++;
                        break;
                }

                break;          /* exit loop */
            }
            /* no date designators found? */
            if (++seq == 3)
                goto error;
        }
        cur++;
    }

    if (isneg) {
        dur->value.dur.mon = -dur->value.dur.mon;
        dur->value.dur.day = -dur->value.dur.day;
        dur->value.dur.sec = -dur->value.dur.sec;
    }

    if (val != NULL)
        *val = dur;

    return 0;

error:
    if (dur != NULL)
	xmlSchemaFreeValue(dur);
    return 1;
}

/**
 * xmlSchemaValidatePredefinedType:
 * @type: the predefined type
 * @value: the value to check
 * @val:  the return computed value
 *
 * Check that a value conforms to the lexical space of the predefined type.
 * if true a value is computed and returned in @val.
 *
 * Returns 0 if this validates, a positive error code number otherwise
 *         and -1 in case of internal or API error.
 */
int
xmlSchemaValidatePredefinedType(xmlSchemaTypePtr type, const xmlChar *value,
	                        xmlSchemaValPtr *val) {
    xmlSchemaValPtr v;

    if (xmlSchemaTypesInitialized == 0)
	return(-1);
    if (type == NULL)
	return(-1);

    if (val != NULL)
	*val = NULL;
    if (type == xmlSchemaTypeStringDef) { 
	return(0);
    } else if (type == xmlSchemaTypeAnyTypeDef) {
	return(0);
    } else if (type == xmlSchemaTypeAnySimpleTypeDef) {
	return(0);
    } else if (type == xmlSchemaTypeNmtoken) {
	if (xmlValidateNmtokenValue(value))
	    return(0);
	return(1);
    } else if (type == xmlSchemaTypeDecimalDef) {
	const xmlChar *cur = value, *tmp;
	int frac = 0, len, neg = 0;
	unsigned long base = 0;
	if (cur == NULL)
	    return(1);
	if (*cur == '+')
	    cur++;
	else if (*cur == '-') {
	    neg = 1;
	    cur++;
	}
	tmp = cur;
	while ((*cur >= '0') && (*cur <= '9')) {
	    base = base * 10 + (*cur - '0');
	    cur++;
	}
	len = cur - tmp;
	if (*cur == '.') {
	    cur++;
	    tmp = cur;
	    while ((*cur >= '0') && (*cur <= '9')) {
		base = base * 10 + (*cur - '0');
		cur++;
	    }
	    frac = cur - tmp;
	}
	if (*cur != 0)
	    return(1);
	if (val != NULL) {
	    v = xmlSchemaNewValue(XML_SCHEMAS_DECIMAL);
	    if (v != NULL) {
		v->value.decimal.base = base;
		v->value.decimal.sign = neg;
		v->value.decimal.frac = frac;
		v->value.decimal.total = frac + len;
		*val = v;
	    }
	}
	return(0);
    } else if (type == xmlSchemaTypeDurationDef) {
        return xmlSchemaValidateDuration(type, value, val);
    } else if ((type == xmlSchemaTypeDatetimeDef) ||
               (type == xmlSchemaTypeTimeDef) ||
               (type == xmlSchemaTypeDateDef) ||
               (type == xmlSchemaTypeGYearDef) ||
               (type == xmlSchemaTypeGYearMonthDef) ||
               (type == xmlSchemaTypeGMonthDef) ||
               (type == xmlSchemaTypeGMonthDayDef) ||
               (type == xmlSchemaTypeGDayDef))         {
        return xmlSchemaValidateDates(type, value, val);
    } else if (type == xmlSchemaTypePositiveIntegerDef) {
	const xmlChar *cur = value;
	unsigned long base = 0;
	int total = 0;
	if (cur == NULL)
	    return(1);
	if (*cur == '+')
	    cur++;
	while ((*cur >= '0') && (*cur <= '9')) {
	    base = base * 10 + (*cur - '0');
	    total++;
	    cur++;
	}
	if (*cur != 0)
	    return(1);
	if (val != NULL) {
	    v = xmlSchemaNewValue(XML_SCHEMAS_DECIMAL);
	    if (v != NULL) {
		v->value.decimal.base = base;
		v->value.decimal.sign = 0;
		v->value.decimal.frac = 0;
		v->value.decimal.total = total;
		*val = v;
	    }
	}
	return(0);
    } else if (type == xmlSchemaTypeNonNegativeIntegerDef) {
	const xmlChar *cur = value;
	unsigned long base = 0;
	int total = 0;
	int sign = 0;
	if (cur == NULL)
	    return(1);
	if (*cur == '-') {
	    sign = 1;
	    cur++;
	} else if (*cur == '+')
	    cur++;
	while ((*cur >= '0') && (*cur <= '9')) {
	    base = base * 10 + (*cur - '0');
	    total++;
	    cur++;
	}
	if (*cur != 0)
	    return(1);
	if ((sign == 1) && (base != 0))
	    return(1);
	if (val != NULL) {
	    v = xmlSchemaNewValue(XML_SCHEMAS_DECIMAL);
	    if (v != NULL) {
		v->value.decimal.base = base;
		v->value.decimal.sign = 0;
		v->value.decimal.frac = 0;
		v->value.decimal.total = total;
		*val = v;
	    }
	}
	return(0);
    } else if (type == xmlSchemaTypeFloatDef) {
	const xmlChar *cur = value, *tmp;
	int frac = 0, len, neg = 0;
	unsigned long base = 0;
	if (cur == NULL)
	    return(1);
	if (*cur == '+')
	    cur++;
	else if (*cur == '-') {
	    neg = 1;
	    cur++;
	}
	tmp = cur;
	while ((*cur >= '0') && (*cur <= '9')) {
	    base = base * 10 + (*cur - '0');
	    cur++;
	}
	len = cur - tmp;
	if (*cur == '.') {
	    cur++;
	    tmp = cur;
	    while ((*cur >= '0') && (*cur <= '9')) {
		base = base * 10 + (*cur - '0');
		cur++;
	    }
	    frac = cur - tmp;
	}
	TODO
	return(0);
    } else if (type == xmlSchemaTypeDoubleDef) {
	TODO
	return(0);
    } else {
	TODO
	return(0);
    }
}

/**
 * xmlSchemaCompareDecimals:
 * @x:  a first decimal value
 * @y:  a second decimal value
 *
 * Compare 2 decimals
 *
 * Returns -1 if x < y, 0 if x == y, 1 if x > y and -2 in case of error
 */
static int
xmlSchemaCompareDecimals(xmlSchemaValPtr x, xmlSchemaValPtr y)
{
    xmlSchemaValPtr swp;
    int order = 1;
    unsigned long tmp;

    if ((x->value.decimal.sign) && (x->value.decimal.sign))
        order = -1;
    else if (x->value.decimal.sign)
        return (-1);
    else if (y->value.decimal.sign)
        return (1);
    if (x->value.decimal.frac == y->value.decimal.frac) {
        if (x->value.decimal.base < y->value.decimal.base)
            return (-1);
        return (x->value.decimal.base > y->value.decimal.base);
    }
    if (y->value.decimal.frac > x->value.decimal.frac) {
        swp = y;
        y = x;
        x = swp;
        order = -order;
    }
    tmp =
        x->value.decimal.base / powten[x->value.decimal.frac -
                                       y->value.decimal.frac];
    if (tmp > y->value.decimal.base)
        return (order);
    if (tmp < y->value.decimal.base)
        return (-order);
    tmp =
        y->value.decimal.base * powten[x->value.decimal.frac -
                                       y->value.decimal.frac];
    if (x->value.decimal.base < tmp)
        return (-order);
    if (x->value.decimal.base == tmp)
        return (0);
    return (order);
}

/**
 * xmlSchemaCompareDurations:
 * @x:  a first duration value
 * @y:  a second duration value
 *
 * Compare 2 durations
 *
 * Returns -1 if x < y, 0 if x == y, 1 if x > y, 2 if x <> y, and -2 in
 * case of error
 */
static int
xmlSchemaCompareDurations(xmlSchemaValPtr x, xmlSchemaValPtr y)
{
    long carry, mon, day;
    double sec;
    long xmon, xday, myear, lyear, minday, maxday;
    static const long dayRange [2][12] = {
        { 0, 28, 59, 89, 120, 150, 181, 212, 242, 273, 303, 334, },
        { 0, 31, 62, 92, 123, 153, 184, 215, 245, 276, 306, 337} };

    if ((x == NULL) || (y == NULL))
        return -2;

    /* months */
    mon = x->value.dur.mon - y->value.dur.mon;

    /* seconds */
    sec = x->value.dur.sec - y->value.dur.sec;
    carry = (long)sec / SECS_PER_DAY;
    sec -= (double)(carry * SECS_PER_DAY);

    /* days */
    day = x->value.dur.day - y->value.dur.day + carry;

    /* easy test */
    if (mon == 0) {
        if (day == 0)
            if (sec == 0.0)
                return 0;
            else if (sec < 0.0)
                return -1;
            else
                return 1;
        else if (day < 0)
            return -1;
        else
            return 1;
    }

    if (mon > 0) {
        if ((day >= 0) && (sec >= 0.0))
            return 1;
        else {
            xmon = mon;
            xday = -day;
        }
    } else if ((day <= 0) && (sec <= 0.0)) {
        return -1;
    } else {
        xmon = -mon;
        xday = day;
    }

    myear = xmon / 12;
    lyear = myear / 4;
    minday = (myear * 365) + (lyear != 0 ? lyear - 1 : 0);
    maxday = (myear * 365) + (lyear != 0 ? lyear + 1 : 0);
        
    xmon = xmon % 12;
    minday += dayRange[0][xmon];
    maxday += dayRange[1][xmon];

    if (maxday < xday)
        return 1;
    else if (minday > xday)
        return -1;

    /* indeterminate */
    return 2;
}

/*
 * macros for adding date/times and durations
 */
#define FQUOTIENT(a,b)                  (floor(((double)a/(double)b)))
#define MODULO(a,b)                     (a - FQUOTIENT(a,b) * b)
#define FQUOTIENT_RANGE(a,low,high)     (FQUOTIENT((a-low),(high-low)))
#define MODULO_RANGE(a,low,high)        ((MODULO((a-low),(high-low)))+low)

/**
 * _xmlSchemaDateAdd:
 * @dt: an #xmlSchemaValPtr
 * @dur: an #xmlSchemaValPtr of type #XS_DURATION
 *
 * Compute a new date/time from @dt and @dur. This function assumes @dt
 * is either #XML_SCHEMAS_DATETIME, #XML_SCHEMAS_DATE, #XML_SCHEMAS_GYEARMONTH,
 * or #XML_SCHEMAS_GYEAR.
 *
 * Returns date/time pointer or NULL.
 */
static xmlSchemaValPtr
_xmlSchemaDateAdd (xmlSchemaValPtr dt, xmlSchemaValPtr dur)
{
    xmlSchemaValPtr ret;
    long carry, tempdays, temp;
    xmlSchemaValDatePtr r, d;
    xmlSchemaValDurationPtr u;

    if ((dt == NULL) || (dur == NULL))
        return NULL;

    ret = xmlSchemaNewValue(dt->type);
    if (ret == NULL)
        return NULL;

    r = &(ret->value.date);
    d = &(dt->value.date);
    u = &(dur->value.dur);

    /* normalization */
    if (d->mon == 0)
        d->mon = 1;

    /* normalize for time zone offset */
    u->sec -= (d->tzo * 60);
    d->tzo = 0;

    /* normalization */
    if (d->day == 0)
        d->day = 1;

    /* month */
    carry  = d->mon + u->mon;
    r->mon = MODULO_RANGE(carry, 1, 13);
    carry  = FQUOTIENT_RANGE(carry, 1, 13);

    /* year (may be modified later) */
    r->year = d->year + carry;
    if (r->year == 0) {
        if (d->year > 0)
            r->year--;
        else
            r->year++;
    }

    /* time zone */
    r->tzo     = d->tzo;
    r->tz_flag = d->tz_flag;

    /* seconds */
    r->sec = d->sec + u->sec;
    carry  = FQUOTIENT((long)r->sec, 60);
    if (r->sec != 0.0) {
        r->sec = MODULO(r->sec, 60.0);
    }

    /* minute */
    carry += d->min;
    r->min = MODULO(carry, 60);
    carry  = FQUOTIENT(carry, 60);

    /* hours */
    carry  += d->hour;
    r->hour = MODULO(carry, 24);
    carry   = FQUOTIENT(carry, 24);

    /*
     * days
     * Note we use tempdays because the temporary values may need more
     * than 5 bits
     */
    if ((VALID_YEAR(r->year)) && (VALID_MONTH(r->mon)) &&
                  (d->day > MAX_DAYINMONTH(r->year, r->mon)))
        tempdays = MAX_DAYINMONTH(r->year, r->mon);
    else if (d->day < 1)
        tempdays = 1;
    else
        tempdays = d->day;

    tempdays += u->day + carry;

    while (1) {
        if (tempdays < 1) {
            long tmon = MODULO_RANGE(r->mon-1, 1, 13);
            long tyr  = r->year + FQUOTIENT_RANGE(r->mon-1, 1, 13);
            if (tyr == 0)
                tyr--;
            tempdays += MAX_DAYINMONTH(tyr, tmon);
            carry = -1;
        } else if (tempdays > MAX_DAYINMONTH(r->year, r->mon)) {
            tempdays = tempdays - MAX_DAYINMONTH(r->year, r->mon);
            carry = 1;
        } else
            break;

        temp = r->mon + carry;
        r->mon = MODULO_RANGE(temp, 1, 13);
        r->year = r->year + FQUOTIENT_RANGE(temp, 1, 13);
        if (r->year == 0) {
            if (temp < 1)
                r->year--;
            else
                r->year++;
	}
    }
    
    r->day = tempdays;

    /*
     * adjust the date/time type to the date values
     */
    if (ret->type != XML_SCHEMAS_DATETIME) {
        if ((r->hour) || (r->min) || (r->sec))
            ret->type = XML_SCHEMAS_DATETIME;
        else if (ret->type != XML_SCHEMAS_DATE) {
            if ((r->mon != 1) && (r->day != 1))
                ret->type = XML_SCHEMAS_DATE;
            else if ((ret->type != XML_SCHEMAS_GYEARMONTH) && (r->mon != 1))
                ret->type = XML_SCHEMAS_GYEARMONTH;
        }
    }

    return ret;
}

/**
 * xmlSchemaDupVal:
 * @v: value to duplicate
 *
 * returns a duplicated value.
 */
static xmlSchemaValPtr
xmlSchemaDupVal (xmlSchemaValPtr v)
{
    xmlSchemaValPtr ret = xmlSchemaNewValue(v->type);
    if (ret == NULL)
        return ret;
    
    memcpy(ret, v, sizeof(xmlSchemaVal));
    return ret;
}

/**
 * xmlSchemaDateNormalize:
 * @dt: an #xmlSchemaValPtr
 *
 * Normalize @dt to GMT time.
 *
 */
static xmlSchemaValPtr
xmlSchemaDateNormalize (xmlSchemaValPtr dt, double offset)
{
    xmlSchemaValPtr dur, ret;

    if (dt == NULL)
        return NULL;

    if (((dt->type != XML_SCHEMAS_TIME) &&
         (dt->type != XML_SCHEMAS_DATETIME)) || (dt->value.date.tzo == 0))
        return xmlSchemaDupVal(dt);

    dur = xmlSchemaNewValue(XML_SCHEMAS_DURATION);
    if (dur == NULL)
        return NULL;

    dur->value.date.sec -= offset;

    ret = _xmlSchemaDateAdd(dt, dur);
    if (ret == NULL)
        return NULL;

    xmlSchemaFreeValue(dur);

    /* ret->value.date.tzo = 0; */
    return ret;
}

/**
 * _xmlSchemaDateCastYMToDays:
 * @dt: an #xmlSchemaValPtr
 *
 * Convert mon and year of @dt to total number of days. Take the 
 * number of years since (or before) 1 AD and add the number of leap
 * years. This is a function  because negative
 * years must be handled a little differently and there is no zero year.
 *
 * Returns number of days.
 */
static long
_xmlSchemaDateCastYMToDays (const xmlSchemaValPtr dt)
{
    long ret;

    if (dt->value.date.year < 0)
        ret = (dt->value.date.year * 365) +
              (((dt->value.date.year+1)/4)-((dt->value.date.year+1)/100)+
               ((dt->value.date.year+1)/400)) +
              DAY_IN_YEAR(0, dt->value.date.mon, dt->value.date.year);
    else
        ret = ((dt->value.date.year-1) * 365) +
              (((dt->value.date.year-1)/4)-((dt->value.date.year-1)/100)+
               ((dt->value.date.year-1)/400)) +
              DAY_IN_YEAR(0, dt->value.date.mon, dt->value.date.year);

    return ret;
}

/**
 * TIME_TO_NUMBER:
 * @dt:  an #xmlSchemaValPtr
 *
 * Calculates the number of seconds in the time portion of @dt.
 *
 * Returns seconds.
 */
#define TIME_TO_NUMBER(dt)                              \
    ((double)((dt->value.date.hour * SECS_PER_HOUR) +   \
              (dt->value.date.min * SECS_PER_MIN)) + dt->value.date.sec)

/**
 * xmlSchemaCompareDates:
 * @x:  a first date/time value
 * @y:  a second date/time value
 *
 * Compare 2 date/times
 *
 * Returns -1 if x < y, 0 if x == y, 1 if x > y, 2 if x <> y, and -2 in
 * case of error
 */
static int
xmlSchemaCompareDates (xmlSchemaValPtr x, xmlSchemaValPtr y)
{
    unsigned char xmask, ymask, xor_mask, and_mask;
    xmlSchemaValPtr p1, p2, q1, q2;
    long p1d, p2d, q1d, q2d;

    if ((x == NULL) || (y == NULL))
        return -2;

    if (x->value.date.tz_flag) {

        if (!y->value.date.tz_flag) {
            p1 = xmlSchemaDateNormalize(x, 0);
            p1d = _xmlSchemaDateCastYMToDays(p1) + p1->value.date.day;
            /* normalize y + 14:00 */
            q1 = xmlSchemaDateNormalize(y, (14 * SECS_PER_HOUR));

            q1d = _xmlSchemaDateCastYMToDays(q1) + q1->value.date.day;
            if (p1d < q1d) {
		xmlSchemaFreeValue(p1);
		xmlSchemaFreeValue(q1);
                return -1;
	    } else if (p1d == q1d) {
                double sec;

                sec = TIME_TO_NUMBER(p1) - TIME_TO_NUMBER(q1);
                if (sec < 0.0) {
		    xmlSchemaFreeValue(p1);
		    xmlSchemaFreeValue(q1);
                    return -1;
		} else {
                    /* normalize y - 14:00 */
                    q2 = xmlSchemaDateNormalize(y, -(14 * SECS_PER_HOUR));
                    q2d = _xmlSchemaDateCastYMToDays(q2) + q2->value.date.day;
		    xmlSchemaFreeValue(p1);
		    xmlSchemaFreeValue(q1);
		    xmlSchemaFreeValue(q2);
                    if (p1d > q2d)
                        return 1;
                    else if (p1d == q2d) {
                        sec = TIME_TO_NUMBER(p1) - TIME_TO_NUMBER(q2);
                        if (sec > 0.0)
                            return 1;
                        else
                            return 2; /* indeterminate */
                    }
                }
            } else {
		xmlSchemaFreeValue(p1);
		xmlSchemaFreeValue(q1);
	    }
        }
    } else if (y->value.date.tz_flag) {
        q1 = xmlSchemaDateNormalize(y, 0);
        q1d = _xmlSchemaDateCastYMToDays(q1) + q1->value.date.day;

        /* normalize x - 14:00 */
        p1 = xmlSchemaDateNormalize(x, -(14 * SECS_PER_HOUR));
        p1d = _xmlSchemaDateCastYMToDays(p1) + p1->value.date.day;

        if (p1d < q1d) {
	    xmlSchemaFreeValue(p1);
	    xmlSchemaFreeValue(q1);
            return -1;
	} else if (p1d == q1d) {
            double sec;

            sec = TIME_TO_NUMBER(p1) - TIME_TO_NUMBER(q1);
            if (sec < 0.0) {
		xmlSchemaFreeValue(p1);
		xmlSchemaFreeValue(q1);
                return -1;
	    } else {
                /* normalize x + 14:00 */
                p2 = xmlSchemaDateNormalize(x, (14 * SECS_PER_HOUR));
                p2d = _xmlSchemaDateCastYMToDays(p2) + p2->value.date.day;

		xmlSchemaFreeValue(p1);
		xmlSchemaFreeValue(q1);
		xmlSchemaFreeValue(p2);
                if (p2d > q1d)
                    return 1;
                else if (p2d == q1d) {
                    sec = TIME_TO_NUMBER(p2) - TIME_TO_NUMBER(q1);
                    if (sec > 0.0)
                        return 1;
                    else
                        return 2; /* indeterminate */
                }
            }
	} else {
	    xmlSchemaFreeValue(p1);
	    xmlSchemaFreeValue(q1);
        }
    }

    /*
     * if the same type then calculate the difference
     */
    if (x->type == y->type) {
        q1 = xmlSchemaDateNormalize(y, 0);
        q1d = _xmlSchemaDateCastYMToDays(q1) + q1->value.date.day;

        p1 = xmlSchemaDateNormalize(x, 0);
        p1d = _xmlSchemaDateCastYMToDays(p1) + p1->value.date.day;

        if (p1d < q1d) {
	    xmlSchemaFreeValue(p1);
	    xmlSchemaFreeValue(q1);
            return -1;
	} else if (p1d > q1d) {
	    xmlSchemaFreeValue(p1);
	    xmlSchemaFreeValue(q1);
            return 1;
	} else {
            double sec;

            sec = TIME_TO_NUMBER(p1) - TIME_TO_NUMBER(q1);
	    xmlSchemaFreeValue(p1);
	    xmlSchemaFreeValue(q1);
            if (sec < 0.0)
                return -1;
            else if (sec > 0.0)
                return 1;
            
        }
        return 0;
    }

    switch (x->type) {
        case XML_SCHEMAS_DATETIME:
            xmask = 0xf;
            break;
        case XML_SCHEMAS_DATE:
            xmask = 0x7;
            break;
        case XML_SCHEMAS_GYEAR:
            xmask = 0x1;
            break;
        case XML_SCHEMAS_GMONTH:
            xmask = 0x2;
            break;
        case XML_SCHEMAS_GDAY:
            xmask = 0x3;
            break;
        case XML_SCHEMAS_GYEARMONTH:
            xmask = 0x3;
            break;
        case XML_SCHEMAS_GMONTHDAY:
            xmask = 0x6;
            break;
        case XML_SCHEMAS_TIME:
            xmask = 0x8;
            break;
        default:
            xmask = 0;
            break;
    }

    switch (y->type) {
        case XML_SCHEMAS_DATETIME:
            ymask = 0xf;
            break;
        case XML_SCHEMAS_DATE:
            ymask = 0x7;
            break;
        case XML_SCHEMAS_GYEAR:
            ymask = 0x1;
            break;
        case XML_SCHEMAS_GMONTH:
            ymask = 0x2;
            break;
        case XML_SCHEMAS_GDAY:
            ymask = 0x3;
            break;
        case XML_SCHEMAS_GYEARMONTH:
            ymask = 0x3;
            break;
        case XML_SCHEMAS_GMONTHDAY:
            ymask = 0x6;
            break;
        case XML_SCHEMAS_TIME:
            ymask = 0x8;
            break;
        default:
            ymask = 0;
            break;
    }

    xor_mask = xmask ^ ymask;           /* mark type differences */
    and_mask = xmask & ymask;           /* mark field specification */

    /* year */
    if (xor_mask & 1)
        return 2; /* indeterminate */
    else if (and_mask & 1) {
        if (x->value.date.year < y->value.date.year)
            return -1;
        else if (x->value.date.year > y->value.date.year)
            return 1;
    }

    /* month */
    if (xor_mask & 2)
        return 2; /* indeterminate */
    else if (and_mask & 2) {
        if (x->value.date.mon < y->value.date.mon)
            return -1;
        else if (x->value.date.mon > y->value.date.mon)
            return 1;
    }

    /* day */
    if (xor_mask & 4)
        return 2; /* indeterminate */
    else if (and_mask & 4) {
        if (x->value.date.day < y->value.date.day)
            return -1;
        else if (x->value.date.day > y->value.date.day)
            return 1;
    }

    /* time */
    if (xor_mask & 8)
        return 2; /* indeterminate */
    else if (and_mask & 8) {
        if (x->value.date.hour < y->value.date.hour)
            return -1;
        else if (x->value.date.hour > y->value.date.hour)
            return 1;
        else if (x->value.date.min < y->value.date.min)
            return -1;
        else if (x->value.date.min > y->value.date.min)
            return 1;
        else if (x->value.date.sec < y->value.date.sec)
            return -1;
        else if (x->value.date.sec > y->value.date.sec)
            return 1;
    }

    return 0;
}

/**
 * xmlSchemaCompareValues:
 * @x:  a first value
 * @y:  a second value
 *
 * Compare 2 values
 *
 * Returns -1 if x < y, 0 if x == y, 1 if x > y, 2 if x <> y, and -2 in
 * case of error
 */
static int
xmlSchemaCompareValues(xmlSchemaValPtr x, xmlSchemaValPtr y) {
    if ((x == NULL) || (y == NULL))
	return(-2);

    switch (x->type) {
	case XML_SCHEMAS_STRING:
	    TODO
	case XML_SCHEMAS_DECIMAL:
	    if (y->type == XML_SCHEMAS_DECIMAL)
		return(xmlSchemaCompareDecimals(x, y));
	    return(-2);
        case XML_SCHEMAS_DURATION:
	    if (y->type == XML_SCHEMAS_DURATION)
                return(xmlSchemaCompareDurations(x, y));
            return(-2);
        case XML_SCHEMAS_TIME:
        case XML_SCHEMAS_GDAY:
        case XML_SCHEMAS_GMONTH:
        case XML_SCHEMAS_GMONTHDAY:
        case XML_SCHEMAS_GYEAR:
        case XML_SCHEMAS_GYEARMONTH:
        case XML_SCHEMAS_DATE:
        case XML_SCHEMAS_DATETIME:
            if ((y->type == XML_SCHEMAS_DATETIME)  ||
                (y->type == XML_SCHEMAS_TIME)      ||
                (y->type == XML_SCHEMAS_GDAY)      ||
                (y->type == XML_SCHEMAS_GMONTH)    ||
                (y->type == XML_SCHEMAS_GMONTHDAY) ||
                (y->type == XML_SCHEMAS_GYEAR)     ||
                (y->type == XML_SCHEMAS_DATE)      ||
                (y->type == XML_SCHEMAS_GYEARMONTH))
                return (xmlSchemaCompareDates(x, y));

            return (-2);
	default:
	    TODO
    }
    return -2;
}

/**
 * xmlSchemaValidateFacet:
 * @base:  the base type
 * @facet:  the facet to check
 * @value:  the lexical repr of the value to validate
 * @val:  the precomputed value
 *
 * Check a value against a facet condition
 *
 * Returns 0 if the element is schemas valid, a positive error code
 *     number otherwise and -1 in case of internal or API error.
 */
int
xmlSchemaValidateFacet(xmlSchemaTypePtr base ATTRIBUTE_UNUSED,
	               xmlSchemaFacetPtr facet,
	               const xmlChar *value, xmlSchemaValPtr val)
{
    int ret;

    switch (facet->type) {
	case XML_SCHEMA_FACET_PATTERN:
	    ret = xmlRegexpExec(facet->regexp, value);
	    if (ret == 1)
		return(0);
	    if (ret == 0) {
		TODO /* error code */
		return(1);
	    }
	    return(ret);
	case XML_SCHEMA_FACET_MAXEXCLUSIVE:
	    ret = xmlSchemaCompareValues(val, facet->val);
	    if (ret == -2) {
		TODO /* error code */
		return(-1);
	    }
	    if (ret == -1)
		return(0);
	    /* error code */
	    return(1);
	case XML_SCHEMA_FACET_MAXINCLUSIVE:
	    ret = xmlSchemaCompareValues(val, facet->val);
	    if (ret == -2) {
		TODO /* error code */
		return(-1);
	    }
	    if ((ret == -1) || (ret == 0))
		return(0);
	    /* error code */
	    return(1);
	case XML_SCHEMA_FACET_MINEXCLUSIVE:
	    ret = xmlSchemaCompareValues(val, facet->val);
	    if (ret == -2) {
		TODO /* error code */
		return(-1);
	    }
	    if (ret == 1)
		return(0);
	    /* error code */
	    return(1);
	case XML_SCHEMA_FACET_MININCLUSIVE:
	    ret = xmlSchemaCompareValues(val, facet->val);
	    if (ret == -2) {
		TODO /* error code */
		return(-1);
	    }
	    if ((ret == 1) || (ret == 0))
		return(0);
	    /* error code */
	    return(1);
	case XML_SCHEMA_FACET_WHITESPACE:
	    TODO /* whitespaces */
	    return(0);
	case XML_SCHEMA_FACET_MAXLENGTH:
	    if ((facet->val != NULL) &&
		(facet->val->type == XML_SCHEMAS_DECIMAL) &&
		(facet->val->value.decimal.frac == 0)) {
		unsigned int len;

		if (facet->val->value.decimal.sign == 1)
		    return(1);
                len = xmlUTF8Strlen(value);
		if (len > facet->val->value.decimal.base)
		    return(1);
		return(0);
	    }
	    TODO /* error code */
	    return(1);
	case  XML_SCHEMA_FACET_ENUMERATION:
	    if ((facet->value != NULL) &&
		(xmlStrEqual(facet->value, value)))
		return(0);
	    return(1);
	default:
	    TODO
    }
    return(0);
}

#endif /* LIBXML_SCHEMAS_ENABLED */
