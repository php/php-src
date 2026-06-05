--TEST--
Class name with incorrect case fails in dynamic new operator
--FILE--
<?php
class Foo {}

$c = 'Foo';
$o = new $c(); // correct case
var_dump($o instanceof Foo);

$c = 'FOO';
try {
    $o = new $c(); // wrong case
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
bool(true)
Class "FOO" not found
