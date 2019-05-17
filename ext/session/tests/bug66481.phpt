--TEST--
Bug #66481: Calls to session_name() segfault when session.name is null.
--INI--
session.name=
error_log=
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
ob_start();

var_dump(session_name("foo"));
var_dump(session_name("bar"));
--EXPECT--
PHP Warning:  PHP Startup: session.name cannot be a numeric or empty '' in Unknown on line 0

Warning: PHP Startup: session.name cannot be a numeric or empty '' in Unknown on line 0
string(9) "PHPSESSID"
string(3) "foo"
