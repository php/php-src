--TEST--
Lazy objects: clone is independent of the original object
--FILE--
<?php

class SomeObj {
    public string $foo = 'X';
    public string $dummy;
}

function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    $reflector = new ReflectionClass(SomeObj::class);

    $clonedObj = clone $obj;

    $reflector->initializeLazyObject($obj);
    $reflector->getProperty('foo')->setRawValueWithoutLazyInitialization($clonedObj, 'Y');

    $reflector->initializeLazyObject($clonedObj);

    var_dump($clonedObj->foo);
}

$reflector = new ReflectionClass(SomeObj::class);

test('Ghost', $reflector->newLazyGhost(function ($obj) {
}));

test('Proxy', $reflector->newLazyProxy(function () {
    return new SomeObj();
}));

?>
--EXPECT--
# Ghost:
string(1) "Y"
# Proxy:
string(1) "Y"
