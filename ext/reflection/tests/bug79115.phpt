--TEST--
Bug #79115: ReflectionClass::isCloneable call reflected class __destruct
--FILE--
<?php

class A {
    function __construct() { echo __FUNCTION__ . "\n"; }
    function __destruct() { echo __FUNCTION__ . "\n"; }
}

$c = new ReflectionClass('A');
var_dump($c->isCloneable());

?>
--EXPECT--
bool(true)
