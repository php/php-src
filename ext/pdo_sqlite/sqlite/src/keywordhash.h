/* Hash score: 153 */
static int keywordCode(const char *z, int n){
  static const char zText[515] =
    "ABORTABLEFTEMPORARYADDATABASELECTHENDEFAULTRANSACTIONATURALTER"
    "AISEACHECKEYAFTEREFERENCESCAPELSEXCEPTRIGGEREGEXPLAINITIALLYAND"
    "EFERRABLEXCLUSIVEXISTSTATEMENTATTACHAVINGLOBEFOREIGNOREINDEXAUTOINCREMENT"
    "BEGINNERENAMEBETWEENOTNULLIKEBYCASCADEFERREDELETECASECOLLATECOLUMN"
    "COMMITCONFLICTCONSTRAINTERSECTCREATECROSSCURRENT_DATECURRENT_TIMESTAMP"
    "RAGMATCHDESCDETACHDISTINCTDROPRIMARYFAILIMITFROMFULLGROUPDATE"
    "IMMEDIATEINSERTINSTEADINTOFFSETISNULLJOINORDEREPLACEOUTERESTRICT"
    "RIGHTROLLBACKROWHENUNIONUNIQUEUSINGVACUUMVALUESVIEWHERE";
  static const unsigned char aHash[127] = {
      89,  79, 102,  88,   0,   4,   0,   0, 109,   0,  75,   0,   0,
      92,  43,   0,  90,   0, 101, 104,  94,   0,   0,  10,   0,   0,
     108,   0, 105, 100,   0,  28,  47,   0,  40,   0,   0,  63,  69,
       0,  62,  19,   0,   0,  32,  81,   0, 103,  72,   0,   0,  34,
       0,  60,  33,   0,   8,   0, 110,  37,  12,   0,  76,  39,  25,
      64,   0,   0,  31,  80,  52,  30,  49,  20,  86,   0,  35,   0,
      73,  26,   0,  70,   0,   0,   0,   0,  46,  65,  22,  85,  29,
      67,  84,   0,   1,   0,   9,  98,  57,  18,   0, 107,  74,  96,
      53,   6,  83,   0,   0,  48,  91,   0,  99,   0,  68,   0,   0,
      15,   0, 111,  50,  55,   0,   2,  54,   0, 106,
  };
  static const unsigned char aNext[111] = {
       0,   0,   0,   0,   0,   3,   0,   0,   0,   0,   0,   0,   0,
       0,   0,   0,   0,   0,   0,   0,   0,  17,   0,   0,   0,   0,
       0,  11,   0,   0,   0,   7,   0,   5,  13,   0,   0,   0,   0,
       0,   0,   0,   0,   0,   0,  42,   0,   0,   0,   0,   0,   0,
       0,  16,   0,  23,  51,   0,   0,   0,   0,  44,  58,   0,   0,
       0,   0,   0,   0,   0,   0,  71,  41,   0,   0,  24,  59,  21,
       0,  78,   0,  66,   0,   0,  82,  45,   0,   0,   0,   0,   0,
       0,   0,  38,  93,  95,   0,   0,  97,   0,  14,  27,  77,   0,
      56,  87,   0,  36,   0,  61,   0,
  };
  static const unsigned char aLen[111] = {
       5,   5,   4,   4,   9,   2,   3,   8,   2,   6,   4,   3,   7,
      11,   2,   7,   5,   5,   4,   5,   3,   5,  10,   6,   4,   6,
       7,   6,   7,   9,   3,   3,  10,   9,   6,   9,   6,   6,   4,
       6,   3,   7,   6,   7,   5,  13,   2,   2,   5,   5,   6,   7,
       3,   7,   4,   4,   2,   7,   3,   8,   6,   4,   7,   6,   6,
       8,  10,   9,   6,   5,  12,  12,  17,   6,   5,   4,   6,   8,
       2,   4,   7,   4,   5,   4,   4,   5,   6,   9,   6,   7,   4,
       2,   6,   3,   6,   4,   5,   7,   5,   8,   5,   8,   3,   4,
       5,   6,   5,   6,   6,   4,   5,
  };
  static const unsigned short int aOffset[111] = {
       0,   4,   7,  10,  10,  14,  19,  21,  26,  27,  32,  34,  36,
      42,  51,  52,  57,  61,  65,  67,  71,  74,  78,  86,  91,  94,
      99, 105, 108, 113, 118, 122, 124, 133, 141, 146, 155, 160, 165,
     168, 170, 170, 174, 178, 180, 185, 187, 189, 198, 201, 205, 211,
     217, 217, 220, 223, 227, 229, 230, 234, 241, 247, 251, 258, 264,
     270, 278, 285, 294, 300, 305, 317, 317, 333, 337, 342, 346, 352,
     353, 360, 363, 370, 373, 378, 382, 386, 389, 395, 404, 410, 417,
     420, 420, 423, 426, 432, 436, 440, 447, 451, 459, 464, 472, 474,
     478, 483, 489, 494, 500, 506, 509,
  };
  static const unsigned char aCode[111] = {
    TK_ABORT,      TK_TABLE,      TK_JOIN_KW,    TK_TEMP,       TK_TEMP,       
    TK_OR,         TK_ADD,        TK_DATABASE,   TK_AS,         TK_SELECT,     
    TK_THEN,       TK_END,        TK_DEFAULT,    TK_TRANSACTION,TK_ON,         
    TK_JOIN_KW,    TK_ALTER,      TK_RAISE,      TK_EACH,       TK_CHECK,      
    TK_KEY,        TK_AFTER,      TK_REFERENCES, TK_ESCAPE,     TK_ELSE,       
    TK_EXCEPT,     TK_TRIGGER,    TK_LIKE_KW,    TK_EXPLAIN,    TK_INITIALLY,  
    TK_ALL,        TK_AND,        TK_DEFERRABLE, TK_EXCLUSIVE,  TK_EXISTS,     
    TK_STATEMENT,  TK_ATTACH,     TK_HAVING,     TK_LIKE_KW,    TK_BEFORE,     
    TK_FOR,        TK_FOREIGN,    TK_IGNORE,     TK_REINDEX,    TK_INDEX,      
    TK_AUTOINCR,   TK_TO,         TK_IN,         TK_BEGIN,      TK_JOIN_KW,    
    TK_RENAME,     TK_BETWEEN,    TK_NOT,        TK_NOTNULL,    TK_NULL,       
    TK_LIKE_KW,    TK_BY,         TK_CASCADE,    TK_ASC,        TK_DEFERRED,   
    TK_DELETE,     TK_CASE,       TK_COLLATE,    TK_COLUMNKW,   TK_COMMIT,     
    TK_CONFLICT,   TK_CONSTRAINT, TK_INTERSECT,  TK_CREATE,     TK_JOIN_KW,    
    TK_CTIME_KW,   TK_CTIME_KW,   TK_CTIME_KW,   TK_PRAGMA,     TK_MATCH,      
    TK_DESC,       TK_DETACH,     TK_DISTINCT,   TK_IS,         TK_DROP,       
    TK_PRIMARY,    TK_FAIL,       TK_LIMIT,      TK_FROM,       TK_JOIN_KW,    
    TK_GROUP,      TK_UPDATE,     TK_IMMEDIATE,  TK_INSERT,     TK_INSTEAD,    
    TK_INTO,       TK_OF,         TK_OFFSET,     TK_SET,        TK_ISNULL,     
    TK_JOIN,       TK_ORDER,      TK_REPLACE,    TK_JOIN_KW,    TK_RESTRICT,   
    TK_JOIN_KW,    TK_ROLLBACK,   TK_ROW,        TK_WHEN,       TK_UNION,      
    TK_UNIQUE,     TK_USING,      TK_VACUUM,     TK_VALUES,     TK_VIEW,       
    TK_WHERE,      
  };
  int h, i;
  if( n<2 ) return TK_ID;
  h = ((sqlite3UpperToLower[((unsigned char*)z)[0]]*4) ^
      (sqlite3UpperToLower[((unsigned char*)z)[n-1]]*3) ^
      n) % 127;
  for(i=((int)aHash[h])-1; i>=0; i=((int)aNext[i])-1){
    if( aLen[i]==n && sqlite3StrNICmp(&zText[aOffset[i]],z,n)==0 ){
      return aCode[i];
    }
  }
  return TK_ID;
}
int sqlite3KeywordCode(const char *z, int n){
  return keywordCode(z, n);
}
