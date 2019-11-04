--TEST--
Bug #78774: ReflectionNamedType on Typed Properties Crash
--FILE--
<?php

class Test {
    public stdClass $prop;
}

$rc = new ReflectionClass(Test::class);
$rp = $rc->getProperty('prop');
$rt = $rp->getType();

// Force a resolution of the property type
$test = new Test;
$test->prop = new stdClass;

var_dump($rt->getName());

?>
--EXPECT--
string(8) "stdClass"
