--TEST--
instanceof with pseudo-type "object" is equivalent to is_object()
--FILE--
<?php

// Objects
$obj = new stdClass;
var_dump($obj instanceof object);

$anon = new class {};
var_dump($anon instanceof object);

// Case-insensitive
var_dump($obj instanceof Object);
var_dump($obj instanceof OBJECT);

// Non-objects (variables, so they go through ZEND_TYPE_CHECK, not the const early-return)
$null = null;
var_dump($null instanceof object);

$int = 42;
var_dump($int instanceof object);

$str = "string";
var_dump($str instanceof object);

$arr = [];
var_dump($arr instanceof object);

// Must be identical to is_object() for any value
$values = [new stdClass, new class {}, null, 42, "str", [], true, 1.5];
foreach ($values as $v) {
    if (($v instanceof object) !== is_object($v)) {
        echo "MISMATCH for: ";
        var_dump($v);
    }
}
echo "All match is_object()\n";

// Inside a class scope — class context must not affect the result
class Foo {}
$foo = new Foo;
var_dump($foo instanceof object);
var_dump($foo instanceof Foo);  // still works normally

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
All match is_object()
bool(true)
bool(true)
