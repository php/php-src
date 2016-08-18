--TEST--
Bug #68163: Using reference as object property name
--FILE--
<?php

$obj = (object) ['foo' => 'bar'];
$foo = 'foo';
$ref =& $foo;
var_dump($obj->$foo);

?>
--EXPECT--
string(3) "bar"
