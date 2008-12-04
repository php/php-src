--TEST--
Bug #43651 (is_callable() with one or more nonconsecutive colons crashes)
--FILE--
<?php
class Test {
    static function foo() {}
}

var_dump(is_callable("\\\\"));
var_dump(is_callable("\\"));
var_dump(is_callable("x\\"));
var_dump(is_callable("\\x"));
var_dump(is_callable("x\\x"));
var_dump(is_callable("x\\\\"));
var_dump(is_callable("\\x"));
var_dump(is_callable("x\\\\x"));
var_dump(is_callable("cd"));
var_dump(is_callable("Test\\"));
var_dump(is_callable("\\Test"));
var_dump(is_callable("\\Test\\"));
var_dump(is_callable("Test::foo"));
var_dump(is_callable("\\Test::foo"));
var_dump(is_callable("is_string"));
var_dump(is_callable("\\is_string"));
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
