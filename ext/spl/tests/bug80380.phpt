--TEST--
Bug #80380 (Cannot access numeric property in ArrayObject)
--FILE--
<?php
$object = new stdClass;
$object->prop = 'test1';
$object->{'1'} = 'test2';

$arrayObject = new ArrayObject($object);
var_dump($arrayObject['prop']);
var_dump($arrayObject['1']);

$arrayObject = new ArrayObject($object, ArrayObject::ARRAY_AS_PROPS);
var_dump($arrayObject->prop);
var_dump($arrayObject->{'1'});
?>
--EXPECT--
string(5) "test1"
string(5) "test2"
string(5) "test1"
string(5) "test2"
