--TEST--
Test nullsafe operator in nested delayed dims
--FILE--
<?php

$foo = new stdClass();
$foo->bar = 'bar';

$array = ['foo' => ['bar' => 'baz']];

var_dump($array['foo'][$foo?->bar]);

?>
--EXPECT--
string(3) "baz"
