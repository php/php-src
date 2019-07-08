--TEST--
equal '==' operator using array operands with strict_operators
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

$fn = function($a, $b) { return $a == $b; };

two_operands('$a == $b', $fn, (object)['foo' => 0, 'bar' => 2], (object)['bar' => 2, 'foo' => 0]);
two_operands('$a == $b', $fn, (object)['foo' => 0, 'bar' => 2], (object)['bar' => 2, 'foo' => null]);
two_operands('$a == $b', $fn, (object)['foo' => 0, 'bar' => 2], (object)['bar' => '2', 'foo' => 0]);

two_operands('$a == $b', $fn, (object)['a' => ['foo' => 0, 'bar' => 2]], (object)['a' => ['bar' => 2, 'foo' => 0]]);
two_operands('$a == $b', $fn, (object)['a' => ['foo' => 0, 'bar' => 2]], (object)['a' => ['bar' => 2, 'foo' => null]]);
two_operands('$a == $b', $fn, (object)['a' => ['foo' => 0, 'bar' => 2]], (object)['a' => ['bar' => '2', 'foo' => 0]]);

two_operands('$a == $b', $fn, (object)['a' => (object)['foo' => 0, 'bar' => 2]], (object)['a' => (object)['bar' => 2, 'foo' => 0]]);
two_operands('$a == $b', $fn, (object)['a' => (object)['foo' => 0, 'bar' => 2]], (object)['a' => (object)['bar' => 2, 'foo' => null]]);
two_operands('$a == $b', $fn, (object)['a' => (object)['foo' => 0, 'bar' => 2]], (object)['a' => (object)['bar' => '2', 'foo' => 0]]);

--EXPECT--
(object) array ( 'foo' => 0, 'bar' => 2 ) == (object) array ( 'bar' => 2, 'foo' => 0 ) = true
(object) array ( 'foo' => 0, 'bar' => 2 ) == (object) array ( 'bar' => 2, 'foo' => NULL ) = false
(object) array ( 'foo' => 0, 'bar' => 2 ) == (object) array ( 'bar' => '2', 'foo' => 0 ) = false
(object) array ( 'a' =>  array ( 'foo' => 0, 'bar' => 2 ) ) == (object) array ( 'a' =>  array ( 'bar' => 2, 'foo' => 0 ) ) = true
(object) array ( 'a' =>  array ( 'foo' => 0, 'bar' => 2 ) ) == (object) array ( 'a' =>  array ( 'bar' => 2, 'foo' => NULL ) ) = false
(object) array ( 'a' =>  array ( 'foo' => 0, 'bar' => 2 ) ) == (object) array ( 'a' =>  array ( 'bar' => '2', 'foo' => 0 ) ) = false
(object) array ( 'a' =>  (object) array( 'foo' => 0, 'bar' => 2 ) ) == (object) array ( 'a' =>  (object) array( 'bar' => 2, 'foo' => 0 ) ) = true
(object) array ( 'a' =>  (object) array( 'foo' => 0, 'bar' => 2 ) ) == (object) array ( 'a' =>  (object) array( 'bar' => 2, 'foo' => NULL ) ) = false
(object) array ( 'a' =>  (object) array( 'foo' => 0, 'bar' => 2 ) ) == (object) array ( 'a' =>  (object) array( 'bar' => '2', 'foo' => 0 ) ) = false