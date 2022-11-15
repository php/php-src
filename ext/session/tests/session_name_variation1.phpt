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

var_dump(session_name("\0"));
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
string(9) "PHPSESSID"
bool(true)
string(9) "PHPSESSID"
bool(true)
string(9) "PHPSESSID"
string(9) "PHPSESSID"

Warning: session_start(): session.name cannot contain any of the following '=,;.[ \t\r\n\013\014' in %s on line %d
bool(true)
string(1) "	"
bool(true)
string(1) "	"

Warning: session_name(): session.name "" cannot be numeric or empty in %s on line %d
string(1) "	"

Warning: session_start(): session.name cannot contain any of the following '=,;.[ \t\r\n\013\014' in %s on line %d
bool(true)
string(1) "	"
bool(true)
string(1) "	"
Done
