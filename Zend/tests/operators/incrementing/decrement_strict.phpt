--TEST--
decrement (--) operator with strict_operators
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_one_operand('--$a', function($a) { return --$a; });

--EXPECT--
--false - TypeError Unsupported operand type bool for '--' (decrement) operator
--true - TypeError Unsupported operand type bool for '--' (decrement) operator
--0 = -1
--10 = 9
--0.0 = -1.0
--10.0 = 9.0
--3.14 = 2.14
--'0' - TypeError Unsupported operand type string for '--' (decrement) operator
--'10' - TypeError Unsupported operand type string for '--' (decrement) operator
--'010' - TypeError Unsupported operand type string for '--' (decrement) operator
--'10 elephants' - TypeError Unsupported operand type string for '--' (decrement) operator
--'foo' - TypeError Unsupported operand type string for '--' (decrement) operator
--array ( ) - TypeError Unsupported operand type array for '--' (decrement) operator
--array ( 0 => 1 ) - TypeError Unsupported operand type array for '--' (decrement) operator
--array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for '--' (decrement) operator
--array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '--' (decrement) operator
--array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '--' (decrement) operator
--(object) array ( ) - TypeError Unsupported operand type object for '--' (decrement) operator
--(object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '--' (decrement) operator
--(object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '--' (decrement) operator
--DateTime - TypeError Unsupported operand type object for '--' (decrement) operator
--resource - TypeError Unsupported operand type resource for '--' (decrement) operator
--NULL - TypeError Unsupported operand type null for '--' (decrement) operator