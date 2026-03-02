--TEST--
ReflectionFunction::isAnonymous
--FILE--
<?php
$rf = new ReflectionFunction(function() {});
var_dump($rf->isAnonymous());

$rf = new ReflectionFunction('strlen');
var_dump($rf->isAnonymous());

$rf = new ReflectionFunction(strlen(...));
var_dump($rf->isAnonymous());
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
