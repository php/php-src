--TEST--
identity operator
--FILE--
<?php
require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_one_operand('+$a', function($a) { return +$a; });

--EXPECT--
+false = 0
+true = 1
+0 = 0
+10 = 10
+0.0 = 0.0
+10.0 = 10.0
+3.14 = 3.14
+'0' = 0
+'10' = 10
+'010' = 10
+'10 elephants' = 10 - Notice A non well formed numeric value encountered
+'foo' = 0 - Warning A non-numeric value encountered
+array ( ) - Error Unsupported operand types
+array ( 0 => 1 ) - Error Unsupported operand types
+array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
+array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
+array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
+(object) array ( ) = 1 - Notice Object of class stdClass could not be converted to number
+(object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
+(object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
+DateTime = 1 - Notice Object of class DateTime could not be converted to number
+resource = 6
+NULL = 0