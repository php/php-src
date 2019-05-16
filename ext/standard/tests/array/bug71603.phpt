--TEST--
Bug #71603 (compact() maintains references in php7)
--FILE--
<?php
$foo = "okey";
$foo_reference =& $foo;

$array = compact('foo_reference');

$foo = 'changed!';

var_dump($array['foo_reference']);
--EXPECT--
string(4) "okey"
