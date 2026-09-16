--TEST--
GH-16615 001 (Assertion failure in zend_std_read_property)
--FILE--
<?php

class Foo {
    public string $bar {
        set => $value;
    }
}

$reflector = new ReflectionClass(Foo::class);

// Adds IS_PROP_LAZY to prop flags
$foo = $reflector->newLazyGhost(function ($ghost) {
    $ghost->bar = 'bar';
});

echo $foo->bar;

?>
--EXPECT--
bar
