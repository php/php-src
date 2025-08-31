--TEST--
Test session_name() function : error functionality
--INI--
session.save_path=
session.name=PHPSESSID
session.save_handler=files
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_name() : error functionality ***\n";

var_dump(session_name());
var_dump(session_name("blah"));
var_dump(session_start());
var_dump(session_name());
var_dump(session_destroy());
var_dump(session_name());

echo "Done";
ob_end_flush();
?>
--EXPECT--
*** Testing session_name() : error functionality ***
string(9) "PHPSESSID"
string(9) "PHPSESSID"
bool(true)
string(4) "blah"
bool(true)
string(4) "blah"
Done
