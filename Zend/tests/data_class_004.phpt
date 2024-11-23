--TEST--
Reflection should work
--FILE--
<?php

class Foo {
    public function bar() {}
}

data class Bar {
    public function baz() {}
}

$foo = new ReflectionClass(Foo::class);
$bar = new ReflectionClass(Bar::class);
var_dump($foo->isDataClass());
var_dump($bar->isDataClass());
?>
--EXPECT--
bool(false)
bool(true)
