--TEST--
Lazy objects: clone is independent of the original object
--FILE--
<?php

class SomeObj {
    public string $foo = 'A';
    public string $dummy;
}

$reflector = new ReflectionClass(SomeObj::class);

$predefinedObject = new SomeObj();
$initializer = function () use ($predefinedObject) {
    return $predefinedObject;
};

$myProxy = $reflector->newLazyProxy($initializer);
$reflector->getProperty('foo')->skipLazyInitialization($myProxy);

$clonedProxy = clone $myProxy;
var_dump($clonedProxy->foo);

$reflector->initializeLazyObject($myProxy);
$myProxy->foo = 'B';

$reflector->initializeLazyObject($clonedProxy);

var_dump($myProxy->foo);
var_dump($clonedProxy->foo);

--EXPECT--
string(1) "A"
string(1) "B"
string(1) "A"
