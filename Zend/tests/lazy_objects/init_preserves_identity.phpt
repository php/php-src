--TEST--
Lazy objects: initialization of proxy does not change object id
--FILE--
<?php

class MyObject {
    public $a;
}

$reflector = new ReflectionClass(MyObject::class);

$object = new MyObject();
$objectId = spl_object_id($object);

$reflector->resetAsLazyProxy($object, function (MyObject $object) use (&$object2Id) {
    $object2 = new MyObject();
    $object2Id = spl_object_id($object2);
    return $object2;
});
var_dump(spl_object_id($object) === $objectId);
$reflector->initializeLazyObject($object);
var_dump(spl_object_id($object) === $objectId);
var_dump(spl_object_id($object) !== $object2Id);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
