--TEST--
ReflectionParameter::__toString() with new initializer
--FILE--
<?php

function test(
    $p1 = new stdClass,
    $p2 = new SomeClass(new With, some: new Parameters)
) {}

echo new ReflectionParameter('test', 'p1'), "\n";
echo new ReflectionParameter('test', 'p2'), "\n";

?>
--EXPECT--
Parameter #0 [ <optional> $p1 = new \stdClass() ]
Parameter #1 [ <optional> $p2 = new \SomeClass(new \With(), some: new \Parameters()) ]
