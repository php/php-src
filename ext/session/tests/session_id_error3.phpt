--TEST--
Test session_id() function : error functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/*
 * Prototype : string session_id([string $id])
 * Description : Get and/or set the current session id
 * Source code : ext/session/session.c
 */

echo "*** Testing session_id() : error functionality ***\n";

var_dump(session_id());
var_dump(session_id("!"));
var_dump(session_id());
@session_start();
@session_destroy();

var_dump(session_id());
var_dump(session_id("?><"));
var_dump(session_id());
@session_start();
@session_destroy();

var_dump(session_id());
var_dump(session_id("\xa3$%^&*()"));
var_dump(session_id());
@session_start();
@session_destroy();

var_dump(session_id());
var_dump(session_id("\r\n"));
var_dump(session_id());
@session_start();
@session_destroy();

var_dump(session_id());
var_dump(session_id("\0"));
var_dump(session_id());
@session_start();
@session_destroy();

var_dump(session_id());
var_dump(session_id("\xac``@~:{>?><,./[]+--"));
var_dump(session_id());
@session_start();
@session_destroy();

echo "Done";
ob_end_flush();
?>
--EXPECT--
*** Testing session_id() : error functionality ***
string(0) ""
string(0) ""
string(1) "!"
string(0) ""
string(0) ""
string(3) "?><"
string(0) ""
string(0) ""
string(8) "£$%^&*()"
string(0) ""
string(0) ""
string(2) "
"
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(19) "¬``@~:{>?><,./[]+--"
Done
