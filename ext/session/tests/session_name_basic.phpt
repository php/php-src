--TEST--
Test session_name() function : error functionality
--INI--
session.save_path=
session.name=PHPSESSID
session.save_handler=files
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/* 
 * Prototype : string session_name([string $name])
 * Description : Get and/or set the current session name
 * Source code : ext/session/session.c 
 */

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
--EXPECTF--
*** Testing session_name() : error functionality ***
string(9) "PHPSESSID"
string(9) "PHPSESSID"
bool(true)
string(4) "blah"
bool(true)
string(4) "blah"
Done

