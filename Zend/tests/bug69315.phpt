--TEST--
Bug #69315 (disable_functions behaviors inconsistently)
--INI--
disable_functions=strlen,defined,call_user_func,constant,is_string
--FILE--
<?php

var_dump(function_exists("strlen"));
var_dump(is_callable("strlen"));
var_dump(strlen("xxx"));
var_dump(defined("PHP_VERSION"));
var_dump(constant("PHP_VERSION"));
var_dump(call_user_func("strlen"));
var_dump(is_string("xxx"));
--EXPECTF--
bool(false)
bool(true)

Warning: strlen() has been disabled for security reasons in %sbug69315.php on line %d
NULL

Warning: defined() has been disabled for security reasons in %sbug69315.php on line %d
NULL

Warning: constant() has been disabled for security reasons in %sbug69315.php on line %d
NULL

Warning: call_user_func() has been disabled for security reasons in %sbug69315.php on line %d
NULL

Warning: is_string() has been disabled for security reasons in %sbug69315.php on line %d
NULL
