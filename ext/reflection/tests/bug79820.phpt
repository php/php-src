--TEST--
Bug #79820: Use after free when type duplicated into ReflectionProperty gets resolved
--FILE--
<?php

class Test {
    public stdClass $prop;
}

$rp = new ReflectionProperty(Test::class, 'prop');
$test = new Test;
$test->prop = new stdClass;
var_dump($rp->getType()->getName());
$test->dynProp = 42;

$rp = new ReflectionProperty($test, 'dynProp');
var_dump($rp->getType());

?>
--EXPECT--
string(8) "stdClass"
NULL
