--TEST--
Test ReflectionProperty::isWritable() dynamic
--FILE--
<?php

#[AllowDynamicProperties]
class A {
    private $a;
}

$a = new A;

$r = new ReflectionProperty($a, 'a');
var_dump($r->isWritable(null, $a));

$a->b = 'b';
$r = new ReflectionProperty($a, 'b');
var_dump($r->isWritable(null, $a));

$a = new A;
var_dump($r->isWritable(null, $a));

?>
--EXPECT--
bool(false)
bool(true)
bool(true)
