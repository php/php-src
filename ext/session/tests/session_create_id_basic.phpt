--TEST--
Test session_create_id() function : basic functionality
--INI--
session.save_handler=files
session.sid_length=32
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/*
 * Prototype : string session_create_id([string $prefix])
 * Description : Create new session ID with prefix optionally.
 * Source code : ext/session/session.c
 */

echo "*** Testing session_create_id() : basic functionality ***\n";

// No session
var_dump(session_create_id());
var_dump(session_create_id('ABCD'));

ini_set('session.use_strict_mode', true);
$sid = session_create_id('XYZ');
var_dump($sid);
var_dump(session_id($sid));
session_start();
var_dump(session_id());
var_dump(session_id() === $sid);
session_destroy();

ini_set('session.use_strict_mode', false);
$sid = session_create_id('XYZ');
var_dump($sid);
var_dump(session_id($sid));
session_start();
var_dump(session_id());
var_dump(session_id() === $sid);
session_destroy();

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_create_id() : basic functionality ***
string(32) "%s"
string(36) "ABCD%s"
string(35) "XYZ%s"
string(0) ""
string(32) "%s"
bool(false)
string(35) "XYZ%s"
string(0) ""
string(35) "XYZ%s"
bool(true)
Done
