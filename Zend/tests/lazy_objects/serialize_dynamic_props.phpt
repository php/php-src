--TEST--
Lazy objects: serialize() with dynamic props on initialized object
--FILE--
<?php

#[AllowDynamicProperties]
class C {
    public int $a;
}

$reflector = new ReflectionClass(C::class);

function test(string $name, object $obj) {
    printf("# %s\n", $name);

    var_dump(unserialize(serialize($obj)));
}

$obj = $reflector->newLazyGhost(function ($obj) {
    $obj->dyn = 1;
});

test('Ghost', $obj);

$obj = $reflector->newLazyProxy(function () {
    $c = new C();
    $c->dyn = 1;
    return $c;
});

test('Proxy', $obj);

?>
--EXPECTF--
# Ghost
object(C)#%d (1) {
  ["a"]=>
  uninitialized(int)
  ["dyn"]=>
  int(1)
}
# Proxy
object(C)#%d (1) {
  ["a"]=>
  uninitialized(int)
  ["dyn"]=>
  int(1)
}
