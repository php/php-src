--TEST--
equal '==' operator using array operands
--FILE--
<?php
require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

$fn = function($a, $b) { return $a == $b; };

two_operands('$a == $b', $fn, ['foo' => 0, 'bar' => 2], ['bar' => 2, 'foo' => 0]);
two_operands('$a == $b', $fn, ['foo' => 0, 'bar' => 2], ['bar' => 2, 'foo' => null]);
two_operands('$a == $b', $fn, ['foo' => 0, 'bar' => 2], ['bar' => '2', 'foo' => 0]);

--EXPECT--
array ( 'foo' => 0, 'bar' => 2 ) == array ( 'bar' => 2, 'foo' => 0 ) = true
array ( 'foo' => 0, 'bar' => 2 ) == array ( 'bar' => 2, 'foo' => NULL ) = true
array ( 'foo' => 0, 'bar' => 2 ) == array ( 'bar' => '2', 'foo' => 0 ) = true
