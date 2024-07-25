--TEST--
Lazy objects: Class constants are updated before initialization: update constant failure
--FILE--
<?php

class C {
    public C $a = FOO;
}

$reflector = new ReflectionClass(C::class);
$c = $reflector->newLazyGhost(function () { });

function f() {
    define('FOO', new stdClass);
}

f();

try {
    var_dump($c->a);
} catch (\Error $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

--EXPECT--
TypeError: Cannot assign stdClass to property C::$a of type C
