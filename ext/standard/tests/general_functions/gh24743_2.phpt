--TEST--
GH-24743 (var_export() does not handle exception correctly when retrieving object): lazy object initializer
--FILE--
<?php

class C {
    public int $a = 1;
}

class O {
    public C $c;
    public int $b = 2;
}

$o = new O;
$o->c = (new ReflectionClass(C::class))->newLazyGhost(function ($obj) {
    throw new Exception("init failed");
});

try {
    var_export($o);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_export($o->c);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
init failed
init failed
