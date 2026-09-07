--TEST--
Test ReflectionProperty::isWritable() static
--FILE--
<?php

class A {
    public static $a;
    public static int $b;
    public static int $c = 42;
    protected static int $d = 42;
    private static int $e = 42;
    public private(set) static int $f = 42;
}

$r = new ReflectionProperty('A', 'a');
try {
    $r->isWritable(null, new A);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$rc = new ReflectionClass('A');
foreach ($rc->getProperties() as $rp) {
    echo $rp->getName() . ' from global: ';
    var_dump($rp->isWritable(null));
}

?>
--EXPECT--
ReflectionException: null is expected as object argument for static properties
a from global: bool(true)
b from global: bool(true)
c from global: bool(true)
d from global: bool(false)
e from global: bool(false)
f from global: bool(false)
