--TEST--
Test ReflectionProperty::isWritable() invalid scope
--FILE--
<?php

class A {
    friend C;

    public $prop;
}

$r = new ReflectionProperty('A', 'prop');

try {
    $r->isWritable('B', null);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $r->isWritable('C', null);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Class "B" not found
Error: Class "C" not found
