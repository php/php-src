/*
** Compile and run this standalone program in order to generate code that
** implements a function that will translate alphabetic identifiers into
** parser token codes.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
** All the keywords of the SQL language are stored as in a hash
** table composed of instances of the following structure.
*/
typedef struct Keyword Keyword;
struct Keyword {
  char *zName;         /* The keyword name */
  char *zTokenType;    /* Token value for this keyword */
  int hash;            /* Hash on the keyword */
  int offset;          /* Offset to start of name string */
  int len;             /* Length of this keyword, not counting final \000 */
  int iNext;           /* Index in aKeywordTable[] of next with same hash */
};

/*
** These are the keywords
*/
static Keyword aKeywordTable[] = {
  { "ABORT",            "TK_ABORT",        },
  { "AFTER",            "TK_AFTER",        },
  { "ALL",              "TK_ALL",          },
  { "AND",              "TK_AND",          },
  { "AS",               "TK_AS",           },
  { "ASC",              "TK_ASC",          },
  { "ATTACH",           "TK_ATTACH",       },
  { "BEFORE",           "TK_BEFORE",       },
  { "BEGIN",            "TK_BEGIN",        },
  { "BETWEEN",          "TK_BETWEEN",      },
  { "BY",               "TK_BY",           },
  { "CASCADE",          "TK_CASCADE",      },
  { "CASE",             "TK_CASE",         },
  { "CHECK",            "TK_CHECK",        },
  { "COLLATE",          "TK_COLLATE",      },
  { "COMMIT",           "TK_COMMIT",       },
  { "CONFLICT",         "TK_CONFLICT",     },
  { "CONSTRAINT",       "TK_CONSTRAINT",   },
  { "CREATE",           "TK_CREATE",       },
  { "CROSS",            "TK_JOIN_KW",      },
  { "DATABASE",         "TK_DATABASE",     },
  { "DEFAULT",          "TK_DEFAULT",      },
  { "DEFERRED",         "TK_DEFERRED",     },
  { "DEFERRABLE",       "TK_DEFERRABLE",   },
  { "DELETE",           "TK_DELETE",       },
  { "DESC",             "TK_DESC",         },
  { "DETACH",           "TK_DETACH",       },
  { "DISTINCT",         "TK_DISTINCT",     },
  { "DROP",             "TK_DROP",         },
  { "END",              "TK_END",          },
  { "EACH",             "TK_EACH",         },
  { "ELSE",             "TK_ELSE",         },
  { "EXCEPT",           "TK_EXCEPT",       },
  { "EXCLUSIVE",        "TK_EXCLUSIVE",    },
  { "EXPLAIN",          "TK_EXPLAIN",      },
  { "FAIL",             "TK_FAIL",         },
  { "FOR",              "TK_FOR",          },
  { "FOREIGN",          "TK_FOREIGN",      },
  { "FROM",             "TK_FROM",         },
  { "FULL",             "TK_JOIN_KW",      },
  { "GLOB",             "TK_GLOB",         },
  { "GROUP",            "TK_GROUP",        },
  { "HAVING",           "TK_HAVING",       },
  { "IGNORE",           "TK_IGNORE",       },
  { "IMMEDIATE",        "TK_IMMEDIATE",    },
  { "IN",               "TK_IN",           },
  { "INDEX",            "TK_INDEX",        },
  { "INITIALLY",        "TK_INITIALLY",    },
  { "INNER",            "TK_JOIN_KW",      },
  { "INSERT",           "TK_INSERT",       },
  { "INSTEAD",          "TK_INSTEAD",      },
  { "INTERSECT",        "TK_INTERSECT",    },
  { "INTO",             "TK_INTO",         },
  { "IS",               "TK_IS",           },
  { "ISNULL",           "TK_ISNULL",       },
  { "JOIN",             "TK_JOIN",         },
  { "KEY",              "TK_KEY",          },
  { "LEFT",             "TK_JOIN_KW",      },
  { "LIKE",             "TK_LIKE",         },
  { "LIMIT",            "TK_LIMIT",        },
  { "MATCH",            "TK_MATCH",        },
  { "NATURAL",          "TK_JOIN_KW",      },
  { "NOT",              "TK_NOT",          },
  { "NOTNULL",          "TK_NOTNULL",      },
  { "NULL",             "TK_NULL",         },
  { "OF",               "TK_OF",           },
  { "OFFSET",           "TK_OFFSET",       },
  { "ON",               "TK_ON",           },
  { "OR",               "TK_OR",           },
  { "ORDER",            "TK_ORDER",        },
  { "OUTER",            "TK_JOIN_KW",      },
  { "PRAGMA",           "TK_PRAGMA",       },
  { "PRIMARY",          "TK_PRIMARY",      },
  { "RAISE",            "TK_RAISE",        },
  { "REFERENCES",       "TK_REFERENCES",   },
  { "REPLACE",          "TK_REPLACE",      },
  { "RESTRICT",         "TK_RESTRICT",     },
  { "RIGHT",            "TK_JOIN_KW",      },
  { "ROLLBACK",         "TK_ROLLBACK",     },
  { "ROW",              "TK_ROW",          },
  { "SELECT",           "TK_SELECT",       },
  { "SET",              "TK_SET",          },
  { "STATEMENT",        "TK_STATEMENT",    },
  { "TABLE",            "TK_TABLE",        },
  { "TEMP",             "TK_TEMP",         },
  { "TEMPORARY",        "TK_TEMP",         },
  { "THEN",             "TK_THEN",         },
  { "TRANSACTION",      "TK_TRANSACTION",  },
  { "TRIGGER",          "TK_TRIGGER",      },
  { "UNION",            "TK_UNION",        },
  { "UNIQUE",           "TK_UNIQUE",       },
  { "UPDATE",           "TK_UPDATE",       },
  { "USING",            "TK_USING",        },
  { "VACUUM",           "TK_VACUUM",       },
  { "VALUES",           "TK_VALUES",       },
  { "VIEW",             "TK_VIEW",         },
  { "WHEN",             "TK_WHEN",         },
  { "WHERE",            "TK_WHERE",        },
};

/* Number of keywords */
#define NKEYWORD (sizeof(aKeywordTable)/sizeof(aKeywordTable[0]))

/* An array to map all upper-case characters into their corresponding
** lower-case character. 
*/
const unsigned char sqlite3UpperToLower[] = {
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17,
     18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
     36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
     54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 97, 98, 99,100,101,102,103,
    104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,
    122, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,104,105,106,107,
    108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,
    126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
    144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,
    162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,
    180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,
    198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,
    216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,
    234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,
    252,253,254,255
};
#define UpperToLower sqlite3UpperToLower

/*
** Comparision function for two Keyword records
*/
static int keywordCompare(const void *a, const void *b){
  const Keyword *pA = (Keyword*)a;
  const Keyword *pB = (Keyword*)b;
  return strcmp(pA->zName, pB->zName);
}

/*
** This routine does the work.  The generated code is printed on standard
** output.
*/
int main(int argc, char **argv){
  int i, j, h;
  int bestSize, bestCount;
  int count;
  int nChar;
  int aHash[1000];  /* 1000 is much bigger than NKEYWORD */

  /* Make sure the table is sorted */
  qsort(aKeywordTable, NKEYWORD, sizeof(aKeywordTable[0]), keywordCompare);

  /* Fill in the hash value, length, and offset for all entries */
  nChar = 0;
  for(i=0; i<NKEYWORD; i++){
    Keyword *p = &aKeywordTable[i];
    p->len = strlen(p->zName);
    /* p->hash = sqlite3HashNoCase(p->zName, p->len); */
    p->hash = UpperToLower[p->zName[0]]*5 +
              UpperToLower[p->zName[p->len-1]]*3 + p->len;
    p->offset = nChar;
    if( i<NKEYWORD-1 && strncmp(p->zName, aKeywordTable[i+1].zName,p->len)==0 ){
      /* This entry is a prefix of the one that follows.  Do not advance
      ** the offset */
    }else{
      nChar += p->len;
    }
  }

  /* Figure out how big to make the hash table in order to minimize the
  ** number of collisions */
  bestSize = NKEYWORD;
  bestCount = NKEYWORD*NKEYWORD;
  for(i=NKEYWORD/2; i<=2*NKEYWORD; i++){
    for(j=0; j<i; j++) aHash[j] = 0;
    for(j=0; j<NKEYWORD; j++){
      h = aKeywordTable[j].hash % i;
      aHash[h] *= 2;
      aHash[h]++;
    }
    for(j=count=0; j<i; j++) count += aHash[j];
    if( count<bestCount ){
      bestCount = count;
      bestSize = i;
    }
  }

  /* Compute the hash */
  for(i=0; i<bestSize; i++) aHash[i] = 0;
  for(i=0; i<NKEYWORD; i++){
    h = aKeywordTable[i].hash % bestSize;
    aKeywordTable[i].iNext = aHash[h];
    aHash[h] = i+1;
  }

  /* Begin generating code */
  printf("int sqlite3KeywordCode(const char *z, int n){\n");

  printf("  static const char zText[%d] =\n", nChar+1);
  for(i=j=0; i<NKEYWORD; i++){
    Keyword *p = &aKeywordTable[i];
    if( i<NKEYWORD-1 && p->offset==aKeywordTable[i+1].offset ) continue;
    if( j==0 ) printf("    \"");
    printf("%s", p->zName);
    j += p->len;
    if( j>60 ){
      printf("\"\n");
      j = 0;
    }
  }
  printf("%s;\n", j>0 ? "\"" : "  ");

  printf("  static const unsigned char aHash[%d] = {\n", bestSize);
  for(i=j=0; i<bestSize; i++){
    if( j==0 ) printf("    ");
    printf(" %3d,", aHash[i]);
    j++;
    if( j>12 ){
      printf("\n");
      j = 0;
    }
  }
  printf("%s  };\n", j==0 ? "" : "\n");    

  printf("  static const unsigned char aNext[%d] = {\n", NKEYWORD);
  for(i=j=0; i<NKEYWORD; i++){
    if( j==0 ) printf("    ");
    printf(" %3d,", aKeywordTable[i].iNext);
    j++;
    if( j>12 ){
      printf("\n");
      j = 0;
    }
  }
  printf("%s  };\n", j==0 ? "" : "\n");    

  printf("  static const unsigned char aLen[%d] = {\n", NKEYWORD);
  for(i=j=0; i<NKEYWORD; i++){
    if( j==0 ) printf("    ");
    printf(" %3d,", aKeywordTable[i].len);
    j++;
    if( j>12 ){
      printf("\n");
      j = 0;
    }
  }
  printf("%s  };\n", j==0 ? "" : "\n");    

  printf("  static const unsigned short int aOffset[%d] = {\n", NKEYWORD);
  for(i=j=0; i<NKEYWORD; i++){
    if( j==0 ) printf("    ");
    printf(" %3d,", aKeywordTable[i].offset);
    j++;
    if( j>12 ){
      printf("\n");
      j = 0;
    }
  }
  printf("%s  };\n", j==0 ? "" : "\n");

  printf("  static const unsigned char aCode[%d] = {\n", NKEYWORD);
  for(i=j=0; i<NKEYWORD; i++){
    char *zToken = aKeywordTable[i].zTokenType;
    if( j==0 ) printf("    ");
    printf("%s,%*s", zToken, (int)(14-strlen(zToken)), "");
    j++;
    if( j>=5 ){
      printf("\n");
      j = 0;
    }
  }
  printf("%s  };\n", j==0 ? "" : "\n");

  printf("  int h, i;\n");
  printf("  if( n<2 ) return TK_ID;\n");
  printf("  h = (sqlite3UpperToLower[((unsigned char*)z)[0]]*5 + \n"
         "      sqlite3UpperToLower[((unsigned char*)z)[n-1]]*3 +\n"
         "      n) %% %d;\n", bestSize);
  printf("  for(i=((int)aHash[h])-1; i>=0; i=((int)aNext[i])-1){\n");
  printf("    if( aLen[i]==n &&"
                   " sqlite3StrNICmp(&zText[aOffset[i]],z,n)==0 ){\n");
  printf("      return aCode[i];\n");
  printf("    }\n");
  printf("  }\n");
  printf("  return TK_ID;\n");
  printf("}\n");

  return 0;
}
