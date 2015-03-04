--TEST--
ReflectionClass::isPublic() method
--FILE--
<?php

class TestImplicitClass {}
public class TestPublicClass {}
private class TestPrivateClass {}

$implicitClass = new ReflectionClass('TestImplicitClass');
$explicitClass = new ReflectionClass('TestPublicClass');
$privateClass = new ReflectionClass('TestPrivateClass');

var_dump($implicitClass->isPublic());
var_dump($explicitClass->isPublic());
var_dump($privateClass->isPublic());

?>
--EXPECT--
bool(true)
bool(true)
bool(false)
