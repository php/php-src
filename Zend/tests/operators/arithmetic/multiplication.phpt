--TEST--
multiplication '*' operator
--FILE--
<?php
require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a * $b', function($a, $b) { return $a * $b; });

--EXPECT--
false * false = 0
false * true = 0
false * 0 = 0
false * 10 = 0
false * 0.0 = 0.0
false * 10.0 = 0.0
false * 3.14 = 0.0
false * '0' = 0
false * '10' = 0
false * '010' = 0
false * '10 elephants' = 0 - Notice A non well formed numeric value encountered
false * 'foo' = 0 - Warning A non-numeric value encountered
false * array ( ) - Error Unsupported operand types
false * array ( 0 => 1 ) - Error Unsupported operand types
false * array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
false * array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
false * array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
false * (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to number
false * (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to number
false * (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to number
false * DateTime = 0 - Notice Object of class DateTime could not be converted to number
false * resource = 0
false * NULL = 0
true * false = 0
true * true = 1
true * 0 = 0
true * 10 = 10
true * 0.0 = 0.0
true * 10.0 = 10.0
true * 3.14 = 3.14
true * '0' = 0
true * '10' = 10
true * '010' = 10
true * '10 elephants' = 10 - Notice A non well formed numeric value encountered
true * 'foo' = 0 - Warning A non-numeric value encountered
true * array ( ) - Error Unsupported operand types
true * array ( 0 => 1 ) - Error Unsupported operand types
true * array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
true * array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
true * array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
true * (object) array ( ) = 1 - Notice Object of class stdClass could not be converted to number
true * (object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
true * (object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
true * DateTime = 1 - Notice Object of class DateTime could not be converted to number
true * resource = 6
true * NULL = 0
0 * false = 0
0 * true = 0
0 * 0 = 0
0 * 10 = 0
0 * 0.0 = 0.0
0 * 10.0 = 0.0
0 * 3.14 = 0.0
0 * '0' = 0
0 * '10' = 0
0 * '010' = 0
0 * '10 elephants' = 0 - Notice A non well formed numeric value encountered
0 * 'foo' = 0 - Warning A non-numeric value encountered
0 * array ( ) - Error Unsupported operand types
0 * array ( 0 => 1 ) - Error Unsupported operand types
0 * array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
0 * array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
0 * array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
0 * (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to number
0 * (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to number
0 * (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to number
0 * DateTime = 0 - Notice Object of class DateTime could not be converted to number
0 * resource = 0
0 * NULL = 0
10 * false = 0
10 * true = 10
10 * 0 = 0
10 * 10 = 100
10 * 0.0 = 0.0
10 * 10.0 = 100.0
10 * 3.14 = 31.400000000000002
10 * '0' = 0
10 * '10' = 100
10 * '010' = 100
10 * '10 elephants' = 100 - Notice A non well formed numeric value encountered
10 * 'foo' = 0 - Warning A non-numeric value encountered
10 * array ( ) - Error Unsupported operand types
10 * array ( 0 => 1 ) - Error Unsupported operand types
10 * array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
10 * array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
10 * array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
10 * (object) array ( ) = 10 - Notice Object of class stdClass could not be converted to number
10 * (object) array ( 'foo' => 1, 'bar' => 2 ) = 10 - Notice Object of class stdClass could not be converted to number
10 * (object) array ( 'bar' => 1, 'foo' => 2 ) = 10 - Notice Object of class stdClass could not be converted to number
10 * DateTime = 10 - Notice Object of class DateTime could not be converted to number
10 * resource = 60
10 * NULL = 0
0.0 * false = 0.0
0.0 * true = 0.0
0.0 * 0 = 0.0
0.0 * 10 = 0.0
0.0 * 0.0 = 0.0
0.0 * 10.0 = 0.0
0.0 * 3.14 = 0.0
0.0 * '0' = 0.0
0.0 * '10' = 0.0
0.0 * '010' = 0.0
0.0 * '10 elephants' = 0.0 - Notice A non well formed numeric value encountered
0.0 * 'foo' = 0.0 - Warning A non-numeric value encountered
0.0 * array ( ) - Error Unsupported operand types
0.0 * array ( 0 => 1 ) - Error Unsupported operand types
0.0 * array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
0.0 * array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
0.0 * array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
0.0 * (object) array ( ) = 0.0 - Notice Object of class stdClass could not be converted to number
0.0 * (object) array ( 'foo' => 1, 'bar' => 2 ) = 0.0 - Notice Object of class stdClass could not be converted to number
0.0 * (object) array ( 'bar' => 1, 'foo' => 2 ) = 0.0 - Notice Object of class stdClass could not be converted to number
0.0 * DateTime = 0.0 - Notice Object of class DateTime could not be converted to number
0.0 * resource = 0.0
0.0 * NULL = 0.0
10.0 * false = 0.0
10.0 * true = 10.0
10.0 * 0 = 0.0
10.0 * 10 = 100.0
10.0 * 0.0 = 0.0
10.0 * 10.0 = 100.0
10.0 * 3.14 = 31.400000000000002
10.0 * '0' = 0.0
10.0 * '10' = 100.0
10.0 * '010' = 100.0
10.0 * '10 elephants' = 100.0 - Notice A non well formed numeric value encountered
10.0 * 'foo' = 0.0 - Warning A non-numeric value encountered
10.0 * array ( ) - Error Unsupported operand types
10.0 * array ( 0 => 1 ) - Error Unsupported operand types
10.0 * array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
10.0 * array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
10.0 * array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
10.0 * (object) array ( ) = 10.0 - Notice Object of class stdClass could not be converted to number
10.0 * (object) array ( 'foo' => 1, 'bar' => 2 ) = 10.0 - Notice Object of class stdClass could not be converted to number
10.0 * (object) array ( 'bar' => 1, 'foo' => 2 ) = 10.0 - Notice Object of class stdClass could not be converted to number
10.0 * DateTime = 10.0 - Notice Object of class DateTime could not be converted to number
10.0 * resource = 60.0
10.0 * NULL = 0.0
3.14 * false = 0.0
3.14 * true = 3.14
3.14 * 0 = 0.0
3.14 * 10 = 31.400000000000002
3.14 * 0.0 = 0.0
3.14 * 10.0 = 31.400000000000002
3.14 * 3.14 = 9.8596
3.14 * '0' = 0.0
3.14 * '10' = 31.400000000000002
3.14 * '010' = 31.400000000000002
3.14 * '10 elephants' = 31.400000000000002 - Notice A non well formed numeric value encountered
3.14 * 'foo' = 0.0 - Warning A non-numeric value encountered
3.14 * array ( ) - Error Unsupported operand types
3.14 * array ( 0 => 1 ) - Error Unsupported operand types
3.14 * array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
3.14 * array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
3.14 * array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
3.14 * (object) array ( ) = 3.14 - Notice Object of class stdClass could not be converted to number
3.14 * (object) array ( 'foo' => 1, 'bar' => 2 ) = 3.14 - Notice Object of class stdClass could not be converted to number
3.14 * (object) array ( 'bar' => 1, 'foo' => 2 ) = 3.14 - Notice Object of class stdClass could not be converted to number
3.14 * DateTime = 3.14 - Notice Object of class DateTime could not be converted to number
3.14 * resource = 18.84
3.14 * NULL = 0.0
'0' * false = 0
'0' * true = 0
'0' * 0 = 0
'0' * 10 = 0
'0' * 0.0 = 0.0
'0' * 10.0 = 0.0
'0' * 3.14 = 0.0
'0' * '0' = 0
'0' * '10' = 0
'0' * '010' = 0
'0' * '10 elephants' = 0 - Notice A non well formed numeric value encountered
'0' * 'foo' = 0 - Warning A non-numeric value encountered
'0' * array ( ) - Error Unsupported operand types
'0' * array ( 0 => 1 ) - Error Unsupported operand types
'0' * array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
'0' * array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
'0' * array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
'0' * (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to number
'0' * (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to number
'0' * (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to number
'0' * DateTime = 0 - Notice Object of class DateTime could not be converted to number
'0' * resource = 0
'0' * NULL = 0
'10' * false = 0
'10' * true = 10
'10' * 0 = 0
'10' * 10 = 100
'10' * 0.0 = 0.0
'10' * 10.0 = 100.0
'10' * 3.14 = 31.400000000000002
'10' * '0' = 0
'10' * '10' = 100
'10' * '010' = 100
'10' * '10 elephants' = 100 - Notice A non well formed numeric value encountered
'10' * 'foo' = 0 - Warning A non-numeric value encountered
'10' * array ( ) - Error Unsupported operand types
'10' * array ( 0 => 1 ) - Error Unsupported operand types
'10' * array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
'10' * array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
'10' * array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
'10' * (object) array ( ) = 10 - Notice Object of class stdClass could not be converted to number
'10' * (object) array ( 'foo' => 1, 'bar' => 2 ) = 10 - Notice Object of class stdClass could not be converted to number
'10' * (object) array ( 'bar' => 1, 'foo' => 2 ) = 10 - Notice Object of class stdClass could not be converted to number
'10' * DateTime = 10 - Notice Object of class DateTime could not be converted to number
'10' * resource = 60
'10' * NULL = 0
'010' * false = 0
'010' * true = 10
'010' * 0 = 0
'010' * 10 = 100
'010' * 0.0 = 0.0
'010' * 10.0 = 100.0
'010' * 3.14 = 31.400000000000002
'010' * '0' = 0
'010' * '10' = 100
'010' * '010' = 100
'010' * '10 elephants' = 100 - Notice A non well formed numeric value encountered
'010' * 'foo' = 0 - Warning A non-numeric value encountered
'010' * array ( ) - Error Unsupported operand types
'010' * array ( 0 => 1 ) - Error Unsupported operand types
'010' * array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
'010' * array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
'010' * array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
'010' * (object) array ( ) = 10 - Notice Object of class stdClass could not be converted to number
'010' * (object) array ( 'foo' => 1, 'bar' => 2 ) = 10 - Notice Object of class stdClass could not be converted to number
'010' * (object) array ( 'bar' => 1, 'foo' => 2 ) = 10 - Notice Object of class stdClass could not be converted to number
'010' * DateTime = 10 - Notice Object of class DateTime could not be converted to number
'010' * resource = 60
'010' * NULL = 0
'10 elephants' * false = 0 - Notice A non well formed numeric value encountered
'10 elephants' * true = 10 - Notice A non well formed numeric value encountered
'10 elephants' * 0 = 0 - Notice A non well formed numeric value encountered
'10 elephants' * 10 = 100 - Notice A non well formed numeric value encountered
'10 elephants' * 0.0 = 0.0 - Notice A non well formed numeric value encountered
'10 elephants' * 10.0 = 100.0 - Notice A non well formed numeric value encountered
'10 elephants' * 3.14 = 31.400000000000002 - Notice A non well formed numeric value encountered
'10 elephants' * '0' = 0 - Notice A non well formed numeric value encountered
'10 elephants' * '10' = 100 - Notice A non well formed numeric value encountered
'10 elephants' * '010' = 100 - Notice A non well formed numeric value encountered
'10 elephants' * '10 elephants' = 100 - Notice A non well formed numeric value encountered
'10 elephants' * 'foo' = 0 - Warning A non-numeric value encountered
'10 elephants' * array ( ) - Error Unsupported operand types
'10 elephants' * array ( 0 => 1 ) - Error Unsupported operand types
'10 elephants' * array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
'10 elephants' * array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
'10 elephants' * array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
'10 elephants' * (object) array ( ) = 10 - Notice Object of class stdClass could not be converted to number
'10 elephants' * (object) array ( 'foo' => 1, 'bar' => 2 ) = 10 - Notice Object of class stdClass could not be converted to number
'10 elephants' * (object) array ( 'bar' => 1, 'foo' => 2 ) = 10 - Notice Object of class stdClass could not be converted to number
'10 elephants' * DateTime = 10 - Notice Object of class DateTime could not be converted to number
'10 elephants' * resource = 60 - Notice A non well formed numeric value encountered
'10 elephants' * NULL = 0 - Notice A non well formed numeric value encountered
'foo' * false = 0 - Warning A non-numeric value encountered
'foo' * true = 0 - Warning A non-numeric value encountered
'foo' * 0 = 0 - Warning A non-numeric value encountered
'foo' * 10 = 0 - Warning A non-numeric value encountered
'foo' * 0.0 = 0.0 - Warning A non-numeric value encountered
'foo' * 10.0 = 0.0 - Warning A non-numeric value encountered
'foo' * 3.14 = 0.0 - Warning A non-numeric value encountered
'foo' * '0' = 0 - Warning A non-numeric value encountered
'foo' * '10' = 0 - Warning A non-numeric value encountered
'foo' * '010' = 0 - Warning A non-numeric value encountered
'foo' * '10 elephants' = 0 - Notice A non well formed numeric value encountered
'foo' * 'foo' = 0 - Warning A non-numeric value encountered
'foo' * array ( ) - Error Unsupported operand types
'foo' * array ( 0 => 1 ) - Error Unsupported operand types
'foo' * array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
'foo' * array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
'foo' * array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
'foo' * (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to number
'foo' * (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to number
'foo' * (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to number
'foo' * DateTime = 0 - Notice Object of class DateTime could not be converted to number
'foo' * resource = 0 - Warning A non-numeric value encountered
'foo' * NULL = 0 - Warning A non-numeric value encountered
array ( ) * false - Error Unsupported operand types
array ( ) * true - Error Unsupported operand types
array ( ) * 0 - Error Unsupported operand types
array ( ) * 10 - Error Unsupported operand types
array ( ) * 0.0 - Error Unsupported operand types
array ( ) * 10.0 - Error Unsupported operand types
array ( ) * 3.14 - Error Unsupported operand types
array ( ) * '0' - Error Unsupported operand types
array ( ) * '10' - Error Unsupported operand types
array ( ) * '010' - Error Unsupported operand types
array ( ) * '10 elephants' - Error Unsupported operand types
array ( ) * 'foo' - Error Unsupported operand types
array ( ) * array ( ) - Error Unsupported operand types
array ( ) * array ( 0 => 1 ) - Error Unsupported operand types
array ( ) * array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
array ( ) * array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
array ( ) * array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
array ( ) * (object) array ( ) - Error Unsupported operand types
array ( ) * (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
array ( ) * (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
array ( ) * DateTime - Error Unsupported operand types
array ( ) * resource - Error Unsupported operand types
array ( ) * NULL - Error Unsupported operand types
array ( 0 => 1 ) * false - Error Unsupported operand types
array ( 0 => 1 ) * true - Error Unsupported operand types
array ( 0 => 1 ) * 0 - Error Unsupported operand types
array ( 0 => 1 ) * 10 - Error Unsupported operand types
array ( 0 => 1 ) * 0.0 - Error Unsupported operand types
array ( 0 => 1 ) * 10.0 - Error Unsupported operand types
array ( 0 => 1 ) * 3.14 - Error Unsupported operand types
array ( 0 => 1 ) * '0' - Error Unsupported operand types
array ( 0 => 1 ) * '10' - Error Unsupported operand types
array ( 0 => 1 ) * '010' - Error Unsupported operand types
array ( 0 => 1 ) * '10 elephants' - Error Unsupported operand types
array ( 0 => 1 ) * 'foo' - Error Unsupported operand types
array ( 0 => 1 ) * array ( ) - Error Unsupported operand types
array ( 0 => 1 ) * array ( 0 => 1 ) - Error Unsupported operand types
array ( 0 => 1 ) * array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
array ( 0 => 1 ) * array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
array ( 0 => 1 ) * array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
array ( 0 => 1 ) * (object) array ( ) - Error Unsupported operand types
array ( 0 => 1 ) * (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
array ( 0 => 1 ) * (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
array ( 0 => 1 ) * DateTime - Error Unsupported operand types
array ( 0 => 1 ) * resource - Error Unsupported operand types
array ( 0 => 1 ) * NULL - Error Unsupported operand types
array ( 0 => 1, 1 => 100 ) * false - Error Unsupported operand types
array ( 0 => 1, 1 => 100 ) * true - Error Unsupported operand types
array ( 0 => 1, 1 => 100 ) * 0 - Error Unsupported operand types
array ( 0 => 1, 1 => 100 ) * 10 - Error Unsupported operand types
array ( 0 => 1, 1 => 100 ) * 0.0 - Error Unsupported operand types
array ( 0 => 1, 1 => 100 ) * 10.0 - Error Unsupported operand types
array ( 0 => 1, 1 => 100 ) * 3.14 - Error Unsupported operand types
array ( 0 => 1, 1 => 100 ) * '0' - Error Unsupported operand types
array ( 0 => 1, 1 => 100 ) * '10' - Error Unsupported operand types
array ( 0 => 1, 1 => 100 ) * '010' - Error Unsupported operand types
array ( 0 => 1, 1 => 100 ) * '10 elephants' - Error Unsupported operand types
array ( 0 => 1, 1 => 100 ) * 'foo' - Error Unsupported operand types
array ( 0 => 1, 1 => 100 ) * array ( ) - Error Unsupported operand types
array ( 0 => 1, 1 => 100 ) * array ( 0 => 1 ) - Error Unsupported operand types
array ( 0 => 1, 1 => 100 ) * array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
array ( 0 => 1, 1 => 100 ) * array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
array ( 0 => 1, 1 => 100 ) * array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
array ( 0 => 1, 1 => 100 ) * (object) array ( ) - Error Unsupported operand types
array ( 0 => 1, 1 => 100 ) * (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
array ( 0 => 1, 1 => 100 ) * (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
array ( 0 => 1, 1 => 100 ) * DateTime - Error Unsupported operand types
array ( 0 => 1, 1 => 100 ) * resource - Error Unsupported operand types
array ( 0 => 1, 1 => 100 ) * NULL - Error Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) * false - Error Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) * true - Error Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) * 0 - Error Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) * 10 - Error Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) * 0.0 - Error Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) * 10.0 - Error Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) * 3.14 - Error Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) * '0' - Error Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) * '10' - Error Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) * '010' - Error Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) * '10 elephants' - Error Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) * 'foo' - Error Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) * array ( ) - Error Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) * array ( 0 => 1 ) - Error Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) * array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) * array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) * array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) * (object) array ( ) - Error Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) * (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) * (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) * DateTime - Error Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) * resource - Error Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) * NULL - Error Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) * false - Error Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) * true - Error Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) * 0 - Error Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) * 10 - Error Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) * 0.0 - Error Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) * 10.0 - Error Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) * 3.14 - Error Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) * '0' - Error Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) * '10' - Error Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) * '010' - Error Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) * '10 elephants' - Error Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) * 'foo' - Error Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) * array ( ) - Error Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) * array ( 0 => 1 ) - Error Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) * array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) * array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) * array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) * (object) array ( ) - Error Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) * (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) * (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) * DateTime - Error Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) * resource - Error Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) * NULL - Error Unsupported operand types
(object) array ( ) * false = 0 - Notice Object of class stdClass could not be converted to number
(object) array ( ) * true = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( ) * 0 = 0 - Notice Object of class stdClass could not be converted to number
(object) array ( ) * 10 = 10 - Notice Object of class stdClass could not be converted to number
(object) array ( ) * 0.0 = 0.0 - Notice Object of class stdClass could not be converted to number
(object) array ( ) * 10.0 = 10.0 - Notice Object of class stdClass could not be converted to number
(object) array ( ) * 3.14 = 3.14 - Notice Object of class stdClass could not be converted to number
(object) array ( ) * '0' = 0 - Notice Object of class stdClass could not be converted to number
(object) array ( ) * '10' = 10 - Notice Object of class stdClass could not be converted to number
(object) array ( ) * '010' = 10 - Notice Object of class stdClass could not be converted to number
(object) array ( ) * '10 elephants' = 10 - Notice A non well formed numeric value encountered
(object) array ( ) * 'foo' = 0 - Warning A non-numeric value encountered
(object) array ( ) * array ( ) - Error Unsupported operand types
(object) array ( ) * array ( 0 => 1 ) - Error Unsupported operand types
(object) array ( ) * array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
(object) array ( ) * array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
(object) array ( ) * array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
(object) array ( ) * (object) array ( ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( ) * (object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( ) * (object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( ) * DateTime = 1 - Notice Object of class DateTime could not be converted to number
(object) array ( ) * resource = 6 - Notice Object of class stdClass could not be converted to number
(object) array ( ) * NULL = 0 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) * false = 0 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) * true = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) * 0 = 0 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) * 10 = 10 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) * 0.0 = 0.0 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) * 10.0 = 10.0 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) * 3.14 = 3.14 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) * '0' = 0 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) * '10' = 10 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) * '010' = 10 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) * '10 elephants' = 10 - Notice A non well formed numeric value encountered
(object) array ( 'foo' => 1, 'bar' => 2 ) * 'foo' = 0 - Warning A non-numeric value encountered
(object) array ( 'foo' => 1, 'bar' => 2 ) * array ( ) - Error Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) * array ( 0 => 1 ) - Error Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) * array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) * array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) * array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) * (object) array ( ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) * (object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) * (object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) * DateTime = 1 - Notice Object of class DateTime could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) * resource = 6 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) * NULL = 0 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) * false = 0 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) * true = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) * 0 = 0 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) * 10 = 10 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) * 0.0 = 0.0 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) * 10.0 = 10.0 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) * 3.14 = 3.14 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) * '0' = 0 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) * '10' = 10 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) * '010' = 10 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) * '10 elephants' = 10 - Notice A non well formed numeric value encountered
(object) array ( 'bar' => 1, 'foo' => 2 ) * 'foo' = 0 - Warning A non-numeric value encountered
(object) array ( 'bar' => 1, 'foo' => 2 ) * array ( ) - Error Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) * array ( 0 => 1 ) - Error Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) * array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) * array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) * array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) * (object) array ( ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) * (object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) * (object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) * DateTime = 1 - Notice Object of class DateTime could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) * resource = 6 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) * NULL = 0 - Notice Object of class stdClass could not be converted to number
DateTime * false = 0 - Notice Object of class DateTime could not be converted to number
DateTime * true = 1 - Notice Object of class DateTime could not be converted to number
DateTime * 0 = 0 - Notice Object of class DateTime could not be converted to number
DateTime * 10 = 10 - Notice Object of class DateTime could not be converted to number
DateTime * 0.0 = 0.0 - Notice Object of class DateTime could not be converted to number
DateTime * 10.0 = 10.0 - Notice Object of class DateTime could not be converted to number
DateTime * 3.14 = 3.14 - Notice Object of class DateTime could not be converted to number
DateTime * '0' = 0 - Notice Object of class DateTime could not be converted to number
DateTime * '10' = 10 - Notice Object of class DateTime could not be converted to number
DateTime * '010' = 10 - Notice Object of class DateTime could not be converted to number
DateTime * '10 elephants' = 10 - Notice A non well formed numeric value encountered
DateTime * 'foo' = 0 - Warning A non-numeric value encountered
DateTime * array ( ) - Error Unsupported operand types
DateTime * array ( 0 => 1 ) - Error Unsupported operand types
DateTime * array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
DateTime * array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
DateTime * array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
DateTime * (object) array ( ) = 1 - Notice Object of class stdClass could not be converted to number
DateTime * (object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
DateTime * (object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
DateTime * DateTime = 1 - Notice Object of class DateTime could not be converted to number
DateTime * resource = 6 - Notice Object of class DateTime could not be converted to number
DateTime * NULL = 0 - Notice Object of class DateTime could not be converted to number
resource * false = 0
resource * true = 6
resource * 0 = 0
resource * 10 = 60
resource * 0.0 = 0.0
resource * 10.0 = 60.0
resource * 3.14 = 18.84
resource * '0' = 0
resource * '10' = 60
resource * '010' = 60
resource * '10 elephants' = 60 - Notice A non well formed numeric value encountered
resource * 'foo' = 0 - Warning A non-numeric value encountered
resource * array ( ) - Error Unsupported operand types
resource * array ( 0 => 1 ) - Error Unsupported operand types
resource * array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
resource * array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
resource * array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
resource * (object) array ( ) = 6 - Notice Object of class stdClass could not be converted to number
resource * (object) array ( 'foo' => 1, 'bar' => 2 ) = 6 - Notice Object of class stdClass could not be converted to number
resource * (object) array ( 'bar' => 1, 'foo' => 2 ) = 6 - Notice Object of class stdClass could not be converted to number
resource * DateTime = 6 - Notice Object of class DateTime could not be converted to number
resource * resource = 36
resource * NULL = 0
NULL * false = 0
NULL * true = 0
NULL * 0 = 0
NULL * 10 = 0
NULL * 0.0 = 0.0
NULL * 10.0 = 0.0
NULL * 3.14 = 0.0
NULL * '0' = 0
NULL * '10' = 0
NULL * '010' = 0
NULL * '10 elephants' = 0 - Notice A non well formed numeric value encountered
NULL * 'foo' = 0 - Warning A non-numeric value encountered
NULL * array ( ) - Error Unsupported operand types
NULL * array ( 0 => 1 ) - Error Unsupported operand types
NULL * array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
NULL * array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
NULL * array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
NULL * (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to number
NULL * (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to number
NULL * (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to number
NULL * DateTime = 0 - Notice Object of class DateTime could not be converted to number
NULL * resource = 0
NULL * NULL = 0