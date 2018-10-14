--TEST--
Test session_save_path() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.save_handler=files
session.gc_probability=0
--FILE--
<?php

ob_start();

/*
 * Prototype : string session_save_path([string $path])
 * Description : Get and/or set the current session save path
 * Source code : ext/session/session.c
 */

echo "*** Testing session_save_path() : variation ***\n";

ini_set("session.save_path", "/blah");
var_dump(session_save_path());
var_dump(session_start());
var_dump(session_save_path());
var_dump(session_destroy());
var_dump(session_save_path());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_save_path() : variation ***
string(5) "/blah"

Warning: session_start(): open(%s, O_RDWR) failed: No such file or directory (2) in %s on line %d

Warning: session_start(): Failed to read session data: files (path: %sblah) in %s on line %d
bool(false)
string(5) "/blah"

Warning: session_destroy(): Trying to destroy uninitialized session in %s on line %d
bool(false)
string(5) "/blah"
Done
