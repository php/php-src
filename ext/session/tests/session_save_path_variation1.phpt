--TEST--
Test session_save_path() function : variation
--INI--
session.gc_probability=0
session.save_path=
session.name=PHPSESSID
session.save_handler=files
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_save_path() : variation ***\n";

$directory = __DIR__;
var_dump(session_save_path());
var_dump(session_save_path($directory));
var_dump(session_save_path());

var_dump(session_start());
var_dump(session_save_path());
var_dump(session_save_path($directory));
var_dump(session_save_path());
var_dump(session_destroy());

var_dump(session_save_path());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_save_path() : variation ***
string(0) ""
string(0) ""
string(%d) "%stests"
bool(true)
string(%d) "%stests"

Warning: session_save_path(): Session save path cannot be changed when a session is active in %s on line %d
bool(false)
string(%d) "%stests"
bool(true)
string(%d) "%stests"
Done
