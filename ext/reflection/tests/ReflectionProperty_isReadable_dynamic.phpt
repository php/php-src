--TEST--
Test ReflectionProperty::isReadable() dynamic
--FILE--
<?php

#[AllowDynamicProperties]
class A {}

$a = new A;

$a->a = 'a';
$r = new ReflectionProperty($a, 'a');

var_dump($r->isReadable(null, $a));
unset($a->a);
var_dump($r->isReadable(null, $a));

$a = new A;
var_dump($r->isReadable(null, $a));

var_dump($r->isReadable(null, null));

?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(false)
