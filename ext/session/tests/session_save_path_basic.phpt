--TEST--
Test session_save_path() function : basic functionality
--INI--
session.gc_probability=0
session.save_path=
session.name=PHPSESSID
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_save_path() : error functionality ***\n";

$directory = __DIR__;
var_dump(session_save_path());
var_dump(session_save_path($directory));
var_dump(session_save_path());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_save_path() : error functionality ***
string(0) ""
string(0) ""
string(%d) "%s"
Done
