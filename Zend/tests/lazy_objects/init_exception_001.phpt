--TEST--
Lazy objects: init exception 001
--FILE--
<?php

#[AllowDynamicProperties]
class C {
    public int $a;
    public $b;
}

$reflector = new ReflectionClass(C::class);

for ($i = 0; $i < 2; $i++) {
    $obj = $reflector->newLazyGhost(function ($obj) use ($i) {
        if ($i === 1) {
            throw new \Exception();
        }
    });
    $obj->c = 1;
}

?>
--EXPECTF--
Fatal error: Uncaught Exception in %s:%d
Stack trace:
#0 %s(%d): {closure:%s:%d}(Object(C))
#1 {main}
  thrown in %s on line %d
