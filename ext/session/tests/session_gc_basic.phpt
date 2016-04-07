--TEST--
Test session_gc() function : basic functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/* 
 * Prototype : int session_gc(void)
 * Description : Perform GC
 * Source code : ext/session/session.c
 */

echo "*** Testing session_gc() : basic functionality ***\n";

// Turn off strice mode, since it does not allow uninitialized session ID
ini_set('session.use_strict_mode',false);

var_dump(session_gc());

var_dump(session_start());
var_dump(session_gc(), session_gc() >= -1);
var_dump(session_destroy());
var_dump(session_id());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_gc() : basic functionality ***

Warning: session_gc(): Session is not active in %s on line 16
bool(false)
bool(true)
int(%d)
bool(true)
bool(true)
string(0) ""
Done


