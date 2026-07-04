--TEST--
Class name with incorrect case fails in new operator
--FILE--
<?php
class Foo {}

$o = new Foo(); // correct case
var_dump($o instanceof Foo);

try {
    $o = new FOO(); // wrong case
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $o = new foo(); // wrong case
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
bool(true)
Class "FOO" not found
Class "foo" not found
