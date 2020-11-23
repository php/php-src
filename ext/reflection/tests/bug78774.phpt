--TEST--
Bug #78774: ReflectionNamedType on Typed Properties Crash
--FILE--
<?php

class Test {
    public stdClass $prop;
    public stdClass|Foo $prop2;
}

$rc = new ReflectionClass(Test::class);
$rp = $rc->getProperty('prop');
$rt = $rp->getType();
$rp2 = $rc->getProperty('prop2');
$rt2 = $rp2->getType();

// Force a resolution of the property type
$test = new Test;
$test->prop = new stdClass;
$test->prop2 = new stdClass;

var_dump($rt->getName());
var_dump((string) $rt2);

?>
--EXPECT--
string(8) "stdClass"
string(12) "stdClass|Foo"
