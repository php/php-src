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
var_dump($r->isWritable(null, null));

$a->b = 'b';
$r = new ReflectionProperty($a, 'b');
var_dump($r->isWritable(null, $a));

$a = new A;
var_dump($r->isWritable(null, $a));

var_dump($r->isWritable(null, null));

?>
--EXPECT--
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
