--TEST--
identity operator with strict_operators
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_one_operand('+$a', function($a) { return +$a; });

--EXPECT--
+false - TypeError Unsupported operand type bool for multiplication
+true - TypeError Unsupported operand type bool for multiplication
+0 = 0
+10 = 10
+0.0 = 0.0
+10.0 = 10.0
+3.14 = 3.14
+'0' - TypeError Unsupported operand type string for multiplication
+'10' - TypeError Unsupported operand type string for multiplication
+'010' - TypeError Unsupported operand type string for multiplication
+'10 elephants' - TypeError Unsupported operand type string for multiplication
+'foo' - TypeError Unsupported operand type string for multiplication
+array ( ) - TypeError Unsupported operand type array for multiplication
+array ( 0 => 1 ) - TypeError Unsupported operand type array for multiplication
+array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for multiplication
+array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for multiplication
+array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for multiplication
+(object) array ( ) - TypeError Unsupported operand type object for multiplication
+(object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for multiplication
+(object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for multiplication
+DateTime - TypeError Unsupported operand type object for multiplication
+resource - TypeError Unsupported operand type resource for multiplication
+NULL - TypeError Unsupported operand type null for multiplication