--TEST--
oss-fuzz #71446
--FILE--
<?php

class C {
    public mixed $a;
    function __sleep() {
        return['a'];
    }
}

$reflector = new ReflectionClass(C::class);

$obj = $reflector->newLazyProxy(function() {
    $c = new C;
    return $c;
});

serialize($obj);
?>
--EXPECTF--
Deprecated: The __sleep() serialization magic method has been deprecated. Implement __serialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d
