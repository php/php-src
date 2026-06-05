--TEST--
Class name with leading backslash uses correct case
--FILE--
<?php
class Foo {}

$o = new Foo();

// Correct case with leading backslash
var_dump($o instanceof \Foo);
var_dump(is_a($o, '\Foo'));
var_dump(class_exists('\Foo'));

// Wrong case with leading backslash - case-sensitive, returns false
var_dump(is_a($o, '\FOO'));
var_dump(class_exists('\FOO'));

// Dynamic new with leading backslash and wrong case - error
$cls = '\FOO';
try {
    $obj = new $cls();
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
Class "\FOO" not found
