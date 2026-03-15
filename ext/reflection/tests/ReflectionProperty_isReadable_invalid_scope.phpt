--TEST--
Test ReflectionProperty::isReadable() invalid scope
--FILE--
<?php

class A {
    public $prop;
}

$r = new ReflectionProperty('A', 'prop');

try {
    $r->isReadable('B', null);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Class "B" not found
