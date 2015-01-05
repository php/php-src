--TEST--
ReflectionObject:getProperties() issues invalid reads when it get_properties returns a hash table with (inaccessible) dynamic numeric properties
--FILE--
<?php
$x = new ArrayObject();
$x[0] = 'test string 2';
$x['test'] = 'test string 3';
$reflObj = new ReflectionObject($x);
print_r($reflObj->getProperties(ReflectionProperty::IS_PUBLIC));

$x = (object)array("a", "oo" => "b");
$reflObj = new ReflectionObject($x);
print_r($reflObj->getProperties(ReflectionProperty::IS_PUBLIC));
--EXPECT--
Array
(
    [0] => ReflectionProperty Object
        (
            [name] => test
            [class] => ArrayObject
        )

)
Array
(
    [0] => ReflectionProperty Object
        (
            [name] => oo
            [class] => stdClass
        )

)
