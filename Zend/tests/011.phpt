--TEST--
property_exists() tests
--FILE--
<?php

class foo {
    public $pp1 = 1;
    private $pp2 = 2;
    protected $pp3 = 3;

    function bar() {
        var_dump(property_exists("foo","pp1"));
        var_dump(property_exists("foo","pp2"));
        var_dump(property_exists("foo","pp3"));
    }
}

class bar extends foo {
    function test() {
        var_dump(property_exists("foo","pp1"));
        var_dump(property_exists("foo","pp2"));
        var_dump(property_exists("foo","pp3"));
    }
}

var_dump(property_exists("foo","pp1"));
var_dump(property_exists("foo","pp2"));
var_dump(property_exists("foo","pp3"));
var_dump(property_exists("foo","nonexistent"));
var_dump(property_exists("fo","nonexistent"));
var_dump(property_exists("foo",""));
var_dump(property_exists("","test"));
var_dump(property_exists("",""));

$foo = new foo;

var_dump(property_exists($foo,"pp1"));
var_dump(property_exists($foo,"pp2"));
var_dump(property_exists($foo,"pp3"));
var_dump(property_exists($foo,"nonexistent"));
var_dump(property_exists($foo,""));

try {
    var_dump(property_exists(array(), "test"));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(property_exists(1, "test"));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(property_exists(3.14, "test"));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(property_exists(true, "test"));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(property_exists(null, "test"));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$foo->bar();

$bar = new bar;
$bar->test();

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
property_exists(): Argument #1 ($object_or_class) must be of type object|string, array given
property_exists(): Argument #1 ($object_or_class) must be of type object|string, int given
property_exists(): Argument #1 ($object_or_class) must be of type object|string, float given
property_exists(): Argument #1 ($object_or_class) must be of type object|string, bool given
property_exists(): Argument #1 ($object_or_class) must be of type object|string, null given
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
