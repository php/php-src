--TEST--
decrement (--) operator with strict_operators
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_one_operand('--$a', function($a) { return --$a; });

--EXPECT--
--false - TypeError Unsupported operand
--true - TypeError Unsupported operand
--0 = -1
--10 = 9
--0.0 = -1.0
--10.0 = 9.0
--3.14 = 2.14
--'0' - TypeError Unsupported operand
--'10' - TypeError Unsupported operand
--'010' - TypeError Unsupported operand
--'10 elephants' - TypeError Unsupported operand
--'foo' - TypeError Unsupported operand
--array ( ) - TypeError Unsupported operand
--array ( 0 => 1 ) - TypeError Unsupported operand
--array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand
--array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand
--array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand
--(object) array ( ) - TypeError Unsupported operand
--(object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand
--(object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand
--DateTime - TypeError Unsupported operand
--resource - TypeError Unsupported operand
--NULL - TypeError Unsupported operand