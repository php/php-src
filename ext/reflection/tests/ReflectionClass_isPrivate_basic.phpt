--TEST--
ReflectionClass::isPrivate() method
--FILE--
<?php

class TestImplicitClass {}
public class TestPublicClass {}
private class TestPrivateClass {}

$implicitClass = new ReflectionClass('TestImplicitClass');
$explicitClass = new ReflectionClass('TestPublicClass');
$privateClass = new ReflectionClass('TestPrivateClass');

var_dump($implicitClass->isPrivate());
var_dump($explicitClass->isPrivate());
var_dump($privateClass->isPrivate());

?>
--EXPECT--
bool(false)
bool(false)
bool(true)
