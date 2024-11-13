--TEST--
Lazy objects: RFC example 007
--FILE--
<?php

class MyClass {
    public $a;
}

$object = new MyClass();
$ref = WeakReference::create($object);
$id = spl_object_id($object);

$reflector = new ReflectionClass(MyClass::class);

$reflector->resetAsLazyGhost($object, function () {});
var_dump($id === spl_object_id($object));
var_dump($ref->get() === $object);

$reflector->initializeLazyObject($object);
var_dump($id === spl_object_id($object));
var_dump($ref->get() === $object);

?>
==DONE==
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
==DONE==
