--TEST--
Bug #66481: Calls to session_name() segfault when session.name is null.
--INI--
session.name=
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

var_dump(session_name("foo"));
var_dump(session_name("bar"));

--EXPECTF--
string(9) "PHPSESSID"
string(3) "foo"

