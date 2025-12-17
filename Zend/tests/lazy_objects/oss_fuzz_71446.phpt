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
