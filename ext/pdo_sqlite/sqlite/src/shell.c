/*
** 2001 September 15
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This file contains code to implement the "sqlite" command line
** utility for accessing SQLite databases.
**
** $Id$
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "sqlite3.h"
#include <ctype.h>
#include <stdarg.h>

#if !defined(_WIN32) && !defined(WIN32) && !defined(__MACOS__) && !defined(__OS2__)
# include <signal.h>
# include <pwd.h>
# include <unistd.h>
# include <sys/types.h>
#endif

#ifdef __MACOS__
# include <console.h>
# include <signal.h>
# include <unistd.h>
# include <extras.h>
# include <Files.h>
# include <Folders.h>
#endif

#ifdef __OS2__
# include <unistd.h>
#endif

#if defined(HAVE_READLINE) && HAVE_READLINE==1
# include <readline/readline.h>
# include <readline/history.h>
#else
# define readline(p) local_getline(p,stdin)
# define add_history(X)
# define read_history(X)
# define write_history(X)
# define stifle_history(X)
#endif

#if defined(_WIN32) || defined(WIN32)
# include <io.h>
#else
/* Make sure isatty() has a prototype.
*/
extern int isatty();
#endif

/*
** If the following flag is set, then command execution stops
** at an error if we are not interactive.
*/
static int bail_on_error = 0;

/*
** Threat stdin as an interactive input if the following variable
** is true.  Otherwise, assume stdin is connected to a file or pipe.
*/
static int stdin_is_interactive = 1;

/*
** The following is the open SQLite database.  We make a pointer
** to this database a static variable so that it can be accessed
** by the SIGINT handler to interrupt database processing.
*/
static sqlite3 *db = 0;

/*
** True if an interrupt (Control-C) has been received.
*/
static volatile int seenInterrupt = 0;

/*
** This is the name of our program. It is set in main(), used
** in a number of other places, mostly for error messages.
*/
static char *Argv0;

/*
** Prompt strings. Initialized in main. Settable with
**   .prompt main continue
*/
static char mainPrompt[20];     /* First line prompt. default: "sqlite> "*/
static char continuePrompt[20]; /* Continuation prompt. default: "   ...> " */

/*
** Write I/O traces to the following stream.
*/
static FILE *iotrace = 0;

/*
** This routine works like printf in that its first argument is a
** format string and subsequent arguments are values to be substituted
** in place of % fields.  The result of formatting this string
** is written to iotrace.
*/
static void iotracePrintf(const char *zFormat, ...){
  va_list ap;
  char *z;
  if( iotrace==0 ) return;
  va_start(ap, zFormat);
  z = sqlite3_vmprintf(zFormat, ap);
  va_end(ap);
  fprintf(iotrace, "%s", z);
  sqlite3_free(z);
}


/*
** Determines if a string is a number of not.
*/
static int isNumber(const char *z, int *realnum){
  if( *z=='-' || *z=='+' ) z++;
  if( !isdigit(*z) ){
    return 0;
  }
  z++;
  if( realnum ) *realnum = 0;
  while( isdigit(*z) ){ z++; }
  if( *z=='.' ){
    z++;
    if( !isdigit(*z) ) return 0;
    while( isdigit(*z) ){ z++; }
    if( realnum ) *realnum = 1;
  }
  if( *z=='e' || *z=='E' ){
    z++;
    if( *z=='+' || *z=='-' ) z++;
    if( !isdigit(*z) ) return 0;
    while( isdigit(*z) ){ z++; }
    if( realnum ) *realnum = 1;
  }
  return *z==0;
}

/*
** A global char* and an SQL function to access its current value 
** from within an SQL statement. This program used to use the 
** sqlite_exec_printf() API to substitue a string into an SQL statement.
** The correct way to do this with sqlite3 is to use the bind API, but
** since the shell is built around the callback paradigm it would be a lot
** of work. Instead just use this hack, which is quite harmless.
*/
static const char *zShellStatic = 0;
static void shellstaticFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  assert( 0==argc );
  assert( zShellStatic );
  sqlite3_result_text(context, zShellStatic, -1, SQLITE_STATIC);
}


/*
** This routine reads a line of text from FILE in, stores
** the text in memory obtained from malloc() and returns a pointer
** to the text.  NULL is returned at end of file, or if malloc()
** fails.
**
** The interface is like "readline" but no command-line editing
** is done.
*/
static char *local_getline(char *zPrompt, FILE *in){
  char *zLine;
  int nLine;
  int n;
  int eol;

  if( zPrompt && *zPrompt ){
    printf("%s",zPrompt);
    fflush(stdout);
  }
  nLine = 100;
  zLine = malloc( nLine );
  if( zLine==0 ) return 0;
  n = 0;
  eol = 0;
  while( !eol ){
    if( n+100>nLine ){
      nLine = nLine*2 + 100;
      zLine = realloc(zLine, nLine);
      if( zLine==0 ) return 0;
    }
    if( fgets(&zLine[n], nLine - n, in)==0 ){
      if( n==0 ){
        free(zLine);
        return 0;
      }
      zLine[n] = 0;
      eol = 1;
      break;
    }
    while( zLine[n] ){ n++; }
    if( n>0 && zLine[n-1]=='\n' ){
      n--;
      zLine[n] = 0;
      eol = 1;
    }
  }
  zLine = realloc( zLine, n+1 );
  return zLine;
}

/*
** Retrieve a single line of input text.
**
** zPrior is a string of prior text retrieved.  If not the empty
** string, then issue a continuation prompt.
*/
static char *one_input_line(const char *zPrior, FILE *in){
  char *zPrompt;
  char *zResult;
  if( in!=0 ){
    return local_getline(0, in);
  }
  if( zPrior && zPrior[0] ){
    zPrompt = continuePrompt;
  }else{
    zPrompt = mainPrompt;
  }
  zResult = readline(zPrompt);
#if defined(HAVE_READLINE) && HAVE_READLINE==1
  if( zResult && *zResult ) add_history(zResult);
#endif
  return zResult;
}

struct previous_mode_data {
  int valid;        /* Is there legit data in here? */
  int mode;
  int showHeader;
  int colWidth[100];
};

/*
** An pointer to an instance of this structure is passed from
** the main program to the callback.  This is used to communicate
** state and mode information.
*/
struct callback_data {
  sqlite3 *db;            /* The database */
  int echoOn;            /* True to echo input commands */
  int cnt;               /* Number of records displayed so far */
  FILE *out;             /* Write results here */
  int mode;              /* An output mode setting */
  int writableSchema;    /* True if PRAGMA writable_schema=ON */
  int showHeader;        /* True to show column names in List or Column mode */
  char *zDestTable;      /* Name of destination table when MODE_Insert */
  char separator[20];    /* Separator character for MODE_List */
  int colWidth[100];     /* Requested width of each column when in column mode*/
  int actualWidth[100];  /* Actual width of each column */
  char nullvalue[20];    /* The text to print when a NULL comes back from
                         ** the database */
  struct previous_mode_data explainPrev;
                         /* Holds the mode information just before
                         ** .explain ON */
  char outfile[FILENAME_MAX]; /* Filename for *out */
  const char *zDbFilename;    /* name of the database file */
};

/*
** These are the allowed modes.
*/
#define MODE_Line     0  /* One column per line.  Blank line between records */
#define MODE_Column   1  /* One record per line in neat columns */
#define MODE_List     2  /* One record per line with a separator */
#define MODE_Semi     3  /* Same as MODE_List but append ";" to each line */
#define MODE_Html     4  /* Generate an XHTML table */
#define MODE_Insert   5  /* Generate SQL "insert" statements */
#define MODE_Tcl      6  /* Generate ANSI-C or TCL quoted elements */
#define MODE_Csv      7  /* Quote strings, numbers are plain */
#define MODE_NUM_OF   8  /* The number of modes (not a mode itself) */

static const char *modeDescr[MODE_NUM_OF] = {
  "line",
  "column",
  "list",
  "semi",
  "html",
  "insert",
  "tcl",
  "csv",
};

/*
** Number of elements in an array
*/
#define ArraySize(X)  (sizeof(X)/sizeof(X[0]))

/*
** Output the given string as a quoted string using SQL quoting conventions.
*/
static void output_quoted_string(FILE *out, const char *z){
  int i;
  int nSingle = 0;
  for(i=0; z[i]; i++){
    if( z[i]=='\'' ) nSingle++;
  }
  if( nSingle==0 ){
    fprintf(out,"'%s'",z);
  }else{
    fprintf(out,"'");
    while( *z ){
      for(i=0; z[i] && z[i]!='\''; i++){}
      if( i==0 ){
        fprintf(out,"''");
        z++;
      }else if( z[i]=='\'' ){
        fprintf(out,"%.*s''",i,z);
        z += i+1;
      }else{
        fprintf(out,"%s",z);
        break;
      }
    }
    fprintf(out,"'");
  }
}

/*
** Output the given string as a quoted according to C or TCL quoting rules.
*/
static void output_c_string(FILE *out, const char *z){
  unsigned int c;
  fputc('"', out);
  while( (c = *(z++))!=0 ){
    if( c=='\\' ){
      fputc(c, out);
      fputc(c, out);
    }else if( c=='\t' ){
      fputc('\\', out);
      fputc('t', out);
    }else if( c=='\n' ){
      fputc('\\', out);
      fputc('n', out);
    }else if( c=='\r' ){
      fputc('\\', out);
      fputc('r', out);
    }else if( !isprint(c) ){
      fprintf(out, "\\%03o", c&0xff);
    }else{
      fputc(c, out);
    }
  }
  fputc('"', out);
}

/*
** Output the given string with characters that are special to
** HTML escaped.
*/
static void output_html_string(FILE *out, const char *z){
  int i;
  while( *z ){
    for(i=0; z[i] && z[i]!='<' && z[i]!='&'; i++){}
    if( i>0 ){
      fprintf(out,"%.*s",i,z);
    }
    if( z[i]=='<' ){
      fprintf(out,"&lt;");
    }else if( z[i]=='&' ){
      fprintf(out,"&amp;");
    }else{
      break;
    }
    z += i + 1;
  }
}

/*
** If a field contains any character identified by a 1 in the following
** array, then the string must be quoted for CSV.
*/
static const char needCsvQuote[] = {
  1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1,   
  1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1,   
  1, 0, 1, 0, 0, 0, 0, 1,   0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 1, 
  1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1,   
  1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1,   
  1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1,   
  1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1,   
  1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1,   
  1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1,   
  1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1,   
  1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1,   
};

/*
** Output a single term of CSV.  Actually, p->separator is used for
** the separator, which may or may not be a comma.  p->nullvalue is
** the null value.  Strings are quoted using ANSI-C rules.  Numbers
** appear outside of quotes.
*/
static void output_csv(struct callback_data *p, const char *z, int bSep){
  FILE *out = p->out;
  if( z==0 ){
    fprintf(out,"%s",p->nullvalue);
  }else{
    int i;
    for(i=0; z[i]; i++){
      if( needCsvQuote[((unsigned char*)z)[i]] ){
        i = 0;
        break;
      }
    }
    if( i==0 ){
      putc('"', out);
      for(i=0; z[i]; i++){
        if( z[i]=='"' ) putc('"', out);
        putc(z[i], out);
      }
      putc('"', out);
    }else{
      fprintf(out, "%s", z);
    }
  }
  if( bSep ){
    fprintf(p->out, p->separator);
  }
}

#ifdef SIGINT
/*
** This routine runs when the user presses Ctrl-C
*/
static void interrupt_handler(int NotUsed){
  seenInterrupt = 1;
  if( db ) sqlite3_interrupt(db);
}
#endif

/*
** This is the callback routine that the SQLite library
** invokes for each row of a query result.
*/
static int callback(void *pArg, int nArg, char **azArg, char **azCol){
  int i;
  struct callback_data *p = (struct callback_data*)pArg;
  switch( p->mode ){
    case MODE_Line: {
      int w = 5;
      if( azArg==0 ) break;
      for(i=0; i<nArg; i++){
        int len = strlen(azCol[i] ? azCol[i] : "");
        if( len>w ) w = len;
      }
      if( p->cnt++>0 ) fprintf(p->out,"\n");
      for(i=0; i<nArg; i++){
        fprintf(p->out,"%*s = %s\n", w, azCol[i],
                azArg[i] ? azArg[i] : p->nullvalue);
      }
      break;
    }
    case MODE_Column: {
      if( p->cnt++==0 ){
        for(i=0; i<nArg; i++){
          int w, n;
          if( i<ArraySize(p->colWidth) ){
             w = p->colWidth[i];
          }else{
             w = 0;
          }
          if( w<=0 ){
            w = strlen(azCol[i] ? azCol[i] : "");
            if( w<10 ) w = 10;
            n = strlen(azArg && azArg[i] ? azArg[i] : p->nullvalue);
            if( w<n ) w = n;
          }
          if( i<ArraySize(p->actualWidth) ){
            p->actualWidth[i] = w;
          }
          if( p->showHeader ){
            fprintf(p->out,"%-*.*s%s",w,w,azCol[i], i==nArg-1 ? "\n": "  ");
          }
        }
        if( p->showHeader ){
          for(i=0; i<nArg; i++){
            int w;
            if( i<ArraySize(p->actualWidth) ){
               w = p->actualWidth[i];
            }else{
               w = 10;
            }
            fprintf(p->out,"%-*.*s%s",w,w,"-----------------------------------"
                   "----------------------------------------------------------",
                    i==nArg-1 ? "\n": "  ");
          }
        }
      }
      if( azArg==0 ) break;
      for(i=0; i<nArg; i++){
        int w;
        if( i<ArraySize(p->actualWidth) ){
           w = p->actualWidth[i];
        }else{
           w = 10;
        }
        fprintf(p->out,"%-*.*s%s",w,w,
            azArg[i] ? azArg[i] : p->nullvalue, i==nArg-1 ? "\n": "  ");
      }
      break;
    }
    case MODE_Semi:
    case MODE_List: {
      if( p->cnt++==0 && p->showHeader ){
        for(i=0; i<nArg; i++){
          fprintf(p->out,"%s%s",azCol[i], i==nArg-1 ? "\n" : p->separator);
        }
      }
      if( azArg==0 ) break;
      for(i=0; i<nArg; i++){
        char *z = azArg[i];
        if( z==0 ) z = p->nullvalue;
        fprintf(p->out, "%s", z);
        if( i<nArg-1 ){
          fprintf(p->out, "%s", p->separator);
        }else if( p->mode==MODE_Semi ){
          fprintf(p->out, ";\n");
        }else{
          fprintf(p->out, "\n");
        }
      }
      break;
    }
    case MODE_Html: {
      if( p->cnt++==0 && p->showHeader ){
        fprintf(p->out,"<TR>");
        for(i=0; i<nArg; i++){
          fprintf(p->out,"<TH>%s</TH>",azCol[i]);
        }
        fprintf(p->out,"</TR>\n");
      }
      if( azArg==0 ) break;
      fprintf(p->out,"<TR>");
      for(i=0; i<nArg; i++){
        fprintf(p->out,"<TD>");
        output_html_string(p->out, azArg[i] ? azArg[i] : p->nullvalue);
        fprintf(p->out,"</TD>\n");
      }
      fprintf(p->out,"</TR>\n");
      break;
    }
    case MODE_Tcl: {
      if( p->cnt++==0 && p->showHeader ){
        for(i=0; i<nArg; i++){
          output_c_string(p->out,azCol[i] ? azCol[i] : "");
          fprintf(p->out, "%s", p->separator);
        }
        fprintf(p->out,"\n");
      }
      if( azArg==0 ) break;
      for(i=0; i<nArg; i++){
        output_c_string(p->out, azArg[i] ? azArg[i] : p->nullvalue);
        fprintf(p->out, "%s", p->separator);
      }
      fprintf(p->out,"\n");
      break;
    }
    case MODE_Csv: {
      if( p->cnt++==0 && p->showHeader ){
        for(i=0; i<nArg; i++){
          output_csv(p, azCol[i] ? azCol[i] : "", i<nArg-1);
        }
        fprintf(p->out,"\n");
      }
      if( azArg==0 ) break;
      for(i=0; i<nArg; i++){
        output_csv(p, azArg[i], i<nArg-1);
      }
      fprintf(p->out,"\n");
      break;
    }
    case MODE_Insert: {
      if( azArg==0 ) break;
      fprintf(p->out,"INSERT INTO %s VALUES(",p->zDestTable);
      for(i=0; i<nArg; i++){
        char *zSep = i>0 ? ",": "";
        if( azArg[i]==0 ){
          fprintf(p->out,"%sNULL",zSep);
        }else if( isNumber(azArg[i], 0) ){
          fprintf(p->out,"%s%s",zSep, azArg[i]);
        }else{
          if( zSep[0] ) fprintf(p->out,"%s",zSep);
          output_quoted_string(p->out, azArg[i]);
        }
      }
      fprintf(p->out,");\n");
      break;
    }
  }
  return 0;
}

/*
** Set the destination table field of the callback_data structure to
** the name of the table given.  Escape any quote characters in the
** table name.
*/
static void set_table_name(struct callback_data *p, const char *zName){
  int i, n;
  int needQuote;
  char *z;

  if( p->zDestTable ){
    free(p->zDestTable);
    p->zDestTable = 0;
  }
  if( zName==0 ) return;
  needQuote = !isalpha((unsigned char)*zName) && *zName!='_';
  for(i=n=0; zName[i]; i++, n++){
    if( !isalnum((unsigned char)zName[i]) && zName[i]!='_' ){
      needQuote = 1;
      if( zName[i]=='\'' ) n++;
    }
  }
  if( needQuote ) n += 2;
  z = p->zDestTable = malloc( n+1 );
  if( z==0 ){
    fprintf(stderr,"Out of memory!\n");
    exit(1);
  }
  n = 0;
  if( needQuote ) z[n++] = '\'';
  for(i=0; zName[i]; i++){
    z[n++] = zName[i];
    if( zName[i]=='\'' ) z[n++] = '\'';
  }
  if( needQuote ) z[n++] = '\'';
  z[n] = 0;
}

/* zIn is either a pointer to a NULL-terminated string in memory obtained
** from malloc(), or a NULL pointer. The string pointed to by zAppend is
** added to zIn, and the result returned in memory obtained from malloc().
** zIn, if it was not NULL, is freed.
**
** If the third argument, quote, is not '\0', then it is used as a 
** quote character for zAppend.
*/
static char *appendText(char *zIn, char const *zAppend, char quote){
  int len;
  int i;
  int nAppend = strlen(zAppend);
  int nIn = (zIn?strlen(zIn):0);

  len = nAppend+nIn+1;
  if( quote ){
    len += 2;
    for(i=0; i<nAppend; i++){
      if( zAppend[i]==quote ) len++;
    }
  }

  zIn = (char *)realloc(zIn, len);
  if( !zIn ){
    return 0;
  }

  if( quote ){
    char *zCsr = &zIn[nIn];
    *zCsr++ = quote;
    for(i=0; i<nAppend; i++){
      *zCsr++ = zAppend[i];
      if( zAppend[i]==quote ) *zCsr++ = quote;
    }
    *zCsr++ = quote;
    *zCsr++ = '\0';
    assert( (zCsr-zIn)==len );
  }else{
    memcpy(&zIn[nIn], zAppend, nAppend);
    zIn[len-1] = '\0';
  }

  return zIn;
}


/*
** Execute a query statement that has a single result column.  Print
** that result column on a line by itself with a semicolon terminator.
**
** This is used, for example, to show the schema of the database by
** querying the SQLITE_MASTER table.
*/
static int run_table_dump_query(FILE *out, sqlite3 *db, const char *zSelect){
  sqlite3_stmt *pSelect;
  int rc;
  rc = sqlite3_prepare(db, zSelect, -1, &pSelect, 0);
  if( rc!=SQLITE_OK || !pSelect ){
    return rc;
  }
  rc = sqlite3_step(pSelect);
  while( rc==SQLITE_ROW ){
    fprintf(out, "%s;\n", sqlite3_column_text(pSelect, 0));
    rc = sqlite3_step(pSelect);
  }
  return sqlite3_finalize(pSelect);
}


/*
** This is a different callback routine used for dumping the database.
** Each row received by this callback consists of a table name,
** the table type ("index" or "table") and SQL to create the table.
** This routine should print text sufficient to recreate the table.
*/
static int dump_callback(void *pArg, int nArg, char **azArg, char **azCol){
  int rc;
  const char *zTable;
  const char *zType;
  const char *zSql;
  struct callback_data *p = (struct callback_data *)pArg;

  if( nArg!=3 ) return 1;
  zTable = azArg[0];
  zType = azArg[1];
  zSql = azArg[2];
  
  if( strcmp(zTable, "sqlite_sequence")==0 ){
    fprintf(p->out, "DELETE FROM sqlite_sequence;\n");
  }else if( strcmp(zTable, "sqlite_stat1")==0 ){
    fprintf(p->out, "ANALYZE sqlite_master;\n");
  }else if( strncmp(zTable, "sqlite_", 7)==0 ){
    return 0;
  }else if( strncmp(zSql, "CREATE VIRTUAL TABLE", 20)==0 ){
    char *zIns;
    if( !p->writableSchema ){
      fprintf(p->out, "PRAGMA writable_schema=ON;\n");
      p->writableSchema = 1;
    }
    zIns = sqlite3_mprintf(
       "INSERT INTO sqlite_master(type,name,tbl_name,rootpage,sql)"
       "VALUES('table','%q','%q',0,'%q');",
       zTable, zTable, zSql);
    fprintf(p->out, "%s\n", zIns);
    sqlite3_free(zIns);
    return 0;
  }else{
    fprintf(p->out, "%s;\n", zSql);
  }

  if( strcmp(zType, "table")==0 ){
    sqlite3_stmt *pTableInfo = 0;
    char *zSelect = 0;
    char *zTableInfo = 0;
    char *zTmp = 0;
   
    zTableInfo = appendText(zTableInfo, "PRAGMA table_info(", 0);
    zTableInfo = appendText(zTableInfo, zTable, '"');
    zTableInfo = appendText(zTableInfo, ");", 0);

    rc = sqlite3_prepare(p->db, zTableInfo, -1, &pTableInfo, 0);
    if( zTableInfo ) free(zTableInfo);
    if( rc!=SQLITE_OK || !pTableInfo ){
      return 1;
    }

    zSelect = appendText(zSelect, "SELECT 'INSERT INTO ' || ", 0);
    zTmp = appendText(zTmp, zTable, '"');
    if( zTmp ){
      zSelect = appendText(zSelect, zTmp, '\'');
    }
    zSelect = appendText(zSelect, " || ' VALUES(' || ", 0);
    rc = sqlite3_step(pTableInfo);
    while( rc==SQLITE_ROW ){
      const char *zText = (const char *)sqlite3_column_text(pTableInfo, 1);
      zSelect = appendText(zSelect, "quote(", 0);
      zSelect = appendText(zSelect, zText, '"');
      rc = sqlite3_step(pTableInfo);
      if( rc==SQLITE_ROW ){
        zSelect = appendText(zSelect, ") || ',' || ", 0);
      }else{
        zSelect = appendText(zSelect, ") ", 0);
      }
    }
    rc = sqlite3_finalize(pTableInfo);
    if( rc!=SQLITE_OK ){
      if( zSelect ) free(zSelect);
      return 1;
    }
    zSelect = appendText(zSelect, "|| ')' FROM  ", 0);
    zSelect = appendText(zSelect, zTable, '"');

    rc = run_table_dump_query(p->out, p->db, zSelect);
    if( rc==SQLITE_CORRUPT ){
      zSelect = appendText(zSelect, " ORDER BY rowid DESC", 0);
      rc = run_table_dump_query(p->out, p->db, zSelect);
    }
    if( zSelect ) free(zSelect);
  }
  return 0;
}

/*
** Run zQuery.  Use dump_callback() as the callback routine so that
** the contents of the query are output as SQL statements.
**
** If we get a SQLITE_CORRUPT error, rerun the query after appending
** "ORDER BY rowid DESC" to the end.
*/
static int run_schema_dump_query(
  struct callback_data *p, 
  const char *zQuery,
  char **pzErrMsg
){
  int rc;
  rc = sqlite3_exec(p->db, zQuery, dump_callback, p, pzErrMsg);
  if( rc==SQLITE_CORRUPT ){
    char *zQ2;
    int len = strlen(zQuery);
    if( pzErrMsg ) sqlite3_free(*pzErrMsg);
    zQ2 = malloc( len+100 );
    if( zQ2==0 ) return rc;
    sprintf(zQ2, "%s ORDER BY rowid DESC", zQuery);
    rc = sqlite3_exec(p->db, zQ2, dump_callback, p, pzErrMsg);
    free(zQ2);
  }
  return rc;
}

/*
** Text of a help message
*/
static char zHelp[] =
  ".bail ON|OFF           Stop after hitting an error.  Default OFF\n"
  ".databases             List names and files of attached databases\n"
  ".dump ?TABLE? ...      Dump the database in an SQL text format\n"
  ".echo ON|OFF           Turn command echo on or off\n"
  ".exit                  Exit this program\n"
  ".explain ON|OFF        Turn output mode suitable for EXPLAIN on or off.\n"
  ".header(s) ON|OFF      Turn display of headers on or off\n"
  ".help                  Show this message\n"
  ".import FILE TABLE     Import data from FILE into TABLE\n"
  ".indices TABLE         Show names of all indices on TABLE\n"
#ifndef SQLITE_OMIT_LOAD_EXTENSION
  ".load FILE ?ENTRY?     Load an extension library\n"
#endif
  ".mode MODE ?TABLE?     Set output mode where MODE is one of:\n"
  "                         csv      Comma-separated values\n"
  "                         column   Left-aligned columns.  (See .width)\n"
  "                         html     HTML <table> code\n"
  "                         insert   SQL insert statements for TABLE\n"
  "                         line     One value per line\n"
  "                         list     Values delimited by .separator string\n"
  "                         tabs     Tab-separated values\n"
  "                         tcl      TCL list elements\n"
  ".nullvalue STRING      Print STRING in place of NULL values\n"
  ".output FILENAME       Send output to FILENAME\n"
  ".output stdout         Send output to the screen\n"
  ".prompt MAIN CONTINUE  Replace the standard prompts\n"
  ".quit                  Exit this program\n"
  ".read FILENAME         Execute SQL in FILENAME\n"
  ".schema ?TABLE?        Show the CREATE statements\n"
  ".separator STRING      Change separator used by output mode and .import\n"
  ".show                  Show the current values for various settings\n"
  ".tables ?PATTERN?      List names of tables matching a LIKE pattern\n"
  ".timeout MS            Try opening locked tables for MS milliseconds\n"
  ".width NUM NUM ...     Set column widths for \"column\" mode\n"
;

/* Forward reference */
static int process_input(struct callback_data *p, FILE *in);

/*
** Make sure the database is open.  If it is not, then open it.  If
** the database fails to open, print an error message and exit.
*/
static void open_db(struct callback_data *p){
  if( p->db==0 ){
    sqlite3_open(p->zDbFilename, &p->db);
    db = p->db;
    sqlite3_create_function(db, "shellstatic", 0, SQLITE_UTF8, 0,
        shellstaticFunc, 0, 0);
    if( SQLITE_OK!=sqlite3_errcode(db) ){
      fprintf(stderr,"Unable to open database \"%s\": %s\n", 
          p->zDbFilename, sqlite3_errmsg(db));
      exit(1);
    }
#ifndef SQLITE_OMIT_LOAD_EXTENSION
    sqlite3_enable_load_extension(p->db, 1);
#endif
  }
}

/*
** Do C-language style dequoting.
**
**    \t    -> tab
**    \n    -> newline
**    \r    -> carriage return
**    \NNN  -> ascii character NNN in octal
**    \\    -> backslash
*/
static void resolve_backslashes(char *z){
  int i, j, c;
  for(i=j=0; (c = z[i])!=0; i++, j++){
    if( c=='\\' ){
      c = z[++i];
      if( c=='n' ){
        c = '\n';
      }else if( c=='t' ){
        c = '\t';
      }else if( c=='r' ){
        c = '\r';
      }else if( c>='0' && c<='7' ){
        c -= '0';
        if( z[i+1]>='0' && z[i+1]<='7' ){
          i++;
          c = (c<<3) + z[i] - '0';
          if( z[i+1]>='0' && z[i+1]<='7' ){
            i++;
            c = (c<<3) + z[i] - '0';
          }
        }
      }
    }
    z[j] = c;
  }
  z[j] = 0;
}

/*
** Interpret zArg as a boolean value.  Return either 0 or 1.
*/
static int booleanValue(char *zArg){
  int val = atoi(zArg);
  int j;
  for(j=0; zArg[j]; j++){
    zArg[j] = tolower(zArg[j]);
  }
  if( strcmp(zArg,"on")==0 ){
    val = 1;
  }else if( strcmp(zArg,"yes")==0 ){
    val = 1;
  }
  return val;
}

/*
** If an input line begins with "." then invoke this routine to
** process that line.
**
** Return 1 on error, 2 to exit, and 0 otherwise.
*/
static int do_meta_command(char *zLine, struct callback_data *p){
  int i = 1;
  int nArg = 0;
  int n, c;
  int rc = 0;
  char *azArg[50];

  /* Parse the input line into tokens.
  */
  while( zLine[i] && nArg<ArraySize(azArg) ){
    while( isspace((unsigned char)zLine[i]) ){ i++; }
    if( zLine[i]==0 ) break;
    if( zLine[i]=='\'' || zLine[i]=='"' ){
      int delim = zLine[i++];
      azArg[nArg++] = &zLine[i];
      while( zLine[i] && zLine[i]!=delim ){ i++; }
      if( zLine[i]==delim ){
        zLine[i++] = 0;
      }
      if( delim=='"' ) resolve_backslashes(azArg[nArg-1]);
    }else{
      azArg[nArg++] = &zLine[i];
      while( zLine[i] && !isspace((unsigned char)zLine[i]) ){ i++; }
      if( zLine[i] ) zLine[i++] = 0;
      resolve_backslashes(azArg[nArg-1]);
    }
  }

  /* Process the input line.
  */
  if( nArg==0 ) return rc;
  n = strlen(azArg[0]);
  c = azArg[0][0];
  if( c=='b' && n>1 && strncmp(azArg[0], "bail", n)==0 && nArg>1 ){
    bail_on_error = booleanValue(azArg[1]);
  }else

  if( c=='d' && n>1 && strncmp(azArg[0], "databases", n)==0 ){
    struct callback_data data;
    char *zErrMsg = 0;
    open_db(p);
    memcpy(&data, p, sizeof(data));
    data.showHeader = 1;
    data.mode = MODE_Column;
    data.colWidth[0] = 3;
    data.colWidth[1] = 15;
    data.colWidth[2] = 58;
    data.cnt = 0;
    sqlite3_exec(p->db, "PRAGMA database_list; ", callback, &data, &zErrMsg);
    if( zErrMsg ){
      fprintf(stderr,"Error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
  }else

  if( c=='d' && strncmp(azArg[0], "dump", n)==0 ){
    char *zErrMsg = 0;
    open_db(p);
    fprintf(p->out, "BEGIN TRANSACTION;\n");
    p->writableSchema = 0;
    if( nArg==1 ){
      run_schema_dump_query(p, 
        "SELECT name, type, sql FROM sqlite_master "
        "WHERE sql NOT NULL AND type=='table'", 0
      );
      run_table_dump_query(p->out, p->db,
        "SELECT sql FROM sqlite_master "
        "WHERE sql NOT NULL AND type IN ('index','trigger','view')"
      );
    }else{
      int i;
      for(i=1; i<nArg; i++){
        zShellStatic = azArg[i];
        run_schema_dump_query(p,
          "SELECT name, type, sql FROM sqlite_master "
          "WHERE tbl_name LIKE shellstatic() AND type=='table'"
          "  AND sql NOT NULL", 0);
        run_table_dump_query(p->out, p->db,
          "SELECT sql FROM sqlite_master "
          "WHERE sql NOT NULL"
          "  AND type IN ('index','trigger','view')"
          "  AND tbl_name LIKE shellstatic()"
        );
        zShellStatic = 0;
      }
    }
    if( p->writableSchema ){
      fprintf(p->out, "PRAGMA writable_schema=OFF;\n");
      p->writableSchema = 0;
    }
    if( zErrMsg ){
      fprintf(stderr,"Error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }else{
      fprintf(p->out, "COMMIT;\n");
    }
  }else

  if( c=='e' && strncmp(azArg[0], "echo", n)==0 && nArg>1 ){
    p->echoOn = booleanValue(azArg[1]);
  }else

  if( c=='e' && strncmp(azArg[0], "exit", n)==0 ){
    rc = 2;
  }else

  if( c=='e' && strncmp(azArg[0], "explain", n)==0 ){
    int val = nArg>=2 ? booleanValue(azArg[1]) : 1;
    if(val == 1) {
      if(!p->explainPrev.valid) {
        p->explainPrev.valid = 1;
        p->explainPrev.mode = p->mode;
        p->explainPrev.showHeader = p->showHeader;
        memcpy(p->explainPrev.colWidth,p->colWidth,sizeof(p->colWidth));
      }
      /* We could put this code under the !p->explainValid
      ** condition so that it does not execute if we are already in
      ** explain mode. However, always executing it allows us an easy
      ** was to reset to explain mode in case the user previously
      ** did an .explain followed by a .width, .mode or .header
      ** command.
      */
      p->mode = MODE_Column;
      p->showHeader = 1;
      memset(p->colWidth,0,ArraySize(p->colWidth));
      p->colWidth[0] = 4;
      p->colWidth[1] = 14;
      p->colWidth[2] = 10;
      p->colWidth[3] = 10;
      p->colWidth[4] = 33;
    }else if (p->explainPrev.valid) {
      p->explainPrev.valid = 0;
      p->mode = p->explainPrev.mode;
      p->showHeader = p->explainPrev.showHeader;
      memcpy(p->colWidth,p->explainPrev.colWidth,sizeof(p->colWidth));
    }
  }else

  if( c=='h' && (strncmp(azArg[0], "header", n)==0 ||
                 strncmp(azArg[0], "headers", n)==0 )&& nArg>1 ){
    p->showHeader = booleanValue(azArg[1]);
  }else

  if( c=='h' && strncmp(azArg[0], "help", n)==0 ){
    fprintf(stderr,zHelp);
  }else

  if( c=='i' && strncmp(azArg[0], "import", n)==0 && nArg>=3 ){
    char *zTable = azArg[2];    /* Insert data into this table */
    char *zFile = azArg[1];     /* The file from which to extract data */
    sqlite3_stmt *pStmt;        /* A statement */
    int rc;                     /* Result code */
    int nCol;                   /* Number of columns in the table */
    int nByte;                  /* Number of bytes in an SQL string */
    int i, j;                   /* Loop counters */
    int nSep;                   /* Number of bytes in p->separator[] */
    char *zSql;                 /* An SQL statement */
    char *zLine;                /* A single line of input from the file */
    char **azCol;               /* zLine[] broken up into columns */
    char *zCommit;              /* How to commit changes */   
    FILE *in;                   /* The input file */
    int lineno = 0;             /* Line number of input file */

    open_db(p);
    nSep = strlen(p->separator);
    if( nSep==0 ){
      fprintf(stderr, "non-null separator required for import\n");
      return 0;
    }
    zSql = sqlite3_mprintf("SELECT * FROM '%q'", zTable);
    if( zSql==0 ) return 0;
    nByte = strlen(zSql);
    rc = sqlite3_prepare(p->db, zSql, -1, &pStmt, 0);
    sqlite3_free(zSql);
    if( rc ){
      fprintf(stderr,"Error: %s\n", sqlite3_errmsg(db));
      nCol = 0;
      rc = 1;
    }else{
      nCol = sqlite3_column_count(pStmt);
    }
    sqlite3_finalize(pStmt);
    if( nCol==0 ) return 0;
    zSql = malloc( nByte + 20 + nCol*2 );
    if( zSql==0 ) return 0;
    sqlite3_snprintf(nByte+20, zSql, "INSERT INTO '%q' VALUES(?", zTable);
    j = strlen(zSql);
    for(i=1; i<nCol; i++){
      zSql[j++] = ',';
      zSql[j++] = '?';
    }
    zSql[j++] = ')';
    zSql[j] = 0;
    rc = sqlite3_prepare(p->db, zSql, -1, &pStmt, 0);
    free(zSql);
    if( rc ){
      fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
      sqlite3_finalize(pStmt);
      return 1;
    }
    in = fopen(zFile, "rb");
    if( in==0 ){
      fprintf(stderr, "cannot open file: %s\n", zFile);
      sqlite3_finalize(pStmt);
      return 0;
    }
    azCol = malloc( sizeof(azCol[0])*(nCol+1) );
    if( azCol==0 ){
      fclose(in);
      return 0;
    }
    sqlite3_exec(p->db, "BEGIN", 0, 0, 0);
    zCommit = "COMMIT";
    while( (zLine = local_getline(0, in))!=0 ){
      char *z;
      i = 0;
      lineno++;
      azCol[0] = zLine;
      for(i=0, z=zLine; *z && *z!='\n' && *z!='\r'; z++){
        if( *z==p->separator[0] && strncmp(z, p->separator, nSep)==0 ){
          *z = 0;
          i++;
          if( i<nCol ){
            azCol[i] = &z[nSep];
            z += nSep-1;
          }
        }
      }
      *z = 0;
      if( i+1!=nCol ){
        fprintf(stderr,"%s line %d: expected %d columns of data but found %d\n",
           zFile, lineno, nCol, i+1);
        zCommit = "ROLLBACK";
        break;
      }
      for(i=0; i<nCol; i++){
        sqlite3_bind_text(pStmt, i+1, azCol[i], -1, SQLITE_STATIC);
      }
      sqlite3_step(pStmt);
      rc = sqlite3_reset(pStmt);
      free(zLine);
      if( rc!=SQLITE_OK ){
        fprintf(stderr,"Error: %s\n", sqlite3_errmsg(db));
        zCommit = "ROLLBACK";
        rc = 1;
        break;
      }
    }
    free(azCol);
    fclose(in);
    sqlite3_finalize(pStmt);
    sqlite3_exec(p->db, zCommit, 0, 0, 0);
  }else

  if( c=='i' && strncmp(azArg[0], "indices", n)==0 && nArg>1 ){
    struct callback_data data;
    char *zErrMsg = 0;
    open_db(p);
    memcpy(&data, p, sizeof(data));
    data.showHeader = 0;
    data.mode = MODE_List;
    zShellStatic = azArg[1];
    sqlite3_exec(p->db,
      "SELECT name FROM sqlite_master "
      "WHERE type='index' AND tbl_name LIKE shellstatic() "
      "UNION ALL "
      "SELECT name FROM sqlite_temp_master "
      "WHERE type='index' AND tbl_name LIKE shellstatic() "
      "ORDER BY 1",
      callback, &data, &zErrMsg
    );
    zShellStatic = 0;
    if( zErrMsg ){
      fprintf(stderr,"Error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
  }else

  if( c=='i' && strncmp(azArg[0], "iotrace", n)==0 ){
    extern void (*sqlite3_io_trace)(const char*, ...);
    if( iotrace && iotrace!=stdout ) fclose(iotrace);
    iotrace = 0;
    if( nArg<2 ){
      sqlite3_io_trace = 0;
    }else if( strcmp(azArg[1], "-")==0 ){
      sqlite3_io_trace = iotracePrintf;
      iotrace = stdout;
    }else{
      iotrace = fopen(azArg[1], "w");
      if( iotrace==0 ){
        fprintf(stderr, "cannot open \"%s\"\n", azArg[1]);
        sqlite3_io_trace = 0;
      }else{
        sqlite3_io_trace = iotracePrintf;
      }
    }
  }else

#ifndef SQLITE_OMIT_LOAD_EXTENSION
  if( c=='l' && strncmp(azArg[0], "load", n)==0 && nArg>=2 ){
    const char *zFile, *zProc;
    char *zErrMsg = 0;
    int rc;
    zFile = azArg[1];
    zProc = nArg>=3 ? azArg[2] : 0;
    open_db(p);
    rc = sqlite3_load_extension(p->db, zFile, zProc, &zErrMsg);
    if( rc!=SQLITE_OK ){
      fprintf(stderr, "%s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      rc = 1;
    }
  }else
#endif

  if( c=='m' && strncmp(azArg[0], "mode", n)==0 && nArg>=2 ){
    int n2 = strlen(azArg[1]);
    if( strncmp(azArg[1],"line",n2)==0
        ||
        strncmp(azArg[1],"lines",n2)==0 ){
      p->mode = MODE_Line;
    }else if( strncmp(azArg[1],"column",n2)==0
              ||
              strncmp(azArg[1],"columns",n2)==0 ){
      p->mode = MODE_Column;
    }else if( strncmp(azArg[1],"list",n2)==0 ){
      p->mode = MODE_List;
    }else if( strncmp(azArg[1],"html",n2)==0 ){
      p->mode = MODE_Html;
    }else if( strncmp(azArg[1],"tcl",n2)==0 ){
      p->mode = MODE_Tcl;
    }else if( strncmp(azArg[1],"csv",n2)==0 ){
      p->mode = MODE_Csv;
      strcpy(p->separator, ",");
    }else if( strncmp(azArg[1],"tabs",n2)==0 ){
      p->mode = MODE_List;
      strcpy(p->separator, "\t");
    }else if( strncmp(azArg[1],"insert",n2)==0 ){
      p->mode = MODE_Insert;
      if( nArg>=3 ){
        set_table_name(p, azArg[2]);
      }else{
        set_table_name(p, "table");
      }
    }else {
      fprintf(stderr,"mode should be one of: "
         "column csv html insert line list tabs tcl\n");
    }
  }else

  if( c=='n' && strncmp(azArg[0], "nullvalue", n)==0 && nArg==2 ) {
    sprintf(p->nullvalue, "%.*s", (int)ArraySize(p->nullvalue)-1, azArg[1]);
  }else

  if( c=='o' && strncmp(azArg[0], "output", n)==0 && nArg==2 ){
    if( p->out!=stdout ){
      fclose(p->out);
    }
    if( strcmp(azArg[1],"stdout")==0 ){
      p->out = stdout;
      strcpy(p->outfile,"stdout");
    }else{
      p->out = fopen(azArg[1], "wb");
      if( p->out==0 ){
        fprintf(stderr,"can't write to \"%s\"\n", azArg[1]);
        p->out = stdout;
      } else {
         strcpy(p->outfile,azArg[1]);
      }
    }
  }else

  if( c=='p' && strncmp(azArg[0], "prompt", n)==0 && (nArg==2 || nArg==3)){
    if( nArg >= 2) {
      strncpy(mainPrompt,azArg[1],(int)ArraySize(mainPrompt)-1);
    }
    if( nArg >= 3) {
      strncpy(continuePrompt,azArg[2],(int)ArraySize(continuePrompt)-1);
    }
  }else

  if( c=='q' && strncmp(azArg[0], "quit", n)==0 ){
    rc = 2;
  }else

  if( c=='r' && strncmp(azArg[0], "read", n)==0 && nArg==2 ){
    FILE *alt = fopen(azArg[1], "rb");
    if( alt==0 ){
      fprintf(stderr,"can't open \"%s\"\n", azArg[1]);
    }else{
      process_input(p, alt);
      fclose(alt);
    }
  }else

  if( c=='s' && strncmp(azArg[0], "schema", n)==0 ){
    struct callback_data data;
    char *zErrMsg = 0;
    open_db(p);
    memcpy(&data, p, sizeof(data));
    data.showHeader = 0;
    data.mode = MODE_Semi;
    if( nArg>1 ){
      int i;
      for(i=0; azArg[1][i]; i++) azArg[1][i] = tolower(azArg[1][i]);
      if( strcmp(azArg[1],"sqlite_master")==0 ){
        char *new_argv[2], *new_colv[2];
        new_argv[0] = "CREATE TABLE sqlite_master (\n"
                      "  type text,\n"
                      "  name text,\n"
                      "  tbl_name text,\n"
                      "  rootpage integer,\n"
                      "  sql text\n"
                      ")";
        new_argv[1] = 0;
        new_colv[0] = "sql";
        new_colv[1] = 0;
        callback(&data, 1, new_argv, new_colv);
      }else if( strcmp(azArg[1],"sqlite_temp_master")==0 ){
        char *new_argv[2], *new_colv[2];
        new_argv[0] = "CREATE TEMP TABLE sqlite_temp_master (\n"
                      "  type text,\n"
                      "  name text,\n"
                      "  tbl_name text,\n"
                      "  rootpage integer,\n"
                      "  sql text\n"
                      ")";
        new_argv[1] = 0;
        new_colv[0] = "sql";
        new_colv[1] = 0;
        callback(&data, 1, new_argv, new_colv);
      }else{
        zShellStatic = azArg[1];
        sqlite3_exec(p->db,
          "SELECT sql FROM "
          "  (SELECT * FROM sqlite_master UNION ALL"
          "   SELECT * FROM sqlite_temp_master) "
          "WHERE tbl_name LIKE shellstatic() AND type!='meta' AND sql NOTNULL "
          "ORDER BY substr(type,2,1), name",
          callback, &data, &zErrMsg);
        zShellStatic = 0;
      }
    }else{
      sqlite3_exec(p->db,
         "SELECT sql FROM "
         "  (SELECT * FROM sqlite_master UNION ALL"
         "   SELECT * FROM sqlite_temp_master) "
         "WHERE type!='meta' AND sql NOTNULL AND name NOT LIKE 'sqlite_%'"
         "ORDER BY substr(type,2,1), name",
         callback, &data, &zErrMsg
      );
    }
    if( zErrMsg ){
      fprintf(stderr,"Error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
  }else

  if( c=='s' && strncmp(azArg[0], "separator", n)==0 && nArg==2 ){
    sprintf(p->separator, "%.*s", (int)ArraySize(p->separator)-1, azArg[1]);
  }else

  if( c=='s' && strncmp(azArg[0], "show", n)==0){
    int i;
    fprintf(p->out,"%9.9s: %s\n","echo", p->echoOn ? "on" : "off");
    fprintf(p->out,"%9.9s: %s\n","explain", p->explainPrev.valid ? "on" :"off");
    fprintf(p->out,"%9.9s: %s\n","headers", p->showHeader ? "on" : "off");
    fprintf(p->out,"%9.9s: %s\n","mode", modeDescr[p->mode]);
    fprintf(p->out,"%9.9s: ", "nullvalue");
      output_c_string(p->out, p->nullvalue);
      fprintf(p->out, "\n");
    fprintf(p->out,"%9.9s: %s\n","output",
                                 strlen(p->outfile) ? p->outfile : "stdout");
    fprintf(p->out,"%9.9s: ", "separator");
      output_c_string(p->out, p->separator);
      fprintf(p->out, "\n");
    fprintf(p->out,"%9.9s: ","width");
    for (i=0;i<(int)ArraySize(p->colWidth) && p->colWidth[i] != 0;i++) {
      fprintf(p->out,"%d ",p->colWidth[i]);
    }
    fprintf(p->out,"\n");
  }else

  if( c=='t' && n>1 && strncmp(azArg[0], "tables", n)==0 ){
    char **azResult;
    int nRow, rc;
    char *zErrMsg;
    open_db(p);
    if( nArg==1 ){
      rc = sqlite3_get_table(p->db,
        "SELECT name FROM sqlite_master "
        "WHERE type IN ('table','view') AND name NOT LIKE 'sqlite_%'"
        "UNION ALL "
        "SELECT name FROM sqlite_temp_master "
        "WHERE type IN ('table','view') "
        "ORDER BY 1",
        &azResult, &nRow, 0, &zErrMsg
      );
    }else{
      zShellStatic = azArg[1];
      rc = sqlite3_get_table(p->db,
        "SELECT name FROM sqlite_master "
        "WHERE type IN ('table','view') AND name LIKE '%'||shellstatic()||'%' "
        "UNION ALL "
        "SELECT name FROM sqlite_temp_master "
        "WHERE type IN ('table','view') AND name LIKE '%'||shellstatic()||'%' "
        "ORDER BY 1",
        &azResult, &nRow, 0, &zErrMsg
      );
      zShellStatic = 0;
    }
    if( zErrMsg ){
      fprintf(stderr,"Error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    if( rc==SQLITE_OK ){
      int len, maxlen = 0;
      int i, j;
      int nPrintCol, nPrintRow;
      for(i=1; i<=nRow; i++){
        if( azResult[i]==0 ) continue;
        len = strlen(azResult[i]);
        if( len>maxlen ) maxlen = len;
      }
      nPrintCol = 80/(maxlen+2);
      if( nPrintCol<1 ) nPrintCol = 1;
      nPrintRow = (nRow + nPrintCol - 1)/nPrintCol;
      for(i=0; i<nPrintRow; i++){
        for(j=i+1; j<=nRow; j+=nPrintRow){
          char *zSp = j<=nPrintRow ? "" : "  ";
          printf("%s%-*s", zSp, maxlen, azResult[j] ? azResult[j] : "");
        }
        printf("\n");
      }
    }else{
      rc = 1;
    }
    sqlite3_free_table(azResult);
  }else

  if( c=='t' && n>1 && strncmp(azArg[0], "timeout", n)==0 && nArg>=2 ){
    open_db(p);
    sqlite3_busy_timeout(p->db, atoi(azArg[1]));
  }else

  if( c=='w' && strncmp(azArg[0], "width", n)==0 ){
    int j;
    assert( nArg<=ArraySize(azArg) );
    for(j=1; j<nArg && j<ArraySize(p->colWidth); j++){
      p->colWidth[j-1] = atoi(azArg[j]);
    }
  }else

  {
    fprintf(stderr, "unknown command or invalid arguments: "
      " \"%s\". Enter \".help\" for help\n", azArg[0]);
  }

  return rc;
}

/*
** Return TRUE if the last non-whitespace character in z[] is a semicolon.
** z[] is N characters long.
*/
static int _ends_with_semicolon(const char *z, int N){
  while( N>0 && isspace((unsigned char)z[N-1]) ){ N--; }
  return N>0 && z[N-1]==';';
}

/*
** Test to see if a line consists entirely of whitespace.
*/
static int _all_whitespace(const char *z){
  for(; *z; z++){
    if( isspace(*(unsigned char*)z) ) continue;
    if( *z=='/' && z[1]=='*' ){
      z += 2;
      while( *z && (*z!='*' || z[1]!='/') ){ z++; }
      if( *z==0 ) return 0;
      z++;
      continue;
    }
    if( *z=='-' && z[1]=='-' ){
      z += 2;
      while( *z && *z!='\n' ){ z++; }
      if( *z==0 ) return 1;
      continue;
    }
    return 0;
  }
  return 1;
}

/*
** Return TRUE if the line typed in is an SQL command terminator other
** than a semi-colon.  The SQL Server style "go" command is understood
** as is the Oracle "/".
*/
static int _is_command_terminator(const char *zLine){
  while( isspace(*(unsigned char*)zLine) ){ zLine++; };
  if( zLine[0]=='/' && _all_whitespace(&zLine[1]) ) return 1;  /* Oracle */
  if( tolower(zLine[0])=='g' && tolower(zLine[1])=='o'
         && _all_whitespace(&zLine[2]) ){
    return 1;  /* SQL Server */
  }
  return 0;
}

/*
** Read input from *in and process it.  If *in==0 then input
** is interactive - the user is typing it it.  Otherwise, input
** is coming from a file or device.  A prompt is issued and history
** is saved only if input is interactive.  An interrupt signal will
** cause this routine to exit immediately, unless input is interactive.
**
** Return the number of errors.
*/
static int process_input(struct callback_data *p, FILE *in){
  char *zLine;
  char *zSql = 0;
  int nSql = 0;
  char *zErrMsg;
  int rc;
  int errCnt = 0;
  int lineno = 0;
  int startline = 0;

  while( errCnt==0 || !bail_on_error || (in==0 && stdin_is_interactive) ){
    fflush(p->out);
    zLine = one_input_line(zSql, in);
    if( zLine==0 ){
      break;  /* We have reached EOF */
    }
    if( seenInterrupt ){
      if( in!=0 ) break;
      seenInterrupt = 0;
    }
    lineno++;
    if( p->echoOn ) printf("%s\n", zLine);
    if( (zSql==0 || zSql[0]==0) && _all_whitespace(zLine) ) continue;
    if( zLine && zLine[0]=='.' && nSql==0 ){
      rc = do_meta_command(zLine, p);
      free(zLine);
      if( rc==2 ){
        break;
      }else if( rc ){
        errCnt++;
      }
      continue;
    }
    if( _is_command_terminator(zLine) ){
      strcpy(zLine,";");
    }
    if( zSql==0 ){
      int i;
      for(i=0; zLine[i] && isspace((unsigned char)zLine[i]); i++){}
      if( zLine[i]!=0 ){
        nSql = strlen(zLine);
        zSql = malloc( nSql+1 );
        if( zSql==0 ){
          fprintf(stderr, "out of memory\n");
          exit(1);
        }
        strcpy(zSql, zLine);
        startline = lineno;
      }
    }else{
      int len = strlen(zLine);
      zSql = realloc( zSql, nSql + len + 2 );
      if( zSql==0 ){
        fprintf(stderr,"%s: out of memory!\n", Argv0);
        exit(1);
      }
      strcpy(&zSql[nSql++], "\n");
      strcpy(&zSql[nSql], zLine);
      nSql += len;
    }
    free(zLine);
    if( zSql && _ends_with_semicolon(zSql, nSql) && sqlite3_complete(zSql) ){
      p->cnt = 0;
      open_db(p);
      rc = sqlite3_exec(p->db, zSql, callback, p, &zErrMsg);
      if( rc || zErrMsg ){
        char zPrefix[100];
        if( in!=0 || !stdin_is_interactive ){
          sprintf(zPrefix, "SQL error near line %d:", startline);
        }else{
          sprintf(zPrefix, "SQL error:");
        }
        if( zErrMsg!=0 ){
          printf("%s %s\n", zPrefix, zErrMsg);
          sqlite3_free(zErrMsg);
          zErrMsg = 0;
        }else{
          printf("%s %s\n", zPrefix, sqlite3_errmsg(p->db));
        }
        errCnt++;
      }
      free(zSql);
      zSql = 0;
      nSql = 0;
    }
  }
  if( zSql ){
    if( !_all_whitespace(zSql) ) printf("Incomplete SQL: %s\n", zSql);
    free(zSql);
  }
  return errCnt;
}

/*
** Return a pathname which is the user's home directory.  A
** 0 return indicates an error of some kind.  Space to hold the
** resulting string is obtained from malloc().  The calling
** function should free the result.
*/
static char *find_home_dir(void){
  char *home_dir = NULL;

#if !defined(_WIN32) && !defined(WIN32) && !defined(__MACOS__) && !defined(__OS2__)
  struct passwd *pwent;
  uid_t uid = getuid();
  if( (pwent=getpwuid(uid)) != NULL) {
    home_dir = pwent->pw_dir;
  }
#endif

#ifdef __MACOS__
  char home_path[_MAX_PATH+1];
  home_dir = getcwd(home_path, _MAX_PATH);
#endif

#if defined(_WIN32) || defined(WIN32) || defined(__OS2__)
  if (!home_dir) {
    home_dir = getenv("USERPROFILE");
  }
#endif

  if (!home_dir) {
    home_dir = getenv("HOME");
  }

#if defined(_WIN32) || defined(WIN32) || defined(__OS2__)
  if (!home_dir) {
    char *zDrive, *zPath;
    int n;
    zDrive = getenv("HOMEDRIVE");
    zPath = getenv("HOMEPATH");
    if( zDrive && zPath ){
      n = strlen(zDrive) + strlen(zPath) + 1;
      home_dir = malloc( n );
      if( home_dir==0 ) return 0;
      sqlite3_snprintf(n, home_dir, "%s%s", zDrive, zPath);
      return home_dir;
    }
    home_dir = "c:\\";
  }
#endif

  if( home_dir ){
    char *z = malloc( strlen(home_dir)+1 );
    if( z ) strcpy(z, home_dir);
    home_dir = z;
  }

  return home_dir;
}

/*
** Read input from the file given by sqliterc_override.  Or if that
** parameter is NULL, take input from ~/.sqliterc
*/
static void process_sqliterc(
  struct callback_data *p,        /* Configuration data */
  const char *sqliterc_override   /* Name of config file. NULL to use default */
){
  char *home_dir = NULL;
  const char *sqliterc = sqliterc_override;
  char *zBuf = 0;
  FILE *in = NULL;

  if (sqliterc == NULL) {
    home_dir = find_home_dir();
    if( home_dir==0 ){
      fprintf(stderr,"%s: cannot locate your home directory!\n", Argv0);
      return;
    }
    zBuf = malloc(strlen(home_dir) + 15);
    if( zBuf==0 ){
      fprintf(stderr,"%s: out of memory!\n", Argv0);
      exit(1);
    }
    sprintf(zBuf,"%s/.sqliterc",home_dir);
    free(home_dir);
    sqliterc = (const char*)zBuf;
  }
  in = fopen(sqliterc,"rb");
  if( in ){
    if( stdin_is_interactive ){
      printf("Loading resources from %s\n",sqliterc);
    }
    process_input(p,in);
    fclose(in);
  }
  free(zBuf);
  return;
}

/*
** Show available command line options
*/
static const char zOptions[] = 
  "   -init filename       read/process named file\n"
  "   -echo                print commands before execution\n"
  "   -[no]header          turn headers on or off\n"
  "   -bail                stop after hitting an error\n"
  "   -interactive         force interactive I/O\n"
  "   -batch               force batch I/O\n"
  "   -column              set output mode to 'column'\n"
  "   -csv                 set output mode to 'csv'\n"
  "   -html                set output mode to HTML\n"
  "   -line                set output mode to 'line'\n"
  "   -list                set output mode to 'list'\n"
  "   -separator 'x'       set output field separator (|)\n"
  "   -nullvalue 'text'    set text string for NULL values\n"
  "   -version             show SQLite version\n"
;
static void usage(int showDetail){
  fprintf(stderr,
      "Usage: %s [OPTIONS] FILENAME [SQL]\n"  
      "FILENAME is the name of an SQLite database. A new database is created\n"
      "if the file does not previously exist.\n", Argv0);
  if( showDetail ){
    fprintf(stderr, "OPTIONS include:\n%s", zOptions);
  }else{
    fprintf(stderr, "Use the -help option for additional information\n");
  }
  exit(1);
}

/*
** Initialize the state information in data
*/
static void main_init(struct callback_data *data) {
  memset(data, 0, sizeof(*data));
  data->mode = MODE_List;
  strcpy(data->separator,"|");
  data->showHeader = 0;
  strcpy(mainPrompt,"sqlite> ");
  strcpy(continuePrompt,"   ...> ");
}

int main(int argc, char **argv){
  char *zErrMsg = 0;
  struct callback_data data;
  const char *zInitFile = 0;
  char *zFirstCmd = 0;
  int i;
  int rc = 0;

#ifdef __MACOS__
  argc = ccommand(&argv);
#endif

  Argv0 = argv[0];
  main_init(&data);
  stdin_is_interactive = isatty(0);

  /* Make sure we have a valid signal handler early, before anything
  ** else is done.
  */
#ifdef SIGINT
  signal(SIGINT, interrupt_handler);
#endif

  /* Do an initial pass through the command-line argument to locate
  ** the name of the database file, the name of the initialization file,
  ** and the first command to execute.
  */
  for(i=1; i<argc-1; i++){
    char *z;
    if( argv[i][0]!='-' ) break;
    z = argv[i];
    if( z[0]=='-' && z[1]=='-' ) z++;
    if( strcmp(argv[i],"-separator")==0 || strcmp(argv[i],"-nullvalue")==0 ){
      i++;
    }else if( strcmp(argv[i],"-init")==0 ){
      i++;
      zInitFile = argv[i];
    }
  }
  if( i<argc ){
    data.zDbFilename = argv[i++];
  }else{
#ifndef SQLITE_OMIT_MEMORYDB
    data.zDbFilename = ":memory:";
#else
    data.zDbFilename = 0;
#endif
  }
  if( i<argc ){
    zFirstCmd = argv[i++];
  }
  data.out = stdout;

#ifdef SQLITE_OMIT_MEMORYDB
  if( data.zDbFilename==0 ){
    fprintf(stderr,"%s: no database filename specified\n", argv[0]);
    exit(1);
  }
#endif

  /* Go ahead and open the database file if it already exists.  If the
  ** file does not exist, delay opening it.  This prevents empty database
  ** files from being created if a user mistypes the database name argument
  ** to the sqlite command-line tool.
  */
  if( access(data.zDbFilename, 0)==0 ){
    open_db(&data);
  }

  /* Process the initialization file if there is one.  If no -init option
  ** is given on the command line, look for a file named ~/.sqliterc and
  ** try to process it.
  */
  process_sqliterc(&data,zInitFile);

  /* Make a second pass through the command-line argument and set
  ** options.  This second pass is delayed until after the initialization
  ** file is processed so that the command-line arguments will override
  ** settings in the initialization file.
  */
  for(i=1; i<argc && argv[i][0]=='-'; i++){
    char *z = argv[i];
    if( z[1]=='-' ){ z++; }
    if( strcmp(z,"-init")==0 ){
      i++;
    }else if( strcmp(z,"-html")==0 ){
      data.mode = MODE_Html;
    }else if( strcmp(z,"-list")==0 ){
      data.mode = MODE_List;
    }else if( strcmp(z,"-line")==0 ){
      data.mode = MODE_Line;
    }else if( strcmp(z,"-column")==0 ){
      data.mode = MODE_Column;
    }else if( strcmp(z,"-csv")==0 ){
      data.mode = MODE_Csv;
      strcpy(data.separator,",");
    }else if( strcmp(z,"-separator")==0 ){
      i++;
      sprintf(data.separator,"%.*s",(int)sizeof(data.separator)-1,argv[i]);
    }else if( strcmp(z,"-nullvalue")==0 ){
      i++;
      sprintf(data.nullvalue,"%.*s",(int)sizeof(data.nullvalue)-1,argv[i]);
    }else if( strcmp(z,"-header")==0 ){
      data.showHeader = 1;
    }else if( strcmp(z,"-noheader")==0 ){
      data.showHeader = 0;
    }else if( strcmp(z,"-echo")==0 ){
      data.echoOn = 1;
    }else if( strcmp(z,"-bail")==0 ){
      bail_on_error = 1;
    }else if( strcmp(z,"-version")==0 ){
      printf("%s\n", sqlite3_libversion());
      return 0;
    }else if( strcmp(z,"-interactive")==0 ){
      stdin_is_interactive = 1;
    }else if( strcmp(z,"-batch")==0 ){
      stdin_is_interactive = 0;
    }else if( strcmp(z,"-help")==0 || strcmp(z, "--help")==0 ){
      usage(1);
    }else{
      fprintf(stderr,"%s: unknown option: %s\n", Argv0, z);
      fprintf(stderr,"Use -help for a list of options.\n");
      return 1;
    }
  }

  if( zFirstCmd ){
    /* Run just the command that follows the database name
    */
    if( zFirstCmd[0]=='.' ){
      do_meta_command(zFirstCmd, &data);
      exit(0);
    }else{
      int rc;
      open_db(&data);
      rc = sqlite3_exec(data.db, zFirstCmd, callback, &data, &zErrMsg);
      if( rc!=0 && zErrMsg!=0 ){
        fprintf(stderr,"SQL error: %s\n", zErrMsg);
        exit(1);
      }
    }
  }else{
    /* Run commands received from standard input
    */
    if( stdin_is_interactive ){
      char *zHome;
      char *zHistory = 0;
      printf(
        "SQLite version %s\n"
        "Enter \".help\" for instructions\n",
        sqlite3_libversion()
      );
      zHome = find_home_dir();
      if( zHome && (zHistory = malloc(strlen(zHome)+20))!=0 ){
        sprintf(zHistory,"%s/.sqlite_history", zHome);
      }
#if defined(HAVE_READLINE) && HAVE_READLINE==1
      if( zHistory ) read_history(zHistory);
#endif
      rc = process_input(&data, 0);
      if( zHistory ){
        stifle_history(100);
        write_history(zHistory);
        free(zHistory);
      }
      free(zHome);
    }else{
      rc = process_input(&data, stdin);
    }
  }
  set_table_name(&data, 0);
  if( db ){
    if( sqlite3_close(db)!=SQLITE_OK ){
      fprintf(stderr,"error closing database: %s\n", sqlite3_errmsg(db));
    }
  }
  return rc;
}
