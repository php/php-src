--TEST--
DB_Error/DB_Warning test
--SKIPIF--
<?php if (!include("DB.php")) print "skip"; ?>
--FILE--
<?php // -*- C++ -*-

// Test for: DB.php
// Parts tested: DB_Error, DB_Warning

require_once "DB.php";

/*
print "testing DB::parseDSN...\n";
var_dump(DB::parseDSN("mysql"));
var_dump(DB::parseDSN("odbc(mssql)"));
var_dump(DB::parseDSN("mysql://localhost"));
var_dump(DB::parseDSN("mysql://remote.host.com/db"));
var_dump(DB::parseDSN("mysql://testuser:testpw"));
var_dump(DB::parseDSN("oci8://user:pass@tns-name"));
var_dump(DB::parseDSN(""));
var_dump(DB::parseDSN(""));
var_dump(DB::parseDSN(""));
var_dump(DB::parseDSN(""));
var_dump(DB::parseDSN(""));
var_dump(DB::parseDSN(""));
var_dump(DB::parseDSN(""));
*/


print "testing different error codes...\n";
$e = new DB_Error(); var_dump($e->toString());
$e = new DB_Error("test error"); var_dump($e->toString());
$e = new DB_Error(DB_OK); var_dump($e->toString());
$e = new DB_Error(DB_ERROR); var_dump($e->toString());
$e = new DB_Error(DB_ERROR_SYNTAX); var_dump($e->toString());
$e = new DB_Error(DB_ERROR_DIVZERO); var_dump($e->toString());
$e = new DB_Warning(); var_dump($e->toString());
$e = new DB_Warning("test warning"); var_dump($e->toString());
$e = new DB_Warning(DB_WARNING_READ_ONLY); var_dump($e->toString());

ini_alter("html_errors", false);

print "testing different error modes...\n";
$e = new DB_Error(DB_ERROR, PEAR_ERROR_PRINT); var_dump($e->toString());
$e = new DB_Error(DB_ERROR_SYNTAX, PEAR_ERROR_TRIGGER);

print "testing different error serverities...\n";
$e = new DB_Error(DB_ERROR_SYNTAX, PEAR_ERROR_TRIGGER, E_USER_NOTICE);
$e = new DB_Error(DB_ERROR_SYNTAX, PEAR_ERROR_TRIGGER, E_USER_WARNING);
$e = new DB_Error(DB_ERROR_SYNTAX, PEAR_ERROR_TRIGGER, E_USER_ERROR);

?>
--GET--
--POST--
--EXPECT--
testing different error codes...
string(109) "[db_error: message="DB Error: unknown error" code=-1 mode=return level=notice prefix="" prepend="" append=""]"
string(105) "[db_error: message="DB Error: test error" code=0 mode=return level=notice prefix="" prepend="" append=""]"
string(103) "[db_error: message="DB Error: no error" code=0 mode=return level=notice prefix="" prepend="" append=""]"
string(109) "[db_error: message="DB Error: unknown error" code=-1 mode=return level=notice prefix="" prepend="" append=""]"
string(108) "[db_error: message="DB Error: syntax error" code=-2 mode=return level=notice prefix="" prepend="" append=""]"
string(113) "[db_error: message="DB Error: division by zero" code=-13 mode=return level=notice prefix="" prepend="" append=""]"
string(118) "[db_warning: message="DB Warning: unknown warning" code=-1000 mode=return level=notice prefix="" prepend="" append=""]"
string(111) "[db_warning: message="DB Warning: test warning" code=0 mode=return level=notice prefix="" prepend="" append=""]"
string(112) "[db_warning: message="DB Warning: read only" code=-1001 mode=return level=notice prefix="" prepend="" append=""]"
testing different error modes...
DB Error: unknown errorstring(108) "[db_error: message="DB Error: unknown error" code=-1 mode=print level=notice prefix="" prepend="" append=""]"
<br>
<b>Notice</b>:  DB Error: syntax error in <b>PEAR.php</b> on line <b>182</b><br>
testing different error serverities...
<br>
<b>Notice</b>:  DB Error: syntax error in <b>PEAR.php</b> on line <b>182</b><br>
<br>
<b>Warning</b>:  DB Error: syntax error in <b>PEAR.php</b> on line <b>182</b><br>
<br>
<b>Fatal error</b>:  DB Error: syntax error in <b>PEAR.php</b> on line <b>182</b><br>
