--TEST--
Reflection for readonly property with default value
--FILE--
<?php

class Foo {
    public readonly int $prop = 1;
    public readonly ?string $nullable = null;
}

$rp = new ReflectionProperty(Foo::class, 'prop');
var_dump($rp->isReadOnly());
var_dump($rp->hasDefaultValue());
var_dump($rp->getDefaultValue());
var_dump(new ReflectionClass(Foo::class)->getDefaultProperties());

$test = new Foo();
try {
    $rp->setValue($test, 2);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
bool(true)
bool(true)
int(1)
array(2) {
  ["prop"]=>
  int(1)
  ["nullable"]=>
  NULL
}
Error: Cannot modify readonly property Foo::$prop
