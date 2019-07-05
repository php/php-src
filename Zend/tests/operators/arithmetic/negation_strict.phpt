--TEST--
negation operator with strict_operators
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_one_operand('-$a', function($a) { return -$a; });

--EXPECT--
-false - TypeError Unsupported operand types
-true - TypeError Unsupported operand types
-0 = 0
-10 = -10
-0.0 = -0.0
-10.0 = -10.0
-3.14 = -3.14
-'0' - TypeError Unsupported operand types
-'10' - TypeError Unsupported operand types
-'010' - TypeError Unsupported operand types
-'10 elephants' - TypeError Unsupported operand types
-'foo' - TypeError Unsupported operand types
-array ( ) - TypeError Unsupported operand types
-array ( 0 => 1 ) - TypeError Unsupported operand types
-array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
-array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
-array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
-(object) array ( ) - TypeError Unsupported operand types
-(object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
-(object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
-DateTime - TypeError Unsupported operand types
-resource - TypeError Unsupported operand types
-NULL - TypeError Unsupported operand types