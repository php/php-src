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
** Code for testing the utf.c module in SQLite.  This code
** is not included in the SQLite library.  It is used for automated
** testing of the SQLite library. Specifically, the code in this file
** is used for testing the SQLite routines for converting between
** the various supported unicode encodings.
**
** $Id$
*/
#include "sqliteInt.h"
#include "vdbeInt.h"
#include "os.h"         /* to get SQLITE_BIGENDIAN */
#include "tcl.h"
#include <stdlib.h>
#include <string.h>

/*
** The first argument is a TCL UTF-8 string. Return the byte array
** object with the encoded representation of the string, including
** the NULL terminator.
*/
static int binarize(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  int len;
  char *bytes;
  Tcl_Obj *pRet;
  assert(objc==2);

  bytes = Tcl_GetStringFromObj(objv[1], &len);
  pRet = Tcl_NewByteArrayObj((u8*)bytes, len+1);
  Tcl_SetObjResult(interp, pRet);
  return TCL_OK;
}

/*
** Usage: test_value_overhead <repeat-count> <do-calls>.
**
** This routine is used to test the overhead of calls to
** sqlite3_value_text(), on a value that contains a UTF-8 string. The idea
** is to figure out whether or not it is a problem to use sqlite3_value
** structures with collation sequence functions.
**
** If <do-calls> is 0, then the calls to sqlite3_value_text() are not
** actually made.
*/
static int test_value_overhead(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  int do_calls;
  int repeat_count;
  int i;
  Mem val;
  const char *zVal;

  if( objc!=3 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"",
        Tcl_GetStringFromObj(objv[0], 0), " <repeat-count> <do-calls>", 0);
    return TCL_ERROR;
  }

  if( Tcl_GetIntFromObj(interp, objv[1], &repeat_count) ) return TCL_ERROR;
  if( Tcl_GetIntFromObj(interp, objv[2], &do_calls) ) return TCL_ERROR;

  val.flags = MEM_Str|MEM_Term|MEM_Static;
  val.z = "hello world";
  val.type = SQLITE_TEXT;
  val.enc = SQLITE_UTF8;

  for(i=0; i<repeat_count; i++){
    if( do_calls ){
      zVal = (char*)sqlite3_value_text(&val);
    }
  }

  return TCL_OK;
}

static u8 name_to_enc(Tcl_Interp *interp, Tcl_Obj *pObj){
  struct EncName {
    char *zName;
    u8 enc;
  } encnames[] = {
    { "UTF8", SQLITE_UTF8 },
    { "UTF16LE", SQLITE_UTF16LE },
    { "UTF16BE", SQLITE_UTF16BE },
    { "UTF16", SQLITE_UTF16NATIVE },
    { 0, 0 }
  };
  struct EncName *pEnc;
  char *z = Tcl_GetString(pObj);
  for(pEnc=&encnames[0]; pEnc->zName; pEnc++){
    if( 0==sqlite3StrICmp(z, pEnc->zName) ){
      break;
    }
  }
  if( !pEnc->enc ){
    Tcl_AppendResult(interp, "No such encoding: ", z, 0);
  }
  return pEnc->enc;
}

/*
** Usage:   test_translate <string/blob> <from enc> <to enc> ?<transient>?
**
*/
static int test_translate(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  u8 enc_from;
  u8 enc_to;
  sqlite3_value *pVal;

  char *z;
  int len;
  void (*xDel)(void *p) = SQLITE_STATIC;

  if( objc!=4 && objc!=5 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"",
        Tcl_GetStringFromObj(objv[0], 0), 
        " <string/blob> <from enc> <to enc>", 0
    );
    return TCL_ERROR;
  }
  if( objc==5 ){
    xDel = sqlite3FreeX;
  }

  enc_from = name_to_enc(interp, objv[2]);
  if( !enc_from ) return TCL_ERROR;
  enc_to = name_to_enc(interp, objv[3]);
  if( !enc_to ) return TCL_ERROR;

  pVal = sqlite3ValueNew();

  if( enc_from==SQLITE_UTF8 ){
    z = Tcl_GetString(objv[1]);
    if( objc==5 ){
      z = sqliteStrDup(z);
    }
    sqlite3ValueSetStr(pVal, -1, z, enc_from, xDel);
  }else{
    z = (char*)Tcl_GetByteArrayFromObj(objv[1], &len);
    if( objc==5 ){
      char *zTmp = z;
      z = sqliteMalloc(len);
      memcpy(z, zTmp, len);
    }
    sqlite3ValueSetStr(pVal, -1, z, enc_from, xDel);
  }

  z = (char *)sqlite3ValueText(pVal, enc_to);
  len = sqlite3ValueBytes(pVal, enc_to) + (enc_to==SQLITE_UTF8?1:2);
  Tcl_SetObjResult(interp, Tcl_NewByteArrayObj((u8*)z, len));

  sqlite3ValueFree(pVal);

  return TCL_OK;
}

/*
** Usage: translate_selftest
**
** Call sqlite3utfSelfTest() to run the internal tests for unicode
** translation. If there is a problem an assert() will fail.
**/
void sqlite3utfSelfTest();
static int test_translate_selftest(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
#ifndef SQLITE_OMIT_UTF16
  sqlite3utfSelfTest();
#endif
  return SQLITE_OK;
}


/*
** Register commands with the TCL interpreter.
*/
int Sqlitetest5_Init(Tcl_Interp *interp){
  static struct {
    char *zName;
    Tcl_ObjCmdProc *xProc;
  } aCmd[] = {
    { "binarize",                (Tcl_ObjCmdProc*)binarize },
    { "test_value_overhead",     (Tcl_ObjCmdProc*)test_value_overhead },
    { "test_translate",          (Tcl_ObjCmdProc*)test_translate     },
    { "translate_selftest",      (Tcl_ObjCmdProc*)test_translate_selftest},
  };
  int i;
  for(i=0; i<sizeof(aCmd)/sizeof(aCmd[0]); i++){
    Tcl_CreateObjCommand(interp, aCmd[i].zName, aCmd[i].xProc, 0, 0);
  }
  return SQLITE_OK;
}
