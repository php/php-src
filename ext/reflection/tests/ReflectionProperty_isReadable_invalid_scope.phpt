--TEST--
Test ReflectionProperty::isReadable() invalid scope
--FILE--
<?php

class A {
    friend C;

    public $prop;
}

$r = new ReflectionProperty('A', 'prop');

try {
    $r->isReadable('B', null);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $r->isReadable('C', null);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Class "B" not found
Error: Class "C" not found
