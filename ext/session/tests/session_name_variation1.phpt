--TEST--
Test session_name() function : variation
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

echo "*** Testing session_name() : variation ***\n";

var_dump(session_name("15"));
var_dump(session_start());
var_dump(session_name());
var_dump(session_destroy());
var_dump(session_name());

var_dump(session_name("10.25"));
var_dump(session_start());
var_dump(session_name());
var_dump(session_destroy());
var_dump(session_name());

var_dump(session_name("\t"));
var_dump(session_start());
var_dump(session_name());
var_dump(session_destroy());
var_dump(session_name());

var_dump(session_name(""));
var_dump(session_start());
var_dump(session_name());
var_dump(session_destroy());
var_dump(session_name());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_name() : variation ***

Warning: session_name(): session.name "15" must not be numeric in %s on line %d
string(9) "PHPSESSID"
bool(true)
string(9) "PHPSESSID"
bool(true)
string(9) "PHPSESSID"

Warning: session_name(): session.name "10.25" must not be numeric in %s on line %d
string(9) "PHPSESSID"
bool(true)
string(9) "PHPSESSID"
bool(true)
string(9) "PHPSESSID"

Warning: session_name(): session.name "	" must not contain any of the following '=,; \t\r\n\v\f' in %s on line %d
string(9) "PHPSESSID"
bool(true)
string(9) "PHPSESSID"
bool(true)
string(9) "PHPSESSID"

Warning: session_name(): session.name "" must not be empty in %s on line %d
string(9) "PHPSESSID"
bool(true)
string(9) "PHPSESSID"
bool(true)
string(9) "PHPSESSID"
Done
