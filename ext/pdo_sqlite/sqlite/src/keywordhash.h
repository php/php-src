/* Hash score: 148 */
static int keywordCode(const char *z, int n){
  static const char zText[504] =
    "ABORTABLEFTEMPORARYAFTERAISELECTHENDATABASEACHECKEYALTEREFERENCES"
    "CAPELSEXCEPTRANSACTIONATURALIKEXCLUSIVEXISTSTATEMENTRIGGEREINDEX"
    "PLAINITIALLYANDEFAULTATTACHAVINGLOBEFOREIGNORENAMEAUTOINCREMENT"
    "BEGINNEREPLACEBETWEENOTNULLIMITBYCASCADEFERRABLECASECOLLATECOMMIT"
    "CONFLICTCONSTRAINTERSECTCREATECROSSCURRENT_DATECURRENT_TIMESTAMP"
    "RAGMATCHDEFERREDELETEDESCDETACHDISTINCTDROPRIMARYFAILFROMFULL"
    "GROUPDATEIMMEDIATEINSERTINSTEADINTOFFSETISNULLJOINORDERESTRICT"
    "OUTERIGHTROLLBACKROWHENUNIONUNIQUEUSINGVACUUMVALUESVIEWHERE";
  static const unsigned char aHash[127] = {
      87,  78,  99,  86,   0,   4,   0,   0, 106,   0,  72,   0,   0,
      90,  43,   0,  88,   0,  98, 101,  92,   0,   0,   9,   0,   0,
     105,   0, 102,  96,   0,  10,  46,   0,  40,   0,   0,  61,  66,
       0,  60,  14,   0,   0,  35,  80,   0, 100,  69,   0,   0,  26,
       0,  73,  59,   0,  12,   0, 107,  37,  11,   0,  75,  39,  20,
       0,   0,   0,  34,  79,  51,  33,  48,  15,  84,   0,  36,   0,
      70,  21,   0,  67,   0,   0,   0,   0,  45,  62,  17,  83,  32,
      64,  82,   0,   1,   0,  13,  50,  56,   8,   0, 104,  71,  94,
      52,   6,  55,   0,   0,  47,  89,   0,  97,   0,  65,   0,   0,
      23,   0, 108,  49,  54,   0,   2,  53,   0, 103,
  };
  static const unsigned char aNext[108] = {
       0,   0,   0,   0,   0,   3,   0,   0,   0,   0,   0,   0,   0,
       0,   0,   0,   7,   0,   0,   0,   0,   0,   0,   0,  18,   5,
       0,   0,   0,   0,   0,   0,   0,   0,   0,  27,   0,   0,   0,
       0,   0,   0,   0,   0,  42,   0,   0,   0,   0,   0,   0,   0,
      24,   0,   0,  44,   0,   0,   0,  30,  57,   0,   0,   0,   0,
       0,   0,   0,  68,  41,   0,   0,   0,   0,  19,  58,  16,   0,
      77,   0,  63,   0,  81,  31,   0,   0,   0,   0,   0,   0,   0,
      38,  91,  93,   0,   0,  95,  22,  29,  76,   0,  25,  85,   0,
      28,   0,  74,   0,
  };
  static const unsigned char aLen[108] = {
       5,   5,   4,   4,   9,   2,   5,   5,   6,   4,   3,   8,   2,
       4,   5,   3,   5,  10,   6,   4,   6,  11,   2,   7,   4,   9,
       6,   9,   7,   7,   5,   7,   9,   3,   3,   7,   6,   6,   4,
       6,   3,   7,   6,   6,  13,   2,   2,   5,   5,   7,   7,   3,
       7,   4,   5,   2,   7,   3,  10,   4,   7,   6,   8,  10,   9,
       6,   5,  12,  12,  17,   6,   5,   8,   6,   4,   6,   8,   2,
       4,   7,   4,   4,   4,   5,   6,   9,   6,   7,   4,   2,   6,
       3,   6,   4,   5,   8,   5,   5,   8,   3,   4,   5,   6,   5,
       6,   6,   4,   5,
  };
  static const unsigned short int aOffset[108] = {
       0,   4,   7,  10,  10,  14,  19,  23,  26,  31,  33,  35,  40,
      42,  44,  48,  51,  55,  63,  68,  71,  76,  85,  86,  92,  95,
     103, 108, 116, 122, 124, 127, 132, 137, 141, 143, 150, 155, 160,
     163, 165, 165, 169, 173, 179, 181, 183, 192, 195, 199, 206, 212,
     212, 215, 218, 223, 225, 226, 230, 240, 244, 251, 257, 265, 272,
     281, 287, 292, 304, 304, 320, 324, 329, 336, 342, 346, 352, 353,
     360, 363, 370, 374, 378, 382, 385, 391, 400, 406, 413, 416, 416,
     419, 422, 428, 432, 436, 444, 448, 453, 461, 463, 467, 472, 478,
     483, 489, 495, 498,
  };
  static const unsigned char aCode[108] = {
    TK_ABORT,      TK_TABLE,      TK_JOIN_KW,    TK_TEMP,       TK_TEMP,       
    TK_OR,         TK_AFTER,      TK_RAISE,      TK_SELECT,     TK_THEN,       
    TK_END,        TK_DATABASE,   TK_AS,         TK_EACH,       TK_CHECK,      
    TK_KEY,        TK_ALTER,      TK_REFERENCES, TK_ESCAPE,     TK_ELSE,       
    TK_EXCEPT,     TK_TRANSACTION,TK_ON,         TK_JOIN_KW,    TK_LIKE,       
    TK_EXCLUSIVE,  TK_EXISTS,     TK_STATEMENT,  TK_TRIGGER,    TK_REINDEX,    
    TK_INDEX,      TK_EXPLAIN,    TK_INITIALLY,  TK_ALL,        TK_AND,        
    TK_DEFAULT,    TK_ATTACH,     TK_HAVING,     TK_GLOB,       TK_BEFORE,     
    TK_FOR,        TK_FOREIGN,    TK_IGNORE,     TK_RENAME,     TK_AUTOINCR,   
    TK_TO,         TK_IN,         TK_BEGIN,      TK_JOIN_KW,    TK_REPLACE,    
    TK_BETWEEN,    TK_NOT,        TK_NOTNULL,    TK_NULL,       TK_LIMIT,      
    TK_BY,         TK_CASCADE,    TK_ASC,        TK_DEFERRABLE, TK_CASE,       
    TK_COLLATE,    TK_COMMIT,     TK_CONFLICT,   TK_CONSTRAINT, TK_INTERSECT,  
    TK_CREATE,     TK_JOIN_KW,    TK_CDATE,      TK_CTIME,      TK_CTIMESTAMP, 
    TK_PRAGMA,     TK_MATCH,      TK_DEFERRED,   TK_DELETE,     TK_DESC,       
    TK_DETACH,     TK_DISTINCT,   TK_IS,         TK_DROP,       TK_PRIMARY,    
    TK_FAIL,       TK_FROM,       TK_JOIN_KW,    TK_GROUP,      TK_UPDATE,     
    TK_IMMEDIATE,  TK_INSERT,     TK_INSTEAD,    TK_INTO,       TK_OF,         
    TK_OFFSET,     TK_SET,        TK_ISNULL,     TK_JOIN,       TK_ORDER,      
    TK_RESTRICT,   TK_JOIN_KW,    TK_JOIN_KW,    TK_ROLLBACK,   TK_ROW,        
    TK_WHEN,       TK_UNION,      TK_UNIQUE,     TK_USING,      TK_VACUUM,     
    TK_VALUES,     TK_VIEW,       TK_WHERE,      
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
