/* This file is generated automatically by configure. */

/* declarations for the big5 character set, filled in by configure */
extern uchar ctype_big5[], to_lower_big5[], to_upper_big5[], sort_order_big5[];
extern int     my_strcoll_big5(const uchar *, const uchar *);
extern int     my_strxfrm_big5(uchar *, const uchar *, int);
extern int     my_strnncoll_big5(const uchar *, int, const uchar *, int);
extern int     my_strnxfrm_big5(uchar *, const uchar *, int, int);
extern my_bool my_like_range_big5(const char *, uint, pchar, uint,
                          char *, char *, uint *, uint *);
extern int     ismbchar_big5(const char *, const char *);
extern my_bool ismbhead_big5(uint);
extern int     mbcharlen_big5(uint);

/* declarations for the czech character set, filled in by configure */
extern uchar ctype_czech[], to_lower_czech[], to_upper_czech[], sort_order_czech[];
extern int     my_strcoll_czech(const uchar *, const uchar *);
extern int     my_strxfrm_czech(uchar *, const uchar *, int);
extern int     my_strnncoll_czech(const uchar *, int, const uchar *, int);
extern int     my_strnxfrm_czech(uchar *, const uchar *, int, int);
extern my_bool my_like_range_czech(const char *, uint, pchar, uint,
                          char *, char *, uint *, uint *);

/* declarations for the euc_kr character set, filled in by configure */
extern uchar ctype_euc_kr[], to_lower_euc_kr[], to_upper_euc_kr[], sort_order_euc_kr[];
extern int     ismbchar_euc_kr(const char *, const char *);
extern my_bool ismbhead_euc_kr(uint);
extern int     mbcharlen_euc_kr(uint);

/* declarations for the gb2312 character set, filled in by configure */
extern uchar ctype_gb2312[], to_lower_gb2312[], to_upper_gb2312[], sort_order_gb2312[];
extern int     ismbchar_gb2312(const char *, const char *);
extern my_bool ismbhead_gb2312(uint);
extern int     mbcharlen_gb2312(uint);

/* declarations for the gbk character set, filled in by configure */
extern uchar ctype_gbk[], to_lower_gbk[], to_upper_gbk[], sort_order_gbk[];
extern int     my_strcoll_gbk(const uchar *, const uchar *);
extern int     my_strxfrm_gbk(uchar *, const uchar *, int);
extern int     my_strnncoll_gbk(const uchar *, int, const uchar *, int);
extern int     my_strnxfrm_gbk(uchar *, const uchar *, int, int);
extern my_bool my_like_range_gbk(const char *, uint, pchar, uint,
                          char *, char *, uint *, uint *);
extern int     ismbchar_gbk(const char *, const char *);
extern my_bool ismbhead_gbk(uint);
extern int     mbcharlen_gbk(uint);

/* declarations for the ujis character set, filled in by configure */
extern uchar ctype_ujis[], to_lower_ujis[], to_upper_ujis[], sort_order_ujis[];
extern int     ismbchar_ujis(const char *, const char *);
extern my_bool ismbhead_ujis(uint);
extern int     mbcharlen_ujis(uint);

/* declarations for the sjis character set, filled in by configure */
extern uchar ctype_sjis[], to_lower_sjis[], to_upper_sjis[], sort_order_sjis[];
extern int     my_strcoll_sjis(const uchar *, const uchar *);
extern int     my_strxfrm_sjis(uchar *, const uchar *, int);
extern int     my_strnncoll_sjis(const uchar *, int, const uchar *, int);
extern int     my_strnxfrm_sjis(uchar *, const uchar *, int, int);
extern my_bool my_like_range_sjis(const char *, uint, pchar, uint,
                          char *, char *, uint *, uint *);
extern int     ismbchar_sjis(const char *, const char *);
extern my_bool ismbhead_sjis(uint);
extern int     mbcharlen_sjis(uint);

/* declarations for the tis620 character set, filled in by configure */
extern uchar ctype_tis620[], to_lower_tis620[], to_upper_tis620[], sort_order_tis620[];
extern int     my_strcoll_tis620(const uchar *, const uchar *);
extern int     my_strxfrm_tis620(uchar *, const uchar *, int);
extern int     my_strnncoll_tis620(const uchar *, int, const uchar *, int);
extern int     my_strnxfrm_tis620(uchar *, const uchar *, int, int);
extern my_bool my_like_range_tis620(const char *, uint, pchar, uint,
                          char *, char *, uint *, uint *);

CHARSET_INFO compiled_charsets[] = {

  /* this information is filled in by configure */
  {
      8,    /* number */
    "latin1",     /* name */
    ctype_latin1,
    to_lower_latin1,
    to_upper_latin1,
    sort_order_latin1,
    0,          /* strxfrm_multiply */
    NULL,       /* strcoll    */
    NULL,       /* strxfrm    */
    NULL,       /* strnncoll  */
    NULL,       /* strnxfrm   */
    NULL,       /* like_range */
    0,          /* mbmaxlen  */
    NULL,       /* ismbchar  */
    NULL,       /* ismbhead  */
    NULL        /* mbcharlen */
  },

  /* this information is filled in by configure */
  {
      1,    /* number */
    "big5",     /* name */
    ctype_big5,
    to_lower_big5,
    to_upper_big5,
    sort_order_big5,
    1,      /* strxfrm_multiply */
    my_strcoll_big5,
    my_strxfrm_big5,
    my_strnncoll_big5,
    my_strnxfrm_big5,
    my_like_range_big5,
    2,      /* mbmaxlen */
    ismbchar_big5,
    ismbhead_big5,
    mbcharlen_big5
  },

  /* this information is filled in by configure */
  {
     14,    /* number */
    "cp1251",     /* name */
    ctype_cp1251,
    to_lower_cp1251,
    to_upper_cp1251,
    sort_order_cp1251,
    0,          /* strxfrm_multiply */
    NULL,       /* strcoll    */
    NULL,       /* strxfrm    */
    NULL,       /* strnncoll  */
    NULL,       /* strnxfrm   */
    NULL,       /* like_range */
    0,          /* mbmaxlen  */
    NULL,       /* ismbchar  */
    NULL,       /* ismbhead  */
    NULL        /* mbcharlen */
  },

  /* this information is filled in by configure */
  {
     29,    /* number */
    "cp1257",     /* name */
    ctype_cp1257,
    to_lower_cp1257,
    to_upper_cp1257,
    sort_order_cp1257,
    0,          /* strxfrm_multiply */
    NULL,       /* strcoll    */
    NULL,       /* strxfrm    */
    NULL,       /* strnncoll  */
    NULL,       /* strnxfrm   */
    NULL,       /* like_range */
    0,          /* mbmaxlen  */
    NULL,       /* ismbchar  */
    NULL,       /* ismbhead  */
    NULL        /* mbcharlen */
  },

  /* this information is filled in by configure */
  {
     27,    /* number */
    "croat",     /* name */
    ctype_croat,
    to_lower_croat,
    to_upper_croat,
    sort_order_croat,
    0,          /* strxfrm_multiply */
    NULL,       /* strcoll    */
    NULL,       /* strxfrm    */
    NULL,       /* strnncoll  */
    NULL,       /* strnxfrm   */
    NULL,       /* like_range */
    0,          /* mbmaxlen  */
    NULL,       /* ismbchar  */
    NULL,       /* ismbhead  */
    NULL        /* mbcharlen */
  },

  /* this information is filled in by configure */
  {
      2,    /* number */
    "czech",     /* name */
    ctype_czech,
    to_lower_czech,
    to_upper_czech,
    sort_order_czech,
    4,      /* strxfrm_multiply */
    my_strcoll_czech,
    my_strxfrm_czech,
    my_strnncoll_czech,
    my_strnxfrm_czech,
    my_like_range_czech,
    0,          /* mbmaxlen  */
    NULL,       /* ismbchar  */
    NULL,       /* ismbhead  */
    NULL        /* mbcharlen */
  },

  /* this information is filled in by configure */
  {
     15,    /* number */
    "danish",     /* name */
    ctype_danish,
    to_lower_danish,
    to_upper_danish,
    sort_order_danish,
    0,          /* strxfrm_multiply */
    NULL,       /* strcoll    */
    NULL,       /* strxfrm    */
    NULL,       /* strnncoll  */
    NULL,       /* strnxfrm   */
    NULL,       /* like_range */
    0,          /* mbmaxlen  */
    NULL,       /* ismbchar  */
    NULL,       /* ismbhead  */
    NULL        /* mbcharlen */
  },

  /* this information is filled in by configure */
  {
      3,    /* number */
    "dec8",     /* name */
    ctype_dec8,
    to_lower_dec8,
    to_upper_dec8,
    sort_order_dec8,
    0,          /* strxfrm_multiply */
    NULL,       /* strcoll    */
    NULL,       /* strxfrm    */
    NULL,       /* strnncoll  */
    NULL,       /* strnxfrm   */
    NULL,       /* like_range */
    0,          /* mbmaxlen  */
    NULL,       /* ismbchar  */
    NULL,       /* ismbhead  */
    NULL        /* mbcharlen */
  },

  /* this information is filled in by configure */
  {
      4,    /* number */
    "dos",     /* name */
    ctype_dos,
    to_lower_dos,
    to_upper_dos,
    sort_order_dos,
    0,          /* strxfrm_multiply */
    NULL,       /* strcoll    */
    NULL,       /* strxfrm    */
    NULL,       /* strnncoll  */
    NULL,       /* strnxfrm   */
    NULL,       /* like_range */
    0,          /* mbmaxlen  */
    NULL,       /* ismbchar  */
    NULL,       /* ismbhead  */
    NULL        /* mbcharlen */
  },

  /* this information is filled in by configure */
  {
     20,    /* number */
    "estonia",     /* name */
    ctype_estonia,
    to_lower_estonia,
    to_upper_estonia,
    sort_order_estonia,
    0,          /* strxfrm_multiply */
    NULL,       /* strcoll    */
    NULL,       /* strxfrm    */
    NULL,       /* strnncoll  */
    NULL,       /* strnxfrm   */
    NULL,       /* like_range */
    0,          /* mbmaxlen  */
    NULL,       /* ismbchar  */
    NULL,       /* ismbhead  */
    NULL        /* mbcharlen */
  },

  /* this information is filled in by configure */
  {
     19,    /* number */
    "euc_kr",     /* name */
    ctype_euc_kr,
    to_lower_euc_kr,
    to_upper_euc_kr,
    sort_order_euc_kr,
    0,          /* strxfrm_multiply */
    NULL,       /* strcoll    */
    NULL,       /* strxfrm    */
    NULL,       /* strnncoll  */
    NULL,       /* strnxfrm   */
    NULL,       /* like_range */
    2,      /* mbmaxlen */
    ismbchar_euc_kr,
    ismbhead_euc_kr,
    mbcharlen_euc_kr
  },

  /* this information is filled in by configure */
  {
     24,    /* number */
    "gb2312",     /* name */
    ctype_gb2312,
    to_lower_gb2312,
    to_upper_gb2312,
    sort_order_gb2312,
    0,          /* strxfrm_multiply */
    NULL,       /* strcoll    */
    NULL,       /* strxfrm    */
    NULL,       /* strnncoll  */
    NULL,       /* strnxfrm   */
    NULL,       /* like_range */
    2,      /* mbmaxlen */
    ismbchar_gb2312,
    ismbhead_gb2312,
    mbcharlen_gb2312
  },

  /* this information is filled in by configure */
  {
     28,    /* number */
    "gbk",     /* name */
    ctype_gbk,
    to_lower_gbk,
    to_upper_gbk,
    sort_order_gbk,
    1,      /* strxfrm_multiply */
    my_strcoll_gbk,
    my_strxfrm_gbk,
    my_strnncoll_gbk,
    my_strnxfrm_gbk,
    my_like_range_gbk,
    2,      /* mbmaxlen */
    ismbchar_gbk,
    ismbhead_gbk,
    mbcharlen_gbk
  },

  /* this information is filled in by configure */
  {
      5,    /* number */
    "german1",     /* name */
    ctype_german1,
    to_lower_german1,
    to_upper_german1,
    sort_order_german1,
    0,          /* strxfrm_multiply */
    NULL,       /* strcoll    */
    NULL,       /* strxfrm    */
    NULL,       /* strnncoll  */
    NULL,       /* strnxfrm   */
    NULL,       /* like_range */
    0,          /* mbmaxlen  */
    NULL,       /* ismbchar  */
    NULL,       /* ismbhead  */
    NULL        /* mbcharlen */
  },

  /* this information is filled in by configure */
  {
     25,    /* number */
    "greek",     /* name */
    ctype_greek,
    to_lower_greek,
    to_upper_greek,
    sort_order_greek,
    0,          /* strxfrm_multiply */
    NULL,       /* strcoll    */
    NULL,       /* strxfrm    */
    NULL,       /* strnncoll  */
    NULL,       /* strnxfrm   */
    NULL,       /* like_range */
    0,          /* mbmaxlen  */
    NULL,       /* ismbchar  */
    NULL,       /* ismbhead  */
    NULL        /* mbcharlen */
  },

  /* this information is filled in by configure */
  {
     16,    /* number */
    "hebrew",     /* name */
    ctype_hebrew,
    to_lower_hebrew,
    to_upper_hebrew,
    sort_order_hebrew,
    0,          /* strxfrm_multiply */
    NULL,       /* strcoll    */
    NULL,       /* strxfrm    */
    NULL,       /* strnncoll  */
    NULL,       /* strnxfrm   */
    NULL,       /* like_range */
    0,          /* mbmaxlen  */
    NULL,       /* ismbchar  */
    NULL,       /* ismbhead  */
    NULL        /* mbcharlen */
  },

  /* this information is filled in by configure */
  {
      6,    /* number */
    "hp8",     /* name */
    ctype_hp8,
    to_lower_hp8,
    to_upper_hp8,
    sort_order_hp8,
    0,          /* strxfrm_multiply */
    NULL,       /* strcoll    */
    NULL,       /* strxfrm    */
    NULL,       /* strnncoll  */
    NULL,       /* strnxfrm   */
    NULL,       /* like_range */
    0,          /* mbmaxlen  */
    NULL,       /* ismbchar  */
    NULL,       /* ismbhead  */
    NULL        /* mbcharlen */
  },

  /* this information is filled in by configure */
  {
     21,    /* number */
    "hungarian",     /* name */
    ctype_hungarian,
    to_lower_hungarian,
    to_upper_hungarian,
    sort_order_hungarian,
    0,          /* strxfrm_multiply */
    NULL,       /* strcoll    */
    NULL,       /* strxfrm    */
    NULL,       /* strnncoll  */
    NULL,       /* strnxfrm   */
    NULL,       /* like_range */
    0,          /* mbmaxlen  */
    NULL,       /* ismbchar  */
    NULL,       /* ismbhead  */
    NULL        /* mbcharlen */
  },

  /* this information is filled in by configure */
  {
      7,    /* number */
    "koi8_ru",     /* name */
    ctype_koi8_ru,
    to_lower_koi8_ru,
    to_upper_koi8_ru,
    sort_order_koi8_ru,
    0,          /* strxfrm_multiply */
    NULL,       /* strcoll    */
    NULL,       /* strxfrm    */
    NULL,       /* strnncoll  */
    NULL,       /* strnxfrm   */
    NULL,       /* like_range */
    0,          /* mbmaxlen  */
    NULL,       /* ismbchar  */
    NULL,       /* ismbhead  */
    NULL        /* mbcharlen */
  },

  /* this information is filled in by configure */
  {
     22,    /* number */
    "koi8_ukr",     /* name */
    ctype_koi8_ukr,
    to_lower_koi8_ukr,
    to_upper_koi8_ukr,
    sort_order_koi8_ukr,
    0,          /* strxfrm_multiply */
    NULL,       /* strcoll    */
    NULL,       /* strxfrm    */
    NULL,       /* strnncoll  */
    NULL,       /* strnxfrm   */
    NULL,       /* like_range */
    0,          /* mbmaxlen  */
    NULL,       /* ismbchar  */
    NULL,       /* ismbhead  */
    NULL        /* mbcharlen */
  },

  /* this information is filled in by configure */
  {
      9,    /* number */
    "latin2",     /* name */
    ctype_latin2,
    to_lower_latin2,
    to_upper_latin2,
    sort_order_latin2,
    0,          /* strxfrm_multiply */
    NULL,       /* strcoll    */
    NULL,       /* strxfrm    */
    NULL,       /* strnncoll  */
    NULL,       /* strnxfrm   */
    NULL,       /* like_range */
    0,          /* mbmaxlen  */
    NULL,       /* ismbchar  */
    NULL,       /* ismbhead  */
    NULL        /* mbcharlen */
  },

  /* this information is filled in by configure */
  {
     10,    /* number */
    "swe7",     /* name */
    ctype_swe7,
    to_lower_swe7,
    to_upper_swe7,
    sort_order_swe7,
    0,          /* strxfrm_multiply */
    NULL,       /* strcoll    */
    NULL,       /* strxfrm    */
    NULL,       /* strnncoll  */
    NULL,       /* strnxfrm   */
    NULL,       /* like_range */
    0,          /* mbmaxlen  */
    NULL,       /* ismbchar  */
    NULL,       /* ismbhead  */
    NULL        /* mbcharlen */
  },

  /* this information is filled in by configure */
  {
     11,    /* number */
    "usa7",     /* name */
    ctype_usa7,
    to_lower_usa7,
    to_upper_usa7,
    sort_order_usa7,
    0,          /* strxfrm_multiply */
    NULL,       /* strcoll    */
    NULL,       /* strxfrm    */
    NULL,       /* strnncoll  */
    NULL,       /* strnxfrm   */
    NULL,       /* like_range */
    0,          /* mbmaxlen  */
    NULL,       /* ismbchar  */
    NULL,       /* ismbhead  */
    NULL        /* mbcharlen */
  },

  /* this information is filled in by configure */
  {
     26,    /* number */
    "win1250",     /* name */
    ctype_win1250,
    to_lower_win1250,
    to_upper_win1250,
    sort_order_win1250,
    0,          /* strxfrm_multiply */
    NULL,       /* strcoll    */
    NULL,       /* strxfrm    */
    NULL,       /* strnncoll  */
    NULL,       /* strnxfrm   */
    NULL,       /* like_range */
    0,          /* mbmaxlen  */
    NULL,       /* ismbchar  */
    NULL,       /* ismbhead  */
    NULL        /* mbcharlen */
  },

  /* this information is filled in by configure */
  {
     17,    /* number */
    "win1251",     /* name */
    ctype_win1251,
    to_lower_win1251,
    to_upper_win1251,
    sort_order_win1251,
    0,          /* strxfrm_multiply */
    NULL,       /* strcoll    */
    NULL,       /* strxfrm    */
    NULL,       /* strnncoll  */
    NULL,       /* strnxfrm   */
    NULL,       /* like_range */
    0,          /* mbmaxlen  */
    NULL,       /* ismbchar  */
    NULL,       /* ismbhead  */
    NULL        /* mbcharlen */
  },

  /* this information is filled in by configure */
  {
     23,    /* number */
    "win1251ukr",     /* name */
    ctype_win1251ukr,
    to_lower_win1251ukr,
    to_upper_win1251ukr,
    sort_order_win1251ukr,
    0,          /* strxfrm_multiply */
    NULL,       /* strcoll    */
    NULL,       /* strxfrm    */
    NULL,       /* strnncoll  */
    NULL,       /* strnxfrm   */
    NULL,       /* like_range */
    0,          /* mbmaxlen  */
    NULL,       /* ismbchar  */
    NULL,       /* ismbhead  */
    NULL        /* mbcharlen */
  },

  /* this information is filled in by configure */
  {
     12,    /* number */
    "ujis",     /* name */
    ctype_ujis,
    to_lower_ujis,
    to_upper_ujis,
    sort_order_ujis,
    0,          /* strxfrm_multiply */
    NULL,       /* strcoll    */
    NULL,       /* strxfrm    */
    NULL,       /* strnncoll  */
    NULL,       /* strnxfrm   */
    NULL,       /* like_range */
    3,      /* mbmaxlen */
    ismbchar_ujis,
    ismbhead_ujis,
    mbcharlen_ujis
  },

  /* this information is filled in by configure */
  {
     13,    /* number */
    "sjis",     /* name */
    ctype_sjis,
    to_lower_sjis,
    to_upper_sjis,
    sort_order_sjis,
    1,      /* strxfrm_multiply */
    my_strcoll_sjis,
    my_strxfrm_sjis,
    my_strnncoll_sjis,
    my_strnxfrm_sjis,
    my_like_range_sjis,
    2,      /* mbmaxlen */
    ismbchar_sjis,
    ismbhead_sjis,
    mbcharlen_sjis
  },

  /* this information is filled in by configure */
  {
     18,    /* number */
    "tis620",     /* name */
    ctype_tis620,
    to_lower_tis620,
    to_upper_tis620,
    sort_order_tis620,
    4,      /* strxfrm_multiply */
    my_strcoll_tis620,
    my_strxfrm_tis620,
    my_strnncoll_tis620,
    my_strnxfrm_tis620,
    my_like_range_tis620,
    0,          /* mbmaxlen  */
    NULL,       /* ismbchar  */
    NULL,       /* ismbhead  */
    NULL        /* mbcharlen */
  },

  /* this information is filled in by configure */
  {
    0,          /* end-of-list marker */
    NullS,
    NULL,
    NULL,
    NULL,
    NULL,
    0,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0,
    NULL,
    NULL,
    NULL
  }
};
