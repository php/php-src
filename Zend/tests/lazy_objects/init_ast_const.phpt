--TEST--
Lazy objects: Class constants are updated before initialization
--FILE--
<?php

class C {
    public stdClass $a = FOO;
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

--EXPECTF--
object(stdClass)#%d (0) {
}
