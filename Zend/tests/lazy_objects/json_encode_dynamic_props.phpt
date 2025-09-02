--TEST--
Lazy objects: json_encode with dynamic props on initialized object
--FILE--
<?php

#[AllowDynamicProperties]
class C {
    public int $a = 1;
}

$reflector = new ReflectionClass(C::class);

$reflector = new ReflectionClass(C::class);

function test(string $name, object $obj) {
    printf("# %s\n", $name);

    var_dump(json_decode(json_encode($obj)));
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

--EXPECTF--
# Ghost
object(stdClass)#%d (2) {
  ["a"]=>
  int(1)
  ["dyn"]=>
  int(1)
}
# Proxy
object(stdClass)#%d (2) {
  ["a"]=>
  int(1)
  ["dyn"]=>
  int(1)
}
