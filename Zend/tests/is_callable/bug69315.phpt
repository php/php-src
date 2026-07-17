--TEST--
Bug #69315 (disable_functions behaviors inconsistently)
--INI--
disable_functions=strlen,defined,call_user_func,constant,is_string
--FILE--
<?php

var_dump(function_exists("strlen"));
var_dump(is_callable("strlen"));
try {
    var_dump(strlen("xxx"));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(defined("PHP_VERSION"));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(constant("PHP_VERSION"));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(call_user_func("strlen"));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(is_string("xxx"));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(is_string());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
bool(false)
bool(false)
Call to undefined function strlen()
Call to undefined function defined()
Call to undefined function constant()
Call to undefined function call_user_func()
Call to undefined function is_string()
Call to undefined function is_string()
