--TEST--
Lazy objects: clone is independent of the original object
--FILE--
<?php

class SomeObj {
    public Value $value;
    public function __construct() {
        $this->value = new Value();
    }
    public function __clone() {
        $this->value = clone $this->value;
    }
}

class Value {
    public string $value = 'A';
}

function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    $reflector = new ReflectionClass(SomeObj::class);

    $clonedObj = clone $obj;
    var_dump($clonedObj->value->value);

    $reflector->initializeLazyObject($obj);
    $obj->value->value = 'B';

    $reflector->initializeLazyObject($clonedObj);

    var_dump($obj->value->value);
    var_dump($clonedObj->value->value);
}

$reflector = new ReflectionClass(SomeObj::class);

test('Ghost', $reflector->newLazyGhost(function ($obj) {
    $obj->__construct();
}));

test('Proxy', $reflector->newLazyProxy(function () {
    return new SomeObj();
}));

?>
--EXPECT--
# Ghost:
string(1) "A"
string(1) "B"
string(1) "A"
# Proxy:
string(1) "A"
string(1) "B"
string(1) "A"
