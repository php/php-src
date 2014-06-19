--TEST--
Bug #66481: Calls to session_name() segfault when session.name is null.
--INI--
session.name=
--SKIPIF--
<?php include('skipif.inc'); ?>
<?php if(substr(PHP_OS, 0, 3) == "WIN") die("skip Not for Windows"); ?>
--FILE--
<?php

var_dump(session_name("foo"));
var_dump(session_name("bar"));
--EXPECTF--
PHP Warning:  PHP Startup: session.name cannot be a numeric or empty '' in Unknown on line 0

Warning: PHP Startup: session.name cannot be a numeric or empty '' in Unknown on line 0
string(9) "PHPSESSID"
string(3) "foo"
