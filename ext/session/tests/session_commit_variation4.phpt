--TEST--
Test session_commit() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_strict_mode=0
--FILE--
<?php

ob_start();

/*
 * Prototype : bool session_commit(void)
 * Description : Write session data and end session
 * Source code : ext/session/session.c
 */

echo "*** Testing session_commit() : variation ***\n";

var_dump(ini_get('session.use_strict_mode'));
var_dump(session_id("test"));
var_dump(session_start());
var_dump(session_id());
var_dump(session_commit());
var_dump(session_id());
var_dump(session_start());
var_dump(ini_get('session.use_strict_mode'));
var_dump(session_id());
var_dump(session_commit());
var_dump(session_id());
var_dump(session_start());
var_dump(session_id());
var_dump(session_commit());
var_dump(session_id());
var_dump(session_start());
var_dump(session_destroy());
var_dump(ini_get('session.use_strict_mode'));

echo "Done";
ob_end_flush();
?>
--EXPECT--
*** Testing session_commit() : variation ***
string(1) "0"
string(0) ""
bool(true)
string(4) "test"
bool(true)
string(4) "test"
bool(true)
string(1) "0"
string(4) "test"
bool(true)
string(4) "test"
bool(true)
string(4) "test"
bool(true)
string(4) "test"
bool(true)
bool(true)
string(1) "0"
Done
