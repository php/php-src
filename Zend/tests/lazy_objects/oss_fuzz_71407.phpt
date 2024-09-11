--TEST--
oss-fuzz #71407
--FILE--
<?php

class C {
    public $ÿ=ÿ;
}

$reflector = new ReflectionClass(C::class);

try {
    $obj = $reflector->newLazyGhost(function() {});
    clone $obj;
} catch (Error $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}


?>
--EXPECT--
Error: Undefined constant "ÿ"
