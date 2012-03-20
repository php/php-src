/*
  date_parse.c: Coded by Tadayoshi Funaba 2011,2012
*/

#include "ruby.h"
#include "ruby/encoding.h"
#include "ruby/re.h"
#include <ctype.h>

#define sizeof_array(o) (sizeof o / sizeof o[0])

#define f_negate(x) rb_funcall(x, rb_intern("-@"), 0)
#define f_add(x,y) rb_funcall(x, '+', 1, y)
#define f_sub(x,y) rb_funcall(x, '-', 1, y)
#define f_mul(x,y) rb_funcall(x, '*', 1, y)
#define f_div(x,y) rb_funcall(x, '/', 1, y)
#define f_idiv(x,y) rb_funcall(x, rb_intern("div"), 1, y)
#define f_mod(x,y) rb_funcall(x, '%', 1, y)
#define f_expt(x,y) rb_funcall(x, rb_intern("**"), 1, y)

#define f_lt_p(x,y) rb_funcall(x, '<', 1, y)
#define f_gt_p(x,y) rb_funcall(x, '>', 1, y)
#define f_le_p(x,y) rb_funcall(x, rb_intern("<="), 1, y)
#define f_ge_p(x,y) rb_funcall(x, rb_intern(">="), 1, y)

#define f_to_s(x) rb_funcall(x, rb_intern("to_s"), 0)

#define f_match(r,s) rb_funcall(r, rb_intern("match"), 1, s)
#define f_aref(o,i) rb_funcall(o, rb_intern("[]"), 1, i)
#define f_aref2(o,i,j) rb_funcall(o, rb_intern("[]"), 2, i, j)
#define f_begin(o,i) rb_funcall(o, rb_intern("begin"), 1, i)
#define f_end(o,i) rb_funcall(o, rb_intern("end"), 1, i)
#define f_aset(o,i,v) rb_funcall(o, rb_intern("[]="), 2, i, v)
#define f_aset2(o,i,j,v) rb_funcall(o, rb_intern("[]="), 3, i, j, v)
#define f_sub_bang(s,r,x) rb_funcall(s, rb_intern("sub!"), 2, r, x)
#define f_gsub_bang(s,r,x) rb_funcall(s, rb_intern("gsub!"), 2, r, x)

#define set_hash(k,v) rb_hash_aset(hash, ID2SYM(rb_intern(k)), v)
#define ref_hash(k) rb_hash_aref(hash, ID2SYM(rb_intern(k)))
#define del_hash(k) rb_hash_delete(hash, ID2SYM(rb_intern(k)))

#define cstr2num(s) rb_cstr_to_inum(s, 10, 0)
#define str2num(s) rb_str_to_inum(s, 10, 0)

static const char *abbr_days[] = {
    "sun", "mon", "tue", "wed",
    "thu", "fri", "sat"
};

static const char *abbr_months[] = {
    "jan", "feb", "mar", "apr", "may", "jun",
    "jul", "aug", "sep", "oct", "nov", "dec"
};

#define issign(c) ((c) == '-' || (c) == '+')
#define asp_string() rb_str_new(" ", 1)

static void
s3e(VALUE hash, VALUE y, VALUE m, VALUE d, int bc)
{
    VALUE c = Qnil;

    if (TYPE(m) != T_STRING)
	m = f_to_s(m);

    if (!NIL_P(y) && !NIL_P(m) && NIL_P(d)) {
	VALUE oy = y;
	VALUE om = m;
	VALUE od = d;

	y = od;
	m = oy;
	d = om;
    }

    if (NIL_P(y)) {
	if (!NIL_P(d) && RSTRING_LEN(d) > 2) {
	    y = d;
	    d = Qnil;
	}
	if (!NIL_P(d) && *RSTRING_PTR(d) == '\'') {
	    y = d;
	    d = Qnil;
	}
    }

    if (!NIL_P(y)) {
	const char *s, *bp, *ep;
	size_t l;

	s = RSTRING_PTR(y);
	while (!issign((unsigned char)*s) && !isdigit((unsigned char)*s))
	    s++;
	bp = s;
	if (issign((unsigned char)*s))
	    s++;
	l = strspn(s, "0123456789");
	ep = s + l;
	if (*ep) {
	    y = d;
	    d = rb_str_new(bp, ep - bp);
	}
    }

    if (!NIL_P(m)) {
	const char *s;

	s = RSTRING_PTR(m);
	if (*s == '\'' || RSTRING_LEN(m) > 2) {
	    /* us -> be */
	    VALUE oy = y;
	    VALUE om = m;
	    VALUE od = d;

	    y = om;
	    m = od;
	    d = oy;
	}
    }

    if (!NIL_P(d)) {
	const char *s;

	s = RSTRING_PTR(d);
	if (*s == '\'' || RSTRING_LEN(d) > 2) {
	    VALUE oy = y;
	    VALUE od = d;

	    y = od;
	    d = oy;
	}
    }

    if (!NIL_P(y)) {
	const char *s, *bp, *ep;
	int sign = 0;
	size_t l;
	VALUE iy;

	s = RSTRING_PTR(y);
	while (!issign((unsigned char)*s) && !isdigit((unsigned char)*s))
	    s++;
	bp = s;
	if (issign(*s)) {
	    s++;
	    sign = 1;
	}
	if (sign)
	    c = Qfalse;
	l = strspn(s, "0123456789");
	ep = s + l;
	if (l > 2)
	    c = Qfalse;
	{
	    char *buf;

	    buf = ALLOCA_N(char, ep - bp + 1);
	    memcpy(buf, bp, ep - bp);
	    buf[ep - bp] = '\0';
	    iy = cstr2num(buf);
	}
	if (bc)
	    iy = f_add(f_negate(iy), INT2FIX(1));
	set_hash("year", iy);
    }

    if (!NIL_P(m)) {
	const char *s, *bp, *ep;
	size_t l;
	VALUE im;

	s = RSTRING_PTR(m);
	while (!isdigit((unsigned char)*s))
	    s++;
	bp = s;
	l = strspn(s, "0123456789");
	ep = s + l;
	{
	    char *buf;

	    buf = ALLOCA_N(char, ep - bp + 1);
	    memcpy(buf, bp, ep - bp);
	    buf[ep - bp] = '\0';
	    im = cstr2num(buf);
	}
	set_hash("mon", im);
    }

    if (!NIL_P(d)) {
	const char *s, *bp, *ep;
	size_t l;
	VALUE id;

	s = RSTRING_PTR(d);
	while (!isdigit((unsigned char)*s))
	    s++;
	bp = s;
	l = strspn(s, "0123456789");
	ep = s + l;
	{
	    char *buf;

	    buf = ALLOCA_N(char, ep - bp + 1);
	    memcpy(buf, bp, ep - bp);
	    buf[ep - bp] = '\0';
	    id = cstr2num(buf);
	}
	set_hash("mday", id);
    }

    if (!NIL_P(c))
	set_hash("_comp", c);
}

#define DAYS "sunday|monday|tuesday|wednesday|thursday|friday|saturday"
#define MONTHS "january|february|march|april|may|june|july|august|september|october|november|december"
#define ABBR_DAYS "sun|mon|tue|wed|thu|fri|sat"
#define ABBR_MONTHS "jan|feb|mar|apr|may|jun|jul|aug|sep|oct|nov|dec"

static VALUE
regcomp(const char *source, long len, int opt)
{
    VALUE pat;

    pat = rb_reg_new(source, len, opt);
    rb_gc_register_mark_object(pat);
    return pat;
}

#define REGCOMP(pat,opt) \
{ \
    if (NIL_P(pat)) \
	pat = regcomp(pat##_source, sizeof pat##_source - 1, opt); \
}

#define REGCOMP_0(pat) REGCOMP(pat, 0)
#define REGCOMP_I(pat) REGCOMP(pat, ONIG_OPTION_IGNORECASE)

#define MATCH(s,p,c) \
{ \
    return match(s, p, hash, c); \
}

static int
match(VALUE str, VALUE pat, VALUE hash, int (*cb)(VALUE, VALUE))
{
    VALUE m;

    m = f_match(pat, str);

    if (NIL_P(m))
	return 0;

    (*cb)(m, hash);

    return 1;
}

#define SUBS(s,p,c) \
{ \
    return subs(s, p, hash, c);	\
}

static int
subs(VALUE str, VALUE pat, VALUE hash, int (*cb)(VALUE, VALUE))
{
    VALUE m;

    m = f_match(pat, str);

    if (NIL_P(m))
	return 0;

    {
	VALUE be, en;

	be = f_begin(m, INT2FIX(0));
	en = f_end(m, INT2FIX(0));
	f_aset2(str, be, LONG2NUM(NUM2LONG(en) - NUM2LONG(be)), asp_string());
	(*cb)(m, hash);
    }

    return 1;
}

struct zone {
    const char *name;
    int offset;
};

static struct zone zones_source[] = {
    {"ut",   0*3600}, {"gmt",  0*3600}, {"est", -5*3600}, {"edt", -4*3600},
    {"cst", -6*3600}, {"cdt", -5*3600}, {"mst", -7*3600}, {"mdt", -6*3600},
    {"pst", -8*3600}, {"pdt", -7*3600},
    {"a",    1*3600}, {"b",    2*3600}, {"c",    3*3600}, {"d",    4*3600},
    {"e",    5*3600}, {"f",    6*3600}, {"g",    7*3600}, {"h",    8*3600},
    {"i",    9*3600}, {"k",   10*3600}, {"l",   11*3600}, {"m",   12*3600},
    {"n",   -1*3600}, {"o",   -2*3600}, {"p",   -3*3600}, {"q",   -4*3600},
    {"r",   -5*3600}, {"s",   -6*3600}, {"t",   -7*3600}, {"u",   -8*3600},
    {"v",   -9*3600}, {"w",  -10*3600}, {"x",  -11*3600}, {"y",  -12*3600},
    {"z",    0*3600},

    {"utc",  0*3600}, {"wet",  0*3600},
    {"at",  -2*3600}, {"brst",-2*3600}, {"ndt", -(2*3600+1800)},
    {"art", -3*3600}, {"adt", -3*3600}, {"brt", -3*3600}, {"clst",-3*3600},
    {"nst", -(3*3600+1800)},
    {"ast", -4*3600}, {"clt", -4*3600},
    {"akdt",-8*3600}, {"ydt", -8*3600},
    {"akst",-9*3600}, {"hadt",-9*3600}, {"hdt", -9*3600}, {"yst", -9*3600},
    {"ahst",-10*3600},{"cat",-10*3600}, {"hast",-10*3600},{"hst",-10*3600},
    {"nt",  -11*3600},
    {"idlw",-12*3600},
    {"bst",  1*3600}, {"cet",  1*3600}, {"fwt",  1*3600}, {"met",  1*3600},
    {"mewt", 1*3600}, {"mez",  1*3600}, {"swt",  1*3600}, {"wat",  1*3600},
    {"west", 1*3600},
    {"cest", 2*3600}, {"eet",  2*3600}, {"fst",  2*3600}, {"mest", 2*3600},
    {"mesz", 2*3600}, {"sast", 2*3600}, {"sst",  2*3600},
    {"bt",   3*3600}, {"eat",  3*3600}, {"eest", 3*3600}, {"msk",  3*3600},
    {"msd",  4*3600}, {"zp4",  4*3600},
    {"zp5",  5*3600}, {"ist",  (5*3600+1800)},
    {"zp6",  6*3600},
    {"wast", 7*3600},
    {"cct",  8*3600}, {"sgt",  8*3600}, {"wadt", 8*3600},
    {"jst",  9*3600}, {"kst",  9*3600},
    {"east",10*3600}, {"gst", 10*3600},
    {"eadt",11*3600},
    {"idle",12*3600}, {"nzst",12*3600}, {"nzt", 12*3600},
    {"nzdt",13*3600},

    {"afghanistan",             16200}, {"alaskan",                -32400},
    {"arab",                    10800}, {"arabian",                 14400},
    {"arabic",                  10800}, {"atlantic",               -14400},
    {"aus central",             34200}, {"aus eastern",             36000},
    {"azores",                  -3600}, {"canada central",         -21600},
    {"cape verde",              -3600}, {"caucasus",                14400},
    {"cen. australia",          34200}, {"central america",        -21600},
    {"central asia",            21600}, {"central europe",           3600},
    {"central european",         3600}, {"central pacific",         39600},
    {"central",                -21600}, {"china",                   28800},
    {"dateline",               -43200}, {"e. africa",               10800},
    {"e. australia",            36000}, {"e. europe",                7200},
    {"e. south america",       -10800}, {"eastern",                -18000},
    {"egypt",                    7200}, {"ekaterinburg",            18000},
    {"fiji",                    43200}, {"fle",                      7200},
    {"greenland",              -10800}, {"greenwich",                   0},
    {"gtb",                      7200}, {"hawaiian",               -36000},
    {"india",                   19800}, {"iran",                    12600},
    {"jerusalem",                7200}, {"korea",                   32400},
    {"mexico",                 -21600}, {"mid-atlantic",            -7200},
    {"mountain",               -25200}, {"myanmar",                 23400},
    {"n. central asia",         21600}, {"nepal",                   20700},
    {"new zealand",             43200}, {"newfoundland",           -12600},
    {"north asia east",         28800}, {"north asia",              25200},
    {"pacific sa",             -14400}, {"pacific",                -28800},
    {"romance",                  3600}, {"russian",                 10800},
    {"sa eastern",             -10800}, {"sa pacific",             -18000},
    {"sa western",             -14400}, {"samoa",                  -39600},
    {"se asia",                 25200}, {"malay peninsula",         28800},
    {"south africa",             7200}, {"sri lanka",               21600},
    {"taipei",                  28800}, {"tasmania",                36000},
    {"tokyo",                   32400}, {"tonga",                   46800},
    {"us eastern",             -18000}, {"us mountain",            -25200},
    {"vladivostok",             36000}, {"w. australia",            28800},
    {"w. central africa",        3600}, {"w. europe",                3600},
    {"west asia",               18000}, {"west pacific",            36000},
    {"yakutsk",                 32400}
};

VALUE
date_zone_to_diff(VALUE str)
{
    VALUE offset = Qnil;

    long l, i;
    char *s, *dest, *d;
    int sp = 1;

    l = RSTRING_LEN(str);
    s = RSTRING_PTR(str);

    dest = d = ALLOCA_N(char, l + 1);

    for (i = 0; i < l; i++) {
	if (isspace((unsigned char)s[i]) || s[i] == '\0') {
	    if (!sp)
		*d++ = ' ';
	    sp = 1;
	}
	else {
	    if (isalpha((unsigned char)s[i]))
		*d++ = tolower((unsigned char)s[i]);
	    else
		*d++ = s[i];
	    sp = 0;
	}
    }
    if (d > dest) {
	if (*(d - 1) == ' ')
	    --d;
	*d = '\0';
    }
    str = rb_str_new2(dest);
    {
#define STD " standard time"
#define DST " daylight time"
	char *ss, *ds;
	long sl, dl;
	int dst = 0;

	sl = RSTRING_LEN(str) - (sizeof STD - 1);
	ss = RSTRING_PTR(str) + sl;
	dl = RSTRING_LEN(str) - (sizeof DST - 1);
	ds = RSTRING_PTR(str) + dl;

	if (sl >= 0 && strcmp(ss, STD) == 0) {
	    str = rb_str_new(RSTRING_PTR(str), sl);
	}
	else if (dl >= 0 && strcmp(ds, DST) == 0) {
	    str = rb_str_new(RSTRING_PTR(str), dl);
	    dst = 1;
	}
#undef STD
#undef DST
	else {
#define DST " dst"
	    char *ds;
	    long dl;

	    dl = RSTRING_LEN(str) - (sizeof DST - 1);
	    ds = RSTRING_PTR(str) + dl;

	    if (dl >= 0 && strcmp(ds, DST) == 0) {
		str = rb_str_new(RSTRING_PTR(str), dl);
		dst = 1;
	    }
#undef DST
	}
	{
	    static VALUE zones = Qnil;

	    if (NIL_P(zones)) {
		int i;

		zones = rb_hash_new();
		rb_gc_register_mark_object(zones);
		for (i = 0; i < (int)sizeof_array(zones_source); i++) {
		    VALUE name = rb_str_new2(zones_source[i].name);
		    VALUE offset = INT2FIX(zones_source[i].offset);
		    rb_hash_aset(zones, name, offset);
		}
	    }

	    offset = f_aref(zones, str);
	    if (!NIL_P(offset)) {
		if (dst)
		    offset = f_add(offset, INT2FIX(3600));
		goto ok;
	    }
	}
	{
	    char *s, *p;
	    VALUE sign;
	    VALUE hour = Qnil, min = Qnil, sec = Qnil;
	    VALUE str_orig;

	    s = RSTRING_PTR(str);
	    str_orig = str;

	    if (strncmp(s, "gmt", 3) == 0 ||
		strncmp(s, "utc", 3) == 0)
		s += 3;
	    if (issign(*s)) {
		sign = rb_str_new(s, 1);
		s++;

		str = rb_str_new2(s);

		if (p = strchr(s, ':')) {
		    hour = rb_str_new(s, p - s);
		    s = ++p;
		    if (p = strchr(s, ':')) {
			min = rb_str_new(s, p - s);
			s = ++p;
			if (p = strchr(s, ':')) {
			    sec = rb_str_new(s, p - s);
			}
			else
			    sec = rb_str_new2(s);
		    }
		    else
			min = rb_str_new2(s);
		    RB_GC_GUARD(str_orig);
		    goto num;
		}
		if (strpbrk(RSTRING_PTR(str), ",.")) {
		    char *a, *b;

		    a = ALLOCA_N(char, RSTRING_LEN(str) + 1);
		    strcpy(a, RSTRING_PTR(str));
		    b = strpbrk(a, ",.");
		    *b = '\0';
		    b++;

		    hour = cstr2num(a);
		    min = f_mul(rb_rational_new2
				(cstr2num(b),
				 f_expt(INT2FIX(10),
					LONG2NUM((long)strlen(b)))),
				INT2FIX(60));
		    goto num;
		}
		{
		    const char *cs = RSTRING_PTR(str);
		    long cl = RSTRING_LEN(str);

		    if (cl % 2) {
			if (cl >= 1)
			    hour = rb_str_new(&cs[0], 1);
			if (cl >= 3)
			    min  = rb_str_new(&cs[1], 2);
			if (cl >= 5)
			    min  = rb_str_new(&cs[3], 2);
		    }
		    else {
			if (cl >= 2)
			    hour = rb_str_new(&cs[0], 2);
			if (cl >= 4)
			    min  = rb_str_new(&cs[2], 2);
			if (cl >= 6)
			    sec  = rb_str_new(&cs[4], 2);
		    }
		    goto num;
		}
	      num:
		if (NIL_P(hour))
		    offset = INT2FIX(0);
		else {
		    if (TYPE(hour) == T_STRING)
			hour = str2num(hour);
		    offset = f_mul(hour, INT2FIX(3600));
		}
		if (!NIL_P(min)) {
		    if (TYPE(min) == T_STRING)
			min = str2num(min);
		    offset = f_add(offset, f_mul(min, INT2FIX(60)));
		}
		if (!NIL_P(sec))
		    offset = f_add(offset, str2num(sec));
		if (!NIL_P(sign) &&
		    RSTRING_LEN(sign) == 1 &&
		    *RSTRING_PTR(sign) == '-')
		    offset = f_negate(offset);
	    }
	}
    }
    RB_GC_GUARD(str);
  ok:
    return offset;
}

static int
day_num(VALUE s)
{
    int i;

    for (i = 0; i < (int)sizeof_array(abbr_days); i++)
	if (strncasecmp(abbr_days[i], RSTRING_PTR(s), 3) == 0)
	    break;
    return i;
}

static int
mon_num(VALUE s)
{
    int i;

    for (i = 0; i < (int)sizeof_array(abbr_months); i++)
	if (strncasecmp(abbr_months[i], RSTRING_PTR(s), 3) == 0)
	    break;
    return i + 1;
}

static int
parse_day_cb(VALUE m, VALUE hash)
{
    VALUE s;

    s = rb_reg_nth_match(1, m);
    set_hash("wday", INT2FIX(day_num(s)));
    return 1;
}

static int
parse_day(VALUE str, VALUE hash)
{
    static const char pat_source[] = "\\b(" ABBR_DAYS ")[^-\\d\\s]*";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    SUBS(str, pat, parse_day_cb);
}

static int
parse_time2_cb(VALUE m, VALUE hash)
{
    VALUE h, min, s, f, p;

    h = rb_reg_nth_match(1, m);
    h = str2num(h);

    min = rb_reg_nth_match(2, m);
    if (!NIL_P(min))
	min = str2num(min);

    s = rb_reg_nth_match(3, m);
    if (!NIL_P(s))
	s = str2num(s);

    f = rb_reg_nth_match(4, m);

    if (!NIL_P(f))
	f = rb_rational_new2(str2num(f),
			     f_expt(INT2FIX(10), LONG2NUM(RSTRING_LEN(f))));

    p = rb_reg_nth_match(5, m);

    if (!NIL_P(p)) {
	int ih = NUM2INT(h);
	ih %= 12;
	if (*RSTRING_PTR(p) == 'P' || *RSTRING_PTR(p) == 'p')
	    ih += 12;
	h = INT2FIX(ih);
    }

    set_hash("hour", h);
    if (!NIL_P(min))
	set_hash("min", min);
    if (!NIL_P(s))
	set_hash("sec", s);
    if (!NIL_P(f))
	set_hash("sec_fraction", f);

    return 1;
}

static int
parse_time_cb(VALUE m, VALUE hash)
{
    static const char pat_source[] =
	    "\\A(\\d+)h?"
	      "(?:\\s*:?\\s*(\\d+)m?"
		"(?:"
		  "\\s*:?\\s*(\\d+)(?:[,.](\\d+))?s?"
		")?"
	      ")?"
	    "(?:\\s*([ap])(?:m\\b|\\.m\\.))?";
    static VALUE pat = Qnil;
    VALUE s1, s2;

    s1 = rb_reg_nth_match(1, m);
    s2 = rb_reg_nth_match(2, m);

    if (!NIL_P(s2))
	set_hash("zone", s2);

    REGCOMP_I(pat);

    {
	VALUE m = f_match(pat, s1);

	if (NIL_P(m))
	    return 0;
	parse_time2_cb(m, hash);
    }

    return 1;
}

static int
parse_time(VALUE str, VALUE hash)
{
    static const char pat_source[] =
		"("
		   "(?:"
		     "\\d+\\s*:\\s*\\d+"
		     "(?:"
		       "\\s*:\\s*\\d+(?:[,.]\\d*)?"
		     ")?"
		   "|"
		     "\\d+\\s*h(?:\\s*\\d+m?(?:\\s*\\d+s?)?)?"
		   ")"
		   "(?:"
		     "\\s*"
		     "[ap](?:m\\b|\\.m\\.)"
		   ")?"
		 "|"
		   "\\d+\\s*[ap](?:m\\b|\\.m\\.)"
		 ")"
		 "(?:"
		   "\\s*"
		   "("
		     "(?:gmt|utc?)?[-+]\\d+(?:[,.:]\\d+(?::\\d+)?)?"
		   "|"
		     "[[:alpha:].\\s]+(?:standard|daylight)\\stime\\b"
		   "|"
		     "[[:alpha:]]+(?:\\sdst)?\\b"
		   ")"
		")?";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    SUBS(str, pat, parse_time_cb);
}

static int
parse_eu_cb(VALUE m, VALUE hash)
{
    VALUE y, mon, d, b;

    d = rb_reg_nth_match(1, m);
    mon = rb_reg_nth_match(2, m);
    b = rb_reg_nth_match(3, m);
    y = rb_reg_nth_match(4, m);

    mon = INT2FIX(mon_num(mon));

    s3e(hash, y, mon, d, !NIL_P(b) &&
	(*RSTRING_PTR(b) == 'B' ||
	 *RSTRING_PTR(b) == 'b'));
    return 1;
}

static int
parse_eu(VALUE str, VALUE hash)
{
    static const char pat_source[] =
		"'?(\\d+)[^-\\d\\s]*"
		 "\\s*"
		 "(" ABBR_MONTHS ")[^-\\d\\s']*"
		 "(?:"
		   "\\s*"
		   "(c(?:e|\\.e\\.)|b(?:ce|\\.c\\.e\\.)|a(?:d|\\.d\\.)|b(?:c|\\.c\\.))?"
		   "\\s*"
		   "('?-?\\d+(?:(?:st|nd|rd|th)\\b)?)"
		")?";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    SUBS(str, pat, parse_eu_cb);
}

static int
parse_us_cb(VALUE m, VALUE hash)
{
    VALUE y, mon, d, b;

    mon = rb_reg_nth_match(1, m);
    d = rb_reg_nth_match(2, m);
    b = rb_reg_nth_match(3, m);
    y = rb_reg_nth_match(4, m);

    mon = INT2FIX(mon_num(mon));

    s3e(hash, y, mon, d, !NIL_P(b) &&
	(*RSTRING_PTR(b) == 'B' ||
	 *RSTRING_PTR(b) == 'b'));
    return 1;
}

static int
parse_us(VALUE str, VALUE hash)
{
    static const char pat_source[] =
		"\\b(" ABBR_MONTHS ")[^-\\d\\s']*"
		 "\\s*"
		 "('?\\d+)[^-\\d\\s']*"
		 "(?:"
		   "\\s*"
		   "(c(?:e|\\.e\\.)|b(?:ce|\\.c\\.e\\.)|a(?:d|\\.d\\.)|b(?:c|\\.c\\.))?"
		   "\\s*"
		   "('?-?\\d+)"
		")?";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    SUBS(str, pat, parse_us_cb);
}

static int
parse_iso_cb(VALUE m, VALUE hash)
{
    VALUE y, mon, d;

    y = rb_reg_nth_match(1, m);
    mon = rb_reg_nth_match(2, m);
    d = rb_reg_nth_match(3, m);

    s3e(hash, y, mon, d, 0);
    return 1;
}

static int
parse_iso(VALUE str, VALUE hash)
{
    static const char pat_source[] = "('?[-+]?\\d+)-(\\d+)-('?-?\\d+)";
    static VALUE pat = Qnil;

    REGCOMP_0(pat);
    SUBS(str, pat, parse_iso_cb);
}

static int
parse_iso21_cb(VALUE m, VALUE hash)
{
    VALUE y, w, d;

    y = rb_reg_nth_match(1, m);
    w = rb_reg_nth_match(2, m);
    d = rb_reg_nth_match(3, m);

    if (!NIL_P(y))
	set_hash("cwyear", str2num(y));
    set_hash("cweek", str2num(w));
    if (!NIL_P(d))
	set_hash("cwday", str2num(d));

    return 1;
}

static int
parse_iso21(VALUE str, VALUE hash)
{
    static const char pat_source[] =
	"\\b(\\d{2}|\\d{4})?-?w(\\d{2})(?:-?(\\d))?\\b";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    SUBS(str, pat, parse_iso21_cb);
}

static int
parse_iso22_cb(VALUE m, VALUE hash)
{
    VALUE d;

    d = rb_reg_nth_match(1, m);
    set_hash("cwday", str2num(d));
    return 1;
}

static int
parse_iso22(VALUE str, VALUE hash)
{
    static const char pat_source[] = "-w-(\\d)\\b";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    SUBS(str, pat, parse_iso22_cb);
}

static int
parse_iso23_cb(VALUE m, VALUE hash)
{
    VALUE mon, d;

    mon = rb_reg_nth_match(1, m);
    d = rb_reg_nth_match(2, m);

    if (!NIL_P(mon))
	set_hash("mon", str2num(mon));
    set_hash("mday", str2num(d));

    return 1;
}

static int
parse_iso23(VALUE str, VALUE hash)
{
    static const char pat_source[] = "--(\\d{2})?-(\\d{2})\\b";
    static VALUE pat = Qnil;

    REGCOMP_0(pat);
    SUBS(str, pat, parse_iso23_cb);
}

static int
parse_iso24_cb(VALUE m, VALUE hash)
{
    VALUE mon, d;

    mon = rb_reg_nth_match(1, m);
    d = rb_reg_nth_match(2, m);

    set_hash("mon", str2num(mon));
    if (!NIL_P(d))
	set_hash("mday", str2num(d));

    return 1;
}

static int
parse_iso24(VALUE str, VALUE hash)
{
    static const char pat_source[] = "--(\\d{2})(\\d{2})?\\b";
    static VALUE pat = Qnil;

    REGCOMP_0(pat);
    SUBS(str, pat, parse_iso24_cb);
}

static int
parse_iso25_cb(VALUE m, VALUE hash)
{
    VALUE y, d;

    y = rb_reg_nth_match(1, m);
    d = rb_reg_nth_match(2, m);

    set_hash("year", str2num(y));
    set_hash("yday", str2num(d));

    return 1;
}

static int
parse_iso25(VALUE str, VALUE hash)
{
    static const char pat0_source[] = "[,.](\\d{2}|\\d{4})-\\d{3}\\b";
    static VALUE pat0 = Qnil;
    static const char pat_source[] = "\\b(\\d{2}|\\d{4})-(\\d{3})\\b";
    static VALUE pat = Qnil;

    REGCOMP_0(pat0);
    REGCOMP_0(pat);

    if (!NIL_P(f_match(pat0, str)))
	return 0;
    SUBS(str, pat, parse_iso25_cb);
}

static int
parse_iso26_cb(VALUE m, VALUE hash)
{
    VALUE d;

    d = rb_reg_nth_match(1, m);
    set_hash("yday", str2num(d));

    return 1;
}
static int
parse_iso26(VALUE str, VALUE hash)
{
    static const char pat0_source[] = "\\d-\\d{3}\\b";
    static VALUE pat0 = Qnil;
    static const char pat_source[] = "\\b-(\\d{3})\\b";
    static VALUE pat = Qnil;

    REGCOMP_0(pat0);
    REGCOMP_0(pat);

    if (!NIL_P(f_match(pat0, str)))
	return 0;
    SUBS(str, pat, parse_iso26_cb);
}

static int
parse_iso2(VALUE str, VALUE hash)
{
    if (parse_iso21(str, hash))
	goto ok;
    if (parse_iso22(str, hash))
	goto ok;
    if (parse_iso23(str, hash))
	goto ok;
    if (parse_iso24(str, hash))
	goto ok;
    if (parse_iso25(str, hash))
	goto ok;
    if (parse_iso26(str, hash))
	goto ok;
    return 0;

  ok:
    return 1;
}

static int
gengo(int c)
{
    int e;

    switch (c) {
      case 'M': case 'm': e = 1867; break;
      case 'T': case 't': e = 1911; break;
      case 'S': case 's': e = 1925; break;
      case 'H': case 'h': e = 1988; break;
      default:  e = 0; break;
    }
    return e;
}

static int
parse_jis_cb(VALUE m, VALUE hash)
{
    VALUE e, y, mon, d;
    int ep;

    e = rb_reg_nth_match(1, m);
    y = rb_reg_nth_match(2, m);
    mon = rb_reg_nth_match(3, m);
    d = rb_reg_nth_match(4, m);

    ep = gengo(*RSTRING_PTR(e));

    set_hash("year", f_add(str2num(y), INT2FIX(ep)));
    set_hash("mon", str2num(mon));
    set_hash("mday", str2num(d));

    return 1;
}

static int
parse_jis(VALUE str, VALUE hash)
{
    static const char pat_source[] = "\\b([mtsh])(\\d+)\\.(\\d+)\\.(\\d+)";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    SUBS(str, pat, parse_jis_cb);
}

static int
parse_vms11_cb(VALUE m, VALUE hash)
{
    VALUE y, mon, d;

    d = rb_reg_nth_match(1, m);
    mon = rb_reg_nth_match(2, m);
    y = rb_reg_nth_match(3, m);

    mon = INT2FIX(mon_num(mon));

    s3e(hash, y, mon, d, 0);
    return 1;
}

static int
parse_vms11(VALUE str, VALUE hash)
{
    static const char pat_source[] =
	"('?-?\\d+)-(" ABBR_MONTHS ")[^-]*"
	"-('?-?\\d+)";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    SUBS(str, pat, parse_vms11_cb);
}

static int
parse_vms12_cb(VALUE m, VALUE hash)
{
    VALUE y, mon, d;

    mon = rb_reg_nth_match(1, m);
    d = rb_reg_nth_match(2, m);
    y = rb_reg_nth_match(3, m);

    mon = INT2FIX(mon_num(mon));

    s3e(hash, y, mon, d, 0);
    return 1;
}

static int
parse_vms12(VALUE str, VALUE hash)
{
    static const char pat_source[] =
	"\\b(" ABBR_MONTHS ")[^-]*"
	"-('?-?\\d+)(?:-('?-?\\d+))?";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    SUBS(str, pat, parse_vms12_cb);
}

static int
parse_vms(VALUE str, VALUE hash)
{
    if (parse_vms11(str, hash))
	goto ok;
    if (parse_vms12(str, hash))
	goto ok;
    return 0;

  ok:
    return 1;
}

static int
parse_sla_cb(VALUE m, VALUE hash)
{
    VALUE y, mon, d;

    y = rb_reg_nth_match(1, m);
    mon = rb_reg_nth_match(2, m);
    d = rb_reg_nth_match(3, m);

    s3e(hash, y, mon, d, 0);
    return 1;
}

static int
parse_sla(VALUE str, VALUE hash)
{
    static const char pat_source[] =
	"('?-?\\d+)/\\s*('?\\d+)(?:\\D\\s*('?-?\\d+))?";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    SUBS(str, pat, parse_sla_cb);
}

static int
parse_dot_cb(VALUE m, VALUE hash)
{
    VALUE y, mon, d;

    y = rb_reg_nth_match(1, m);
    mon = rb_reg_nth_match(2, m);
    d = rb_reg_nth_match(3, m);

    s3e(hash, y, mon, d, 0);
    return 1;
}

static int
parse_dot(VALUE str, VALUE hash)
{
    static const char pat_source[] =
	"('?-?\\d+)\\.\\s*('?\\d+)\\.\\s*('?-?\\d+)";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    SUBS(str, pat, parse_dot_cb);
}

static int
parse_year_cb(VALUE m, VALUE hash)
{
    VALUE y;

    y = rb_reg_nth_match(1, m);
    set_hash("year", str2num(y));
    return 1;
}

static int
parse_year(VALUE str, VALUE hash)
{
    static const char pat_source[] = "'(\\d+)\\b";
    static VALUE pat = Qnil;

    REGCOMP_0(pat);
    SUBS(str, pat, parse_year_cb);
}

static int
parse_mon_cb(VALUE m, VALUE hash)
{
    VALUE mon;

    mon = rb_reg_nth_match(1, m);
    set_hash("mon", INT2FIX(mon_num(mon)));
    return 1;
}

static int
parse_mon(VALUE str, VALUE hash)
{
    static const char pat_source[] = "\\b(" ABBR_MONTHS ")\\S*";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    SUBS(str, pat, parse_mon_cb);
}

static int
parse_mday_cb(VALUE m, VALUE hash)
{
    VALUE d;

    d = rb_reg_nth_match(1, m);
    set_hash("mday", str2num(d));
    return 1;
}

static int
parse_mday(VALUE str, VALUE hash)
{
    static const char pat_source[] = "(\\d+)(st|nd|rd|th)\\b";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    SUBS(str, pat, parse_mday_cb);
}

static int
n2i(const char *s, long f, long w)
{
    long e, i;
    int v;

    e = f + w;
    v = 0;
    for (i = f; i < e; i++) {
	v *= 10;
	v += s[i] - '0';
    }
    return v;
}

static int
parse_ddd_cb(VALUE m, VALUE hash)
{
    VALUE s1, s2, s3, s4, s5;
    const char *cs2, *cs3, *cs5;
    long l2, l3, l4, l5;

    s1 = rb_reg_nth_match(1, m);
    s2 = rb_reg_nth_match(2, m);
    s3 = rb_reg_nth_match(3, m);
    s4 = rb_reg_nth_match(4, m);
    s5 = rb_reg_nth_match(5, m);

    cs2 = RSTRING_PTR(s2);
    l2 = RSTRING_LEN(s2);

    switch (l2) {
      case 2:
	if (NIL_P(s3) && !NIL_P(s4))
	    set_hash("sec",  INT2FIX(n2i(cs2, l2-2, 2)));
	else
	    set_hash("mday", INT2FIX(n2i(cs2,    0, 2)));
	break;
      case 4:
	if (NIL_P(s3) && !NIL_P(s4)) {
	    set_hash("sec",  INT2FIX(n2i(cs2, l2-2, 2)));
	    set_hash("min",  INT2FIX(n2i(cs2, l2-4, 2)));
	}
	else {
	    set_hash("mon",  INT2FIX(n2i(cs2,    0, 2)));
	    set_hash("mday", INT2FIX(n2i(cs2,    2, 2)));
	}
	break;
      case 6:
	if (NIL_P(s3) && !NIL_P(s4)) {
	    set_hash("sec",  INT2FIX(n2i(cs2, l2-2, 2)));
	    set_hash("min",  INT2FIX(n2i(cs2, l2-4, 2)));
	    set_hash("hour", INT2FIX(n2i(cs2, l2-6, 2)));
	}
	else {
	    int                  y = n2i(cs2,    0, 2);
	    if (!NIL_P(s1) && *RSTRING_PTR(s1) == '-')
		y = -y;
	    set_hash("year", INT2FIX(y));
	    set_hash("mon",  INT2FIX(n2i(cs2,    2, 2)));
	    set_hash("mday", INT2FIX(n2i(cs2,    4, 2)));
	}
	break;
      case 8:
      case 10:
      case 12:
      case 14:
	if (NIL_P(s3) && !NIL_P(s4)) {
	    set_hash("sec",  INT2FIX(n2i(cs2, l2-2, 2)));
	    set_hash("min",  INT2FIX(n2i(cs2, l2-4, 2)));
	    set_hash("hour", INT2FIX(n2i(cs2, l2-6, 2)));
	    set_hash("mday", INT2FIX(n2i(cs2, l2-8, 2)));
	    if (l2 >= 10)
		set_hash("mon", INT2FIX(n2i(cs2, l2-10, 2)));
	    if (l2 == 12) {
		int y = n2i(cs2, l2-12, 2);
		if (!NIL_P(s1) && *RSTRING_PTR(s1) == '-')
		    y = -y;
		set_hash("year", INT2FIX(y));
	    }
	    if (l2 == 14) {
		int y = n2i(cs2, l2-14, 4);
		if (!NIL_P(s1) && *RSTRING_PTR(s1) == '-')
		    y = -y;
		set_hash("year", INT2FIX(y));
		set_hash("_comp", Qfalse);
	    }
	}
	else {
	    int                  y = n2i(cs2,    0, 4);
	    if (!NIL_P(s1) && *RSTRING_PTR(s1) == '-')
		y = -y;
	    set_hash("year", INT2FIX(y));
	    set_hash("mon",  INT2FIX(n2i(cs2,    4, 2)));
	    set_hash("mday", INT2FIX(n2i(cs2,    6, 2)));
	    if (l2 >= 10)
		set_hash("hour", INT2FIX(n2i(cs2,    8, 2)));
	    if (l2 >= 12)
		set_hash("min",  INT2FIX(n2i(cs2,   10, 2)));
	    if (l2 >= 14)
		set_hash("sec",  INT2FIX(n2i(cs2,   12, 2)));
	    set_hash("_comp", Qfalse);
	}
	break;
      case 3:
	if (NIL_P(s3) && !NIL_P(s4)) {
	    set_hash("sec",  INT2FIX(n2i(cs2, l2-2, 2)));
	    set_hash("min",  INT2FIX(n2i(cs2, l2-3, 1)));
	}
	else
	    set_hash("yday", INT2FIX(n2i(cs2,    0, 3)));
	break;
      case 5:
	if (NIL_P(s3) && !NIL_P(s4)) {
	    set_hash("sec",  INT2FIX(n2i(cs2, l2-2, 2)));
	    set_hash("min",  INT2FIX(n2i(cs2, l2-4, 2)));
	    set_hash("hour", INT2FIX(n2i(cs2, l2-5, 1)));
	}
	else {
	    int                  y = n2i(cs2,    0, 2);
	    if (!NIL_P(s1) && *RSTRING_PTR(s1) == '-')
		y = -y;
	    set_hash("year", INT2FIX(y));
	    set_hash("yday", INT2FIX(n2i(cs2,    2, 3)));
	}
	break;
      case 7:
	if (NIL_P(s3) && !NIL_P(s4)) {
	    set_hash("sec",  INT2FIX(n2i(cs2, l2-2, 2)));
	    set_hash("min",  INT2FIX(n2i(cs2, l2-4, 2)));
	    set_hash("hour", INT2FIX(n2i(cs2, l2-6, 2)));
	    set_hash("mday", INT2FIX(n2i(cs2, l2-7, 1)));
	}
	else {
	    int                  y = n2i(cs2,    0, 4);
	    if (!NIL_P(s1) && *RSTRING_PTR(s1) == '-')
		y = -y;
	    set_hash("year", INT2FIX(y));
	    set_hash("yday", INT2FIX(n2i(cs2,    4, 3)));
	}
	break;
    }
    RB_GC_GUARD(s2);
    if (!NIL_P(s3)) {
	cs3 = RSTRING_PTR(s3);
	l3 = RSTRING_LEN(s3);

	if (!NIL_P(s4)) {
	    switch (l3) {
	      case 2:
	      case 4:
	      case 6:
		set_hash("sec", INT2FIX(n2i(cs3, l3-2, 2)));
		if (l3 >= 4)
		    set_hash("min", INT2FIX(n2i(cs3, l3-4, 2)));
		if (l3 >= 6)
		    set_hash("hour", INT2FIX(n2i(cs3, l3-6, 2)));
		break;
	    }
	}
	else {
	    switch (l3) {
	      case 2:
	      case 4:
	      case 6:
		set_hash("hour", INT2FIX(n2i(cs3, 0, 2)));
		if (l3 >= 4)
		    set_hash("min", INT2FIX(n2i(cs3, 2, 2)));
		if (l3 >= 6)
		    set_hash("sec", INT2FIX(n2i(cs3, 4, 2)));
		break;
	    }
	}
	RB_GC_GUARD(s3);
    }
    if (!NIL_P(s4)) {
	l4 = RSTRING_LEN(s4);

	set_hash("sec_fraction",
		 rb_rational_new2(str2num(s4),
				  f_expt(INT2FIX(10), LONG2NUM(l4))));
    }
    if (!NIL_P(s5)) {
	cs5 = RSTRING_PTR(s5);
	l5 = RSTRING_LEN(s5);

	set_hash("zone", s5);

	if (*cs5 == '[') {
	    char *buf = ALLOCA_N(char, l5 + 1);
	    char *s1, *s2, *s3;
	    VALUE zone;

	    memcpy(buf, cs5, l5);
	    buf[l5 - 1] = '\0';

	    s1 = buf + 1;
	    s2 = strchr(buf, ':');
	    if (s2) {
		*s2 = '\0';
		s2++;
	    }
	    if (s2)
		s3 = s2;
	    else
		s3 = s1;
	    zone = rb_str_new2(s3);
	    set_hash("zone", zone);
	    if (isdigit((unsigned char)*s1))
		*--s1 = '+';
	    set_hash("offset", date_zone_to_diff(rb_str_new2(s1)));
	}
	RB_GC_GUARD(s5);
    }

    return 1;
}

static int
parse_ddd(VALUE str, VALUE hash)
{
    static const char pat_source[] =
		"([-+]?)(\\d{2,14})"
		  "(?:"
		    "\\s*"
		    "t?"
		    "\\s*"
		    "(\\d{2,6})?(?:[,.](\\d*))?"
		  ")?"
		  "(?:"
		    "\\s*"
		    "("
		      "z\\b"
		    "|"
		      "[-+]\\d{1,4}\\b"
		    "|"
		      "\\[[-+]?\\d[^\\]]*\\]"
		    ")"
		")?";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    SUBS(str, pat, parse_ddd_cb);
}

static int
parse_bc_cb(VALUE m, VALUE hash)
{
    VALUE y;

    y = ref_hash("year");
    if (!NIL_P(y))
	set_hash("year", f_add(f_negate(y), INT2FIX(1)));

    return 1;
}

static int
parse_bc(VALUE str, VALUE hash)
{
    static const char pat_source[] =
	"\\b(bc\\b|bce\\b|b\\.c\\.|b\\.c\\.e\\.)";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    SUBS(str, pat, parse_bc_cb);
}

static int
parse_frag_cb(VALUE m, VALUE hash)
{
    VALUE s, n;

    s = rb_reg_nth_match(1, m);

    if (!NIL_P(ref_hash("hour")) && NIL_P(ref_hash("mday"))) {
	n = str2num(s);
	if (f_ge_p(n, INT2FIX(1)) &&
	    f_le_p(n, INT2FIX(31)))
	    set_hash("mday", n);
    }
    if (!NIL_P(ref_hash("mday")) && NIL_P(ref_hash("hour"))) {
	n = str2num(s);
	if (f_ge_p(n, INT2FIX(0)) &&
	    f_le_p(n, INT2FIX(24)))
	    set_hash("hour", n);
    }

    return 1;
}

static int
parse_frag(VALUE str, VALUE hash)
{
    static const char pat_source[] = "\\A\\s*(\\d{1,2})\\s*\\z";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    SUBS(str, pat, parse_frag_cb);
}

#define HAVE_ALPHA (1<<0)
#define HAVE_DIGIT (1<<1)
#define HAVE_DASH (1<<2)
#define HAVE_DOT (1<<3)
#define HAVE_SLASH (1<<4)

static unsigned
check_class(VALUE s)
{
    unsigned flags;
    long i;

    flags = 0;
    for (i = 0; i < RSTRING_LEN(s); i++) {
	if (isalpha((unsigned char)RSTRING_PTR(s)[i]))
	    flags |= HAVE_ALPHA;
	if (isdigit((unsigned char)RSTRING_PTR(s)[i]))
	    flags |= HAVE_DIGIT;
	if (RSTRING_PTR(s)[i] == '-')
	    flags |= HAVE_DASH;
	if (RSTRING_PTR(s)[i] == '.')
	    flags |= HAVE_DOT;
	if (RSTRING_PTR(s)[i] == '/')
	    flags |= HAVE_SLASH;
    }
    return flags;
}

#define HAVE_ELEM_P(x) ((check_class(str) & (x)) == (x))

VALUE
date__parse(VALUE str, VALUE comp)
{
    VALUE backref, hash;

    backref = rb_backref_get();
    rb_match_busy(backref);

    {
	static const char pat_source[] = "[^-+',./:@[:alnum:]\\[\\]]+";
	static VALUE pat = Qnil;

	REGCOMP_0(pat);
	str = rb_str_dup(str);
	f_gsub_bang(str, pat, asp_string());
    }

    hash = rb_hash_new();
    set_hash("_comp", comp);

    if (HAVE_ELEM_P(HAVE_ALPHA))
	parse_day(str, hash);
    if (HAVE_ELEM_P(HAVE_DIGIT))
	parse_time(str, hash);

    if (HAVE_ELEM_P(HAVE_ALPHA|HAVE_DIGIT))
	if (parse_eu(str, hash))
	    goto ok;
    if (HAVE_ELEM_P(HAVE_ALPHA|HAVE_DIGIT))
	if (parse_us(str, hash))
	    goto ok;
    if (HAVE_ELEM_P(HAVE_DIGIT|HAVE_DASH))
	if (parse_iso(str, hash))
	    goto ok;
    if (HAVE_ELEM_P(HAVE_DIGIT|HAVE_DOT))
	if (parse_jis(str, hash))
	    goto ok;
    if (HAVE_ELEM_P(HAVE_ALPHA|HAVE_DIGIT|HAVE_DASH))
	if (parse_vms(str, hash))
	    goto ok;
    if (HAVE_ELEM_P(HAVE_DIGIT|HAVE_SLASH))
	if (parse_sla(str, hash))
	    goto ok;
    if (HAVE_ELEM_P(HAVE_DIGIT|HAVE_DOT))
	if (parse_dot(str, hash))
	    goto ok;
    if (HAVE_ELEM_P(HAVE_DIGIT))
	if (parse_iso2(str, hash))
	    goto ok;
    if (HAVE_ELEM_P(HAVE_DIGIT))
	if (parse_year(str, hash))
	    goto ok;
    if (HAVE_ELEM_P(HAVE_ALPHA))
	if (parse_mon(str, hash))
	    goto ok;
    if (HAVE_ELEM_P(HAVE_DIGIT))
	if (parse_mday(str, hash))
	    goto ok;
    if (HAVE_ELEM_P(HAVE_DIGIT))
	if (parse_ddd(str, hash))
	    goto ok;

  ok:
    if (HAVE_ELEM_P(HAVE_ALPHA))
	parse_bc(str, hash);
    if (HAVE_ELEM_P(HAVE_DIGIT))
	parse_frag(str, hash);

    {
	if (RTEST(ref_hash("_comp"))) {
	    VALUE y;

	    y = ref_hash("cwyear");
	    if (!NIL_P(y))
		if (f_ge_p(y, INT2FIX(0)) && f_le_p(y, INT2FIX(99))) {
		    if (f_ge_p(y, INT2FIX(69)))
			set_hash("cwyear", f_add(y, INT2FIX(1900)));
		    else
			set_hash("cwyear", f_add(y, INT2FIX(2000)));
		}
	    y = ref_hash("year");
	    if (!NIL_P(y))
		if (f_ge_p(y, INT2FIX(0)) && f_le_p(y, INT2FIX(99))) {
		    if (f_ge_p(y, INT2FIX(69)))
			set_hash("year", f_add(y, INT2FIX(1900)));
		    else
			set_hash("year", f_add(y, INT2FIX(2000)));
		}
	}
    }

    del_hash("_comp");

    {
	VALUE zone = ref_hash("zone");
	if (!NIL_P(zone) && NIL_P(ref_hash("offset")))
	    set_hash("offset", date_zone_to_diff(zone));
    }

    rb_backref_set(backref);

    return hash;
}

static VALUE
comp_year69(VALUE y)
{
    if (f_ge_p(y, INT2FIX(69)))
	return f_add(y, INT2FIX(1900));
    return f_add(y, INT2FIX(2000));
}

static VALUE
comp_year50(VALUE y)
{
    if (f_ge_p(y, INT2FIX(50)))
	return f_add(y, INT2FIX(1900));
    return f_add(y, INT2FIX(2000));
}

static VALUE
sec_fraction(VALUE f)
{
    return rb_rational_new2(str2num(f),
			    f_expt(INT2FIX(10),
				   LONG2NUM(RSTRING_LEN(f))));
}

#define SNUM 14

static int
iso8601_ext_datetime_cb(VALUE m, VALUE hash)
{
    VALUE s[SNUM + 1], y;

    {
	int i;
	s[0] = Qnil;
	for (i = 1; i <= SNUM; i++)
	    s[i] = rb_reg_nth_match(i, m);
    }

    if (!NIL_P(s[3])) {
	set_hash("mday", str2num(s[3]));
	if (strcmp(RSTRING_PTR(s[1]), "-") != 0) {
	    y = str2num(s[1]);
	    if (RSTRING_LEN(s[1]) < 4)
		y = comp_year69(y);
	    set_hash("year", y);
	}
	if (NIL_P(s[2])) {
	    if (strcmp(RSTRING_PTR(s[1]), "-") != 0)
		return 0;
	}
	else
	    set_hash("mon", str2num(s[2]));
    }
    else if (!NIL_P(s[5])) {
	set_hash("yday", str2num(s[5]));
	if (!NIL_P(s[4])) {
	    y = str2num(s[4]);
	    if (RSTRING_LEN(s[4]) < 4)
		y = comp_year69(y);
	    set_hash("year", y);
	}
    }
    else if (!NIL_P(s[8])) {
	set_hash("cweek", str2num(s[7]));
	set_hash("cwday", str2num(s[8]));
	if (!NIL_P(s[6])) {
	    y = str2num(s[6]);
	    if (RSTRING_LEN(s[6]) < 4)
		y = comp_year69(y);
	    set_hash("cwyear", y);
	}
    }
    else if (!NIL_P(s[9])) {
	set_hash("cwday", str2num(s[9]));
    }
    if (!NIL_P(s[10])) {
	set_hash("hour", str2num(s[10]));
	set_hash("min", str2num(s[11]));
	if (!NIL_P(s[12]))
	    set_hash("sec", str2num(s[12]));
    }
    if (!NIL_P(s[13])) {
	set_hash("sec_fraction", sec_fraction(s[13]));
    }
    if (!NIL_P(s[14])) {
	set_hash("zone", s[14]);
	set_hash("offset", date_zone_to_diff(s[14]));
    }

    return 1;
}

static int
iso8601_ext_datetime(VALUE str, VALUE hash)
{
    static const char pat_source[] =
	"\\A\\s*(?:([-+]?\\d{2,}|-)-(\\d{2})?-(\\d{2})|"
		"([-+]?\\d{2,})?-(\\d{3})|"
		"(\\d{4}|\\d{2})?-w(\\d{2})-(\\d)|"
		"-w-(\\d))"
	"(?:t"
	"(\\d{2}):(\\d{2})(?::(\\d{2})(?:[,.](\\d+))?)?"
	"(z|[-+]\\d{2}(?::?\\d{2})?)?)?\\s*\\z";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    MATCH(str, pat, iso8601_ext_datetime_cb);
}

#undef SNUM
#define SNUM 17

static int
iso8601_bas_datetime_cb(VALUE m, VALUE hash)
{
    VALUE s[SNUM + 1], y;

    {
	int i;
	s[0] = Qnil;
	for (i = 1; i <= SNUM; i++)
	    s[i] = rb_reg_nth_match(i, m);
    }

    if (!NIL_P(s[3])) {
	set_hash("mday", str2num(s[3]));
	if (strcmp(RSTRING_PTR(s[1]), "--") != 0) {
	    y = str2num(s[1]);
	    if (RSTRING_LEN(s[1]) < 4)
		y = comp_year69(y);
	    set_hash("year", y);
	}
	if (*RSTRING_PTR(s[2]) == '-') {
	    if (strcmp(RSTRING_PTR(s[1]), "--") != 0)
		return 0;
	}
	else
	    set_hash("mon", str2num(s[2]));
    }
    else if (!NIL_P(s[5])) {
	set_hash("yday", str2num(s[5]));
	y = str2num(s[4]);
	if (RSTRING_LEN(s[4]) < 4)
	    y = comp_year69(y);
	set_hash("year", y);
    }
    else if (!NIL_P(s[6])) {
	set_hash("yday", str2num(s[6]));
    }
    else if (!NIL_P(s[9])) {
	set_hash("cweek", str2num(s[8]));
	set_hash("cwday", str2num(s[9]));
	y = str2num(s[7]);
	if (RSTRING_LEN(s[7]) < 4)
	    y = comp_year69(y);
	set_hash("cwyear", y);
    }
    else if (!NIL_P(s[11])) {
	set_hash("cweek", str2num(s[10]));
	set_hash("cwday", str2num(s[11]));
    }
    else if (!NIL_P(s[12])) {
	set_hash("cwday", str2num(s[12]));
    }
    if (!NIL_P(s[13])) {
	set_hash("hour", str2num(s[13]));
	set_hash("min", str2num(s[14]));
	if (!NIL_P(s[15]))
	    set_hash("sec", str2num(s[15]));
    }
    if (!NIL_P(s[16])) {
	set_hash("sec_fraction", sec_fraction(s[16]));
    }
    if (!NIL_P(s[17])) {
	set_hash("zone", s[17]);
	set_hash("offset", date_zone_to_diff(s[17]));
    }

    return 1;
}

static int
iso8601_bas_datetime(VALUE str, VALUE hash)
{
    static const char pat_source[] =
	"\\A\\s*(?:([-+]?(?:\\d{4}|\\d{2})|--)(\\d{2}|-)(\\d{2})|"
		   "([-+]?(?:\\d{4}|\\d{2}))(\\d{3})|"
		   "-(\\d{3})|"
		   "(\\d{4}|\\d{2})w(\\d{2})(\\d)|"
		   "-w(\\d{2})(\\d)|"
		   "-w-(\\d))"
	"(?:t?"
	"(\\d{2})(\\d{2})(?:(\\d{2})(?:[,.](\\d+))?)?"
	"(z|[-+]\\d{2}(?:\\d{2})?)?)?\\s*\\z";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    MATCH(str, pat, iso8601_bas_datetime_cb);
}

#undef SNUM
#define SNUM 5

static int
iso8601_ext_time_cb(VALUE m, VALUE hash)
{
    VALUE s[SNUM + 1];

    {
	int i;
	s[0] = Qnil;
	for (i = 1; i <= SNUM; i++)
	    s[i] = rb_reg_nth_match(i, m);
    }

    set_hash("hour", str2num(s[1]));
    set_hash("min", str2num(s[2]));
    if (!NIL_P(s[3]))
	set_hash("sec", str2num(s[3]));
    if (!NIL_P(s[4]))
	set_hash("sec_fraction", sec_fraction(s[4]));
    if (!NIL_P(s[5])) {
	set_hash("zone", s[5]);
	set_hash("offset", date_zone_to_diff(s[5]));
    }

    return 1;
}

#define iso8601_bas_time_cb iso8601_ext_time_cb

static int
iso8601_ext_time(VALUE str, VALUE hash)
{
    static const char pat_source[] =
	"\\A\\s*(?:(\\d{2}):(\\d{2})(?::(\\d{2})(?:[,.](\\d+))?)?"
	"(z|[-+]\\d{2}(:?\\d{2})?)?)?\\s*\\z";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    MATCH(str, pat, iso8601_ext_time_cb);
}

static int
iso8601_bas_time(VALUE str, VALUE hash)
{
    static const char pat_source[] =
	"\\A\\s*(?:(\\d{2})(\\d{2})(?:(\\d{2})(?:[,.](\\d+))?)?"
	"(z|[-+]\\d{2}(\\d{2})?)?)?\\s*\\z";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    MATCH(str, pat, iso8601_bas_time_cb);
}

VALUE
date__iso8601(VALUE str)
{
    VALUE backref, hash;

    backref = rb_backref_get();
    rb_match_busy(backref);

    hash = rb_hash_new();

    if (iso8601_ext_datetime(str, hash))
	goto ok;
    if (iso8601_bas_datetime(str, hash))
	goto ok;
    if (iso8601_ext_time(str, hash))
	goto ok;
    if (iso8601_bas_time(str, hash))
	goto ok;

  ok:
    rb_backref_set(backref);

    return hash;
}

#undef SNUM
#define SNUM 8

static int
rfc3339_cb(VALUE m, VALUE hash)
{
    VALUE s[SNUM + 1];

    {
	int i;
	s[0] = Qnil;
	for (i = 1; i <= SNUM; i++)
	    s[i] = rb_reg_nth_match(i, m);
    }

    set_hash("year", str2num(s[1]));
    set_hash("mon", str2num(s[2]));
    set_hash("mday", str2num(s[3]));
    set_hash("hour", str2num(s[4]));
    set_hash("min", str2num(s[5]));
    set_hash("sec", str2num(s[6]));
    set_hash("zone", s[8]);
    set_hash("offset", date_zone_to_diff(s[8]));
    if (!NIL_P(s[7]))
	set_hash("sec_fraction", sec_fraction(s[7]));

    return 1;
}

static int
rfc3339(VALUE str, VALUE hash)
{
    static const char pat_source[] =
	"\\A\\s*(-?\\d{4})-(\\d{2})-(\\d{2})"
	"(?:t|\\s)"
	"(\\d{2}):(\\d{2}):(\\d{2})(?:\\.(\\d+))?"
	"(z|[-+]\\d{2}:\\d{2})\\s*\\z";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    MATCH(str, pat, rfc3339_cb);
}

VALUE
date__rfc3339(VALUE str)
{
    VALUE backref, hash;

    backref = rb_backref_get();
    rb_match_busy(backref);

    hash = rb_hash_new();
    rfc3339(str, hash);
    rb_backref_set(backref);
    return hash;
}

#undef SNUM
#define SNUM 8

static int
xmlschema_datetime_cb(VALUE m, VALUE hash)
{
    VALUE s[SNUM + 1];

    {
	int i;
	s[0] = Qnil;
	for (i = 1; i <= SNUM; i++)
	    s[i] = rb_reg_nth_match(i, m);
    }

    set_hash("year", str2num(s[1]));
    if (!NIL_P(s[2]))
	set_hash("mon", str2num(s[2]));
    if (!NIL_P(s[3]))
	set_hash("mday", str2num(s[3]));
    if (!NIL_P(s[4]))
	set_hash("hour", str2num(s[4]));
    if (!NIL_P(s[5]))
	set_hash("min", str2num(s[5]));
    if (!NIL_P(s[6]))
	set_hash("sec", str2num(s[6]));
    if (!NIL_P(s[7]))
	set_hash("sec_fraction", sec_fraction(s[7]));
    if (!NIL_P(s[8])) {
	set_hash("zone", s[8]);
	set_hash("offset", date_zone_to_diff(s[8]));
    }

    return 1;
}

static int
xmlschema_datetime(VALUE str, VALUE hash)
{
    static const char pat_source[] =
	"\\A\\s*(-?\\d{4,})(?:-(\\d{2})(?:-(\\d{2}))?)?"
	"(?:t"
	  "(\\d{2}):(\\d{2}):(\\d{2})(?:\\.(\\d+))?)?"
	"(z|[-+]\\d{2}:\\d{2})?\\s*\\z";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    MATCH(str, pat, xmlschema_datetime_cb);
}

#undef SNUM
#define SNUM 5

static int
xmlschema_time_cb(VALUE m, VALUE hash)
{
    VALUE s[SNUM + 1];

    {
	int i;
	s[0] = Qnil;
	for (i = 1; i <= SNUM; i++)
	    s[i] = rb_reg_nth_match(i, m);
    }

    set_hash("hour", str2num(s[1]));
    set_hash("min", str2num(s[2]));
    if (!NIL_P(s[3]))
	set_hash("sec", str2num(s[3]));
    if (!NIL_P(s[4]))
	set_hash("sec_fraction", sec_fraction(s[4]));
    if (!NIL_P(s[5])) {
	set_hash("zone", s[5]);
	set_hash("offset", date_zone_to_diff(s[5]));
    }

    return 1;
}

static int
xmlschema_time(VALUE str, VALUE hash)
{
    static const char pat_source[] =
	"\\A\\s*(\\d{2}):(\\d{2}):(\\d{2})(?:\\.(\\d+))?"
	"(z|[-+]\\d{2}:\\d{2})?\\s*\\z";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    MATCH(str, pat, xmlschema_time_cb);
}

#undef SNUM
#define SNUM 4

static int
xmlschema_trunc_cb(VALUE m, VALUE hash)
{
    VALUE s[SNUM + 1];

    {
	int i;
	s[0] = Qnil;
	for (i = 1; i <= SNUM; i++)
	    s[i] = rb_reg_nth_match(i, m);
    }

    if (!NIL_P(s[1]))
	set_hash("mon", str2num(s[1]));
    if (!NIL_P(s[2]))
	set_hash("mday", str2num(s[2]));
    if (!NIL_P(s[3]))
	set_hash("mday", str2num(s[3]));
    if (!NIL_P(s[4])) {
	set_hash("zone", s[4]);
	set_hash("offset", date_zone_to_diff(s[4]));
    }

    return 1;
}

static int
xmlschema_trunc(VALUE str, VALUE hash)
{
    static const char pat_source[] =
	"\\A\\s*(?:--(\\d{2})(?:-(\\d{2}))?|---(\\d{2}))"
	"(z|[-+]\\d{2}:\\d{2})?\\s*\\z";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    MATCH(str, pat, xmlschema_trunc_cb);
}

VALUE
date__xmlschema(VALUE str)
{
    VALUE backref, hash;

    backref = rb_backref_get();
    rb_match_busy(backref);

    hash = rb_hash_new();

    if (xmlschema_datetime(str, hash))
	goto ok;
    if (xmlschema_time(str, hash))
	goto ok;
    if (xmlschema_trunc(str, hash))
	goto ok;

  ok:
    rb_backref_set(backref);

    return hash;
}

#undef SNUM
#define SNUM 8

static int
rfc2822_cb(VALUE m, VALUE hash)
{
    VALUE s[SNUM + 1], y;

    {
	int i;
	s[0] = Qnil;
	for (i = 1; i <= SNUM; i++)
	    s[i] = rb_reg_nth_match(i, m);
    }

    set_hash("wday", INT2FIX(day_num(s[1])));
    set_hash("mday", str2num(s[2]));
    set_hash("mon", INT2FIX(mon_num(s[3])));
    y = str2num(s[4]);
    if (RSTRING_LEN(s[4]) < 4)
	y = comp_year50(y);
    set_hash("year", y);
    set_hash("hour", str2num(s[5]));
    set_hash("min", str2num(s[6]));
    if (!NIL_P(s[7]))
	set_hash("sec", str2num(s[7]));
    set_hash("zone", s[8]);
    set_hash("offset", date_zone_to_diff(s[8]));

    return 1;
}

static int
rfc2822(VALUE str, VALUE hash)
{
    static const char pat_source[] =
	"\\A\\s*(?:(" ABBR_DAYS ")\\s*,\\s+)?"
	"(\\d{1,2})\\s+"
	"(" ABBR_MONTHS ")\\s+"
	"(-?\\d{2,})\\s+"
	"(\\d{2}):(\\d{2})(?::(\\d{2}))?\\s*"
	"([-+]\\d{4}|ut|gmt|e[sd]t|c[sd]t|m[sd]t|p[sd]t|[a-ik-z])\\s*\\z";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    MATCH(str, pat, rfc2822_cb);
}

VALUE
date__rfc2822(VALUE str)
{
    VALUE backref, hash;

    backref = rb_backref_get();
    rb_match_busy(backref);

    hash = rb_hash_new();
    rfc2822(str, hash);
    rb_backref_set(backref);
    return hash;
}

#undef SNUM
#define SNUM 8

static int
httpdate_type1_cb(VALUE m, VALUE hash)
{
    VALUE s[SNUM + 1];

    {
	int i;
	s[0] = Qnil;
	for (i = 1; i <= SNUM; i++)
	    s[i] = rb_reg_nth_match(i, m);
    }

    set_hash("wday", INT2FIX(day_num(s[1])));
    set_hash("mday", str2num(s[2]));
    set_hash("mon", INT2FIX(mon_num(s[3])));
    set_hash("year", str2num(s[4]));
    set_hash("hour", str2num(s[5]));
    set_hash("min", str2num(s[6]));
    set_hash("sec", str2num(s[7]));
    set_hash("zone", s[8]);
    set_hash("offset", INT2FIX(0));

    return 1;
}

static int
httpdate_type1(VALUE str, VALUE hash)
{
    static const char pat_source[] =
	"\\A\\s*(" ABBR_DAYS ")\\s*,\\s+"
	"(\\d{2})\\s+"
	"(" ABBR_MONTHS ")\\s+"
	"(-?\\d{4})\\s+"
	"(\\d{2}):(\\d{2}):(\\d{2})\\s+"
	"(gmt)\\s*\\z";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    MATCH(str, pat, httpdate_type1_cb);
}

#undef SNUM
#define SNUM 8

static int
httpdate_type2_cb(VALUE m, VALUE hash)
{
    VALUE s[SNUM + 1], y;

    {
	int i;
	s[0] = Qnil;
	for (i = 1; i <= SNUM; i++)
	    s[i] = rb_reg_nth_match(i, m);
    }

    set_hash("wday", INT2FIX(day_num(s[1])));
    set_hash("mday", str2num(s[2]));
    set_hash("mon", INT2FIX(mon_num(s[3])));
    y = str2num(s[4]);
    if (f_ge_p(y, INT2FIX(0)) && f_le_p(y, INT2FIX(99)))
	y = comp_year69(y);
    set_hash("year", y);
    set_hash("hour", str2num(s[5]));
    set_hash("min", str2num(s[6]));
    set_hash("sec", str2num(s[7]));
    set_hash("zone", s[8]);
    set_hash("offset", INT2FIX(0));

    return 1;
}

static int
httpdate_type2(VALUE str, VALUE hash)
{
    static const char pat_source[] =
	"\\A\\s*(" DAYS ")\\s*,\\s+"
	"(\\d{2})\\s*-\\s*"
	"(" ABBR_MONTHS ")\\s*-\\s*"
	"(\\d{2})\\s+"
	"(\\d{2}):(\\d{2}):(\\d{2})\\s+"
	"(gmt)\\s*\\z";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    MATCH(str, pat, httpdate_type2_cb);
}

#undef SNUM
#define SNUM 7

static int
httpdate_type3_cb(VALUE m, VALUE hash)
{
    VALUE s[SNUM + 1];

    {
	int i;
	s[0] = Qnil;
	for (i = 1; i <= SNUM; i++)
	    s[i] = rb_reg_nth_match(i, m);
    }

    set_hash("wday", INT2FIX(day_num(s[1])));
    set_hash("mon", INT2FIX(mon_num(s[2])));
    set_hash("mday", str2num(s[3]));
    set_hash("hour", str2num(s[4]));
    set_hash("min", str2num(s[5]));
    set_hash("sec", str2num(s[6]));
    set_hash("year", str2num(s[7]));

    return 1;
}

static int
httpdate_type3(VALUE str, VALUE hash)
{
    static const char pat_source[] =
	"\\A\\s*(" ABBR_DAYS ")\\s+"
	"(" ABBR_MONTHS ")\\s+"
	"(\\d{1,2})\\s+"
	"(\\d{2}):(\\d{2}):(\\d{2})\\s+"
	"(\\d{4})\\s*\\z";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    MATCH(str, pat, httpdate_type3_cb);
}

VALUE
date__httpdate(VALUE str)
{
    VALUE backref, hash;

    backref = rb_backref_get();
    rb_match_busy(backref);

    hash = rb_hash_new();

    if (httpdate_type1(str, hash))
	goto ok;
    if (httpdate_type2(str, hash))
	goto ok;
    if (httpdate_type3(str, hash))
	goto ok;

  ok:
    rb_backref_set(backref);

    return hash;
}

#undef SNUM
#define SNUM 9

static int
jisx0301_cb(VALUE m, VALUE hash)
{
    VALUE s[SNUM + 1];
    int ep;

    {
	int i;
	s[0] = Qnil;
	for (i = 1; i <= SNUM; i++)
	    s[i] = rb_reg_nth_match(i, m);
    }

    ep = gengo(NIL_P(s[1]) ? 'h' : *RSTRING_PTR(s[1]));
    set_hash("year", f_add(str2num(s[2]), INT2FIX(ep)));
    set_hash("mon", str2num(s[3]));
    set_hash("mday", str2num(s[4]));
    if (!NIL_P(s[5])) {
	set_hash("hour", str2num(s[5]));
	if (!NIL_P(s[6]))
	    set_hash("min", str2num(s[6]));
	if (!NIL_P(s[7]))
	    set_hash("sec", str2num(s[7]));
    }
    if (!NIL_P(s[8]))
	set_hash("sec_fraction", sec_fraction(s[8]));
    if (!NIL_P(s[9])) {
	set_hash("zone", s[9]);
	set_hash("offset", date_zone_to_diff(s[9]));
    }

    return 1;
}

static int
jisx0301(VALUE str, VALUE hash)
{
    static const char pat_source[] =
	"\\A\\s*([mtsh])?(\\d{2})\\.(\\d{2})\\.(\\d{2})"
	"(?:t"
	"(?:(\\d{2}):(\\d{2})(?::(\\d{2})(?:[,.](\\d*))?)?"
	"(z|[-+]\\d{2}(?::?\\d{2})?)?)?)?\\s*\\z";
    static VALUE pat = Qnil;

    REGCOMP_I(pat);
    MATCH(str, pat, jisx0301_cb);
}

VALUE
date__jisx0301(VALUE str)
{
    VALUE backref, hash;

    backref = rb_backref_get();
    rb_match_busy(backref);

    hash = rb_hash_new();
    if (jisx0301(str, hash))
	goto ok;
    hash = date__iso8601(str);

  ok:
    rb_backref_set(backref);
    return hash;
}

/*
Local variables:
c-file-style: "ruby"
End:
*/
