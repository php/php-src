--TEST--
GH-20279 (register_argc_argv set to off after deprecation.)
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
var_dump(ini_get("register_argc_argv"));
?>
--EXPECT--
string(1) "1"
