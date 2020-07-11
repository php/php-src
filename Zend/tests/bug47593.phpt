--TEST--
Bug #47593 (interface_exists() returns false when using absolute namespace path)
--FILE--
<?php

namespace test;
const TEST = 11;

class foo {
    public function xyz() {
    }
}

interface baz {
}

function bar() {
}


var_dump(interface_exists('\test\baz'));
var_dump(function_exists('\test\bar'));
var_dump(constant('\test\TEST'));
var_dump(defined('\test\TEST'));
var_dump(defined('TEST'));


?>
--EXPECT--
bool(true)
bool(true)
int(11)
bool(true)
bool(false)
