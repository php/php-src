--TEST--
ini_set() function
--INI--
arg_separator.output=&
--FILE--
<?php
var_dump(ini_set("arg_separator.output", ""));
var_dump(ini_get("arg_separator.output"));
?>
--EXPECT--
bool(false)
string(1) "&"
