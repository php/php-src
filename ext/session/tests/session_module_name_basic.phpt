--TEST--
Test session_module_name() function : basic functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_module_name() : basic functionality ***\n";
var_dump(session_module_name("files"));
var_dump(session_module_name());
var_dump(session_start());
var_dump(session_module_name());
var_dump(session_destroy());
var_dump(session_module_name());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_module_name() : basic functionality ***
string(%d) "%s"
string(5) "files"
bool(true)
string(5) "files"
bool(true)
string(5) "files"
Done
