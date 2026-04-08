--TEST--
Bug #72300 (ignore_user_abort(false) has no effect)
--INI--
ignore_user_abort=0
--FILE--
<?php

var_dump(ignore_user_abort(true));
var_dump(ignore_user_abort());
var_dump(ini_get("ignore_user_abort"));
var_dump(ignore_user_abort(false));
var_dump(ignore_user_abort());
var_dump(ini_get("ignore_user_abort"));

?>
--EXPECT--
bool(false)
bool(true)
string(1) "1"
bool(true)
bool(false)
string(1) "0"
