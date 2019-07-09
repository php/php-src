--TEST--
multiplication '*' operator with strict_operators
--INI--
precision=14
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a * $b', function($a, $b) { return $a * $b; });

--EXPECT--
false * false - TypeError Unsupported operand type bool for multiplication
false * true - TypeError Unsupported operand type bool for multiplication
false * 0 - TypeError Unsupported operand type bool for multiplication
false * 10 - TypeError Unsupported operand type bool for multiplication
false * 0.0 - TypeError Unsupported operand type bool for multiplication
false * 10.0 - TypeError Unsupported operand type bool for multiplication
false * 3.14 - TypeError Unsupported operand type bool for multiplication
false * '0' - TypeError Unsupported operand type bool for multiplication
false * '10' - TypeError Unsupported operand type bool for multiplication
false * '010' - TypeError Unsupported operand type bool for multiplication
false * '10 elephants' - TypeError Unsupported operand type bool for multiplication
false * 'foo' - TypeError Unsupported operand type bool for multiplication
false * array ( ) - TypeError Unsupported operand type bool for multiplication
false * array ( 0 => 1 ) - TypeError Unsupported operand type bool for multiplication
false * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for multiplication
false * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for multiplication
false * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for multiplication
false * (object) array ( ) - TypeError Unsupported operand type bool for multiplication
false * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for multiplication
false * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for multiplication
false * DateTime - TypeError Unsupported operand type bool for multiplication
false * resource - TypeError Unsupported operand type bool for multiplication
false * NULL - TypeError Unsupported operand type bool for multiplication
true * false - TypeError Unsupported operand type bool for multiplication
true * true - TypeError Unsupported operand type bool for multiplication
true * 0 - TypeError Unsupported operand type bool for multiplication
true * 10 - TypeError Unsupported operand type bool for multiplication
true * 0.0 - TypeError Unsupported operand type bool for multiplication
true * 10.0 - TypeError Unsupported operand type bool for multiplication
true * 3.14 - TypeError Unsupported operand type bool for multiplication
true * '0' - TypeError Unsupported operand type bool for multiplication
true * '10' - TypeError Unsupported operand type bool for multiplication
true * '010' - TypeError Unsupported operand type bool for multiplication
true * '10 elephants' - TypeError Unsupported operand type bool for multiplication
true * 'foo' - TypeError Unsupported operand type bool for multiplication
true * array ( ) - TypeError Unsupported operand type bool for multiplication
true * array ( 0 => 1 ) - TypeError Unsupported operand type bool for multiplication
true * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for multiplication
true * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for multiplication
true * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for multiplication
true * (object) array ( ) - TypeError Unsupported operand type bool for multiplication
true * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for multiplication
true * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for multiplication
true * DateTime - TypeError Unsupported operand type bool for multiplication
true * resource - TypeError Unsupported operand type bool for multiplication
true * NULL - TypeError Unsupported operand type bool for multiplication
0 * false - TypeError Unsupported operand type bool for multiplication
0 * true - TypeError Unsupported operand type bool for multiplication
0 * 0 = 0
0 * 10 = 0
0 * 0.0 = 0.0
0 * 10.0 = 0.0
0 * 3.14 = 0.0
0 * '0' - TypeError Unsupported operand type string for multiplication
0 * '10' - TypeError Unsupported operand type string for multiplication
0 * '010' - TypeError Unsupported operand type string for multiplication
0 * '10 elephants' - TypeError Unsupported operand type string for multiplication
0 * 'foo' - TypeError Unsupported operand type string for multiplication
0 * array ( ) - TypeError Unsupported operand type array for multiplication
0 * array ( 0 => 1 ) - TypeError Unsupported operand type array for multiplication
0 * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for multiplication
0 * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for multiplication
0 * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for multiplication
0 * (object) array ( ) - TypeError Unsupported operand type object for multiplication
0 * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for multiplication
0 * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for multiplication
0 * DateTime - TypeError Unsupported operand type object for multiplication
0 * resource - TypeError Unsupported operand type resource for multiplication
0 * NULL - TypeError Unsupported operand type null for multiplication
10 * false - TypeError Unsupported operand type bool for multiplication
10 * true - TypeError Unsupported operand type bool for multiplication
10 * 0 = 0
10 * 10 = 100
10 * 0.0 = 0.0
10 * 10.0 = 100.0
10 * 3.14 = 31.400000000000002
10 * '0' - TypeError Unsupported operand type string for multiplication
10 * '10' - TypeError Unsupported operand type string for multiplication
10 * '010' - TypeError Unsupported operand type string for multiplication
10 * '10 elephants' - TypeError Unsupported operand type string for multiplication
10 * 'foo' - TypeError Unsupported operand type string for multiplication
10 * array ( ) - TypeError Unsupported operand type array for multiplication
10 * array ( 0 => 1 ) - TypeError Unsupported operand type array for multiplication
10 * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for multiplication
10 * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for multiplication
10 * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for multiplication
10 * (object) array ( ) - TypeError Unsupported operand type object for multiplication
10 * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for multiplication
10 * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for multiplication
10 * DateTime - TypeError Unsupported operand type object for multiplication
10 * resource - TypeError Unsupported operand type resource for multiplication
10 * NULL - TypeError Unsupported operand type null for multiplication
0.0 * false - TypeError Unsupported operand type bool for multiplication
0.0 * true - TypeError Unsupported operand type bool for multiplication
0.0 * 0 = 0.0
0.0 * 10 = 0.0
0.0 * 0.0 = 0.0
0.0 * 10.0 = 0.0
0.0 * 3.14 = 0.0
0.0 * '0' - TypeError Unsupported operand type string for multiplication
0.0 * '10' - TypeError Unsupported operand type string for multiplication
0.0 * '010' - TypeError Unsupported operand type string for multiplication
0.0 * '10 elephants' - TypeError Unsupported operand type string for multiplication
0.0 * 'foo' - TypeError Unsupported operand type string for multiplication
0.0 * array ( ) - TypeError Unsupported operand type array for multiplication
0.0 * array ( 0 => 1 ) - TypeError Unsupported operand type array for multiplication
0.0 * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for multiplication
0.0 * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for multiplication
0.0 * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for multiplication
0.0 * (object) array ( ) - TypeError Unsupported operand type object for multiplication
0.0 * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for multiplication
0.0 * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for multiplication
0.0 * DateTime - TypeError Unsupported operand type object for multiplication
0.0 * resource - TypeError Unsupported operand type resource for multiplication
0.0 * NULL - TypeError Unsupported operand type null for multiplication
10.0 * false - TypeError Unsupported operand type bool for multiplication
10.0 * true - TypeError Unsupported operand type bool for multiplication
10.0 * 0 = 0.0
10.0 * 10 = 100.0
10.0 * 0.0 = 0.0
10.0 * 10.0 = 100.0
10.0 * 3.14 = 31.400000000000002
10.0 * '0' - TypeError Unsupported operand type string for multiplication
10.0 * '10' - TypeError Unsupported operand type string for multiplication
10.0 * '010' - TypeError Unsupported operand type string for multiplication
10.0 * '10 elephants' - TypeError Unsupported operand type string for multiplication
10.0 * 'foo' - TypeError Unsupported operand type string for multiplication
10.0 * array ( ) - TypeError Unsupported operand type array for multiplication
10.0 * array ( 0 => 1 ) - TypeError Unsupported operand type array for multiplication
10.0 * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for multiplication
10.0 * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for multiplication
10.0 * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for multiplication
10.0 * (object) array ( ) - TypeError Unsupported operand type object for multiplication
10.0 * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for multiplication
10.0 * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for multiplication
10.0 * DateTime - TypeError Unsupported operand type object for multiplication
10.0 * resource - TypeError Unsupported operand type resource for multiplication
10.0 * NULL - TypeError Unsupported operand type null for multiplication
3.14 * false - TypeError Unsupported operand type bool for multiplication
3.14 * true - TypeError Unsupported operand type bool for multiplication
3.14 * 0 = 0.0
3.14 * 10 = 31.400000000000002
3.14 * 0.0 = 0.0
3.14 * 10.0 = 31.400000000000002
3.14 * 3.14 = 9.8596
3.14 * '0' - TypeError Unsupported operand type string for multiplication
3.14 * '10' - TypeError Unsupported operand type string for multiplication
3.14 * '010' - TypeError Unsupported operand type string for multiplication
3.14 * '10 elephants' - TypeError Unsupported operand type string for multiplication
3.14 * 'foo' - TypeError Unsupported operand type string for multiplication
3.14 * array ( ) - TypeError Unsupported operand type array for multiplication
3.14 * array ( 0 => 1 ) - TypeError Unsupported operand type array for multiplication
3.14 * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for multiplication
3.14 * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for multiplication
3.14 * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for multiplication
3.14 * (object) array ( ) - TypeError Unsupported operand type object for multiplication
3.14 * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for multiplication
3.14 * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for multiplication
3.14 * DateTime - TypeError Unsupported operand type object for multiplication
3.14 * resource - TypeError Unsupported operand type resource for multiplication
3.14 * NULL - TypeError Unsupported operand type null for multiplication
'0' * false - TypeError Unsupported operand type string for multiplication
'0' * true - TypeError Unsupported operand type string for multiplication
'0' * 0 - TypeError Unsupported operand type string for multiplication
'0' * 10 - TypeError Unsupported operand type string for multiplication
'0' * 0.0 - TypeError Unsupported operand type string for multiplication
'0' * 10.0 - TypeError Unsupported operand type string for multiplication
'0' * 3.14 - TypeError Unsupported operand type string for multiplication
'0' * '0' - TypeError Unsupported operand type string for multiplication
'0' * '10' - TypeError Unsupported operand type string for multiplication
'0' * '010' - TypeError Unsupported operand type string for multiplication
'0' * '10 elephants' - TypeError Unsupported operand type string for multiplication
'0' * 'foo' - TypeError Unsupported operand type string for multiplication
'0' * array ( ) - TypeError Unsupported operand type string for multiplication
'0' * array ( 0 => 1 ) - TypeError Unsupported operand type string for multiplication
'0' * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for multiplication
'0' * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for multiplication
'0' * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for multiplication
'0' * (object) array ( ) - TypeError Unsupported operand type string for multiplication
'0' * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for multiplication
'0' * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for multiplication
'0' * DateTime - TypeError Unsupported operand type string for multiplication
'0' * resource - TypeError Unsupported operand type string for multiplication
'0' * NULL - TypeError Unsupported operand type string for multiplication
'10' * false - TypeError Unsupported operand type string for multiplication
'10' * true - TypeError Unsupported operand type string for multiplication
'10' * 0 - TypeError Unsupported operand type string for multiplication
'10' * 10 - TypeError Unsupported operand type string for multiplication
'10' * 0.0 - TypeError Unsupported operand type string for multiplication
'10' * 10.0 - TypeError Unsupported operand type string for multiplication
'10' * 3.14 - TypeError Unsupported operand type string for multiplication
'10' * '0' - TypeError Unsupported operand type string for multiplication
'10' * '10' - TypeError Unsupported operand type string for multiplication
'10' * '010' - TypeError Unsupported operand type string for multiplication
'10' * '10 elephants' - TypeError Unsupported operand type string for multiplication
'10' * 'foo' - TypeError Unsupported operand type string for multiplication
'10' * array ( ) - TypeError Unsupported operand type string for multiplication
'10' * array ( 0 => 1 ) - TypeError Unsupported operand type string for multiplication
'10' * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for multiplication
'10' * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for multiplication
'10' * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for multiplication
'10' * (object) array ( ) - TypeError Unsupported operand type string for multiplication
'10' * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for multiplication
'10' * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for multiplication
'10' * DateTime - TypeError Unsupported operand type string for multiplication
'10' * resource - TypeError Unsupported operand type string for multiplication
'10' * NULL - TypeError Unsupported operand type string for multiplication
'010' * false - TypeError Unsupported operand type string for multiplication
'010' * true - TypeError Unsupported operand type string for multiplication
'010' * 0 - TypeError Unsupported operand type string for multiplication
'010' * 10 - TypeError Unsupported operand type string for multiplication
'010' * 0.0 - TypeError Unsupported operand type string for multiplication
'010' * 10.0 - TypeError Unsupported operand type string for multiplication
'010' * 3.14 - TypeError Unsupported operand type string for multiplication
'010' * '0' - TypeError Unsupported operand type string for multiplication
'010' * '10' - TypeError Unsupported operand type string for multiplication
'010' * '010' - TypeError Unsupported operand type string for multiplication
'010' * '10 elephants' - TypeError Unsupported operand type string for multiplication
'010' * 'foo' - TypeError Unsupported operand type string for multiplication
'010' * array ( ) - TypeError Unsupported operand type string for multiplication
'010' * array ( 0 => 1 ) - TypeError Unsupported operand type string for multiplication
'010' * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for multiplication
'010' * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for multiplication
'010' * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for multiplication
'010' * (object) array ( ) - TypeError Unsupported operand type string for multiplication
'010' * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for multiplication
'010' * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for multiplication
'010' * DateTime - TypeError Unsupported operand type string for multiplication
'010' * resource - TypeError Unsupported operand type string for multiplication
'010' * NULL - TypeError Unsupported operand type string for multiplication
'10 elephants' * false - TypeError Unsupported operand type string for multiplication
'10 elephants' * true - TypeError Unsupported operand type string for multiplication
'10 elephants' * 0 - TypeError Unsupported operand type string for multiplication
'10 elephants' * 10 - TypeError Unsupported operand type string for multiplication
'10 elephants' * 0.0 - TypeError Unsupported operand type string for multiplication
'10 elephants' * 10.0 - TypeError Unsupported operand type string for multiplication
'10 elephants' * 3.14 - TypeError Unsupported operand type string for multiplication
'10 elephants' * '0' - TypeError Unsupported operand type string for multiplication
'10 elephants' * '10' - TypeError Unsupported operand type string for multiplication
'10 elephants' * '010' - TypeError Unsupported operand type string for multiplication
'10 elephants' * '10 elephants' - TypeError Unsupported operand type string for multiplication
'10 elephants' * 'foo' - TypeError Unsupported operand type string for multiplication
'10 elephants' * array ( ) - TypeError Unsupported operand type string for multiplication
'10 elephants' * array ( 0 => 1 ) - TypeError Unsupported operand type string for multiplication
'10 elephants' * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for multiplication
'10 elephants' * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for multiplication
'10 elephants' * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for multiplication
'10 elephants' * (object) array ( ) - TypeError Unsupported operand type string for multiplication
'10 elephants' * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for multiplication
'10 elephants' * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for multiplication
'10 elephants' * DateTime - TypeError Unsupported operand type string for multiplication
'10 elephants' * resource - TypeError Unsupported operand type string for multiplication
'10 elephants' * NULL - TypeError Unsupported operand type string for multiplication
'foo' * false - TypeError Unsupported operand type string for multiplication
'foo' * true - TypeError Unsupported operand type string for multiplication
'foo' * 0 - TypeError Unsupported operand type string for multiplication
'foo' * 10 - TypeError Unsupported operand type string for multiplication
'foo' * 0.0 - TypeError Unsupported operand type string for multiplication
'foo' * 10.0 - TypeError Unsupported operand type string for multiplication
'foo' * 3.14 - TypeError Unsupported operand type string for multiplication
'foo' * '0' - TypeError Unsupported operand type string for multiplication
'foo' * '10' - TypeError Unsupported operand type string for multiplication
'foo' * '010' - TypeError Unsupported operand type string for multiplication
'foo' * '10 elephants' - TypeError Unsupported operand type string for multiplication
'foo' * 'foo' - TypeError Unsupported operand type string for multiplication
'foo' * array ( ) - TypeError Unsupported operand type string for multiplication
'foo' * array ( 0 => 1 ) - TypeError Unsupported operand type string for multiplication
'foo' * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for multiplication
'foo' * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for multiplication
'foo' * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for multiplication
'foo' * (object) array ( ) - TypeError Unsupported operand type string for multiplication
'foo' * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for multiplication
'foo' * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for multiplication
'foo' * DateTime - TypeError Unsupported operand type string for multiplication
'foo' * resource - TypeError Unsupported operand type string for multiplication
'foo' * NULL - TypeError Unsupported operand type string for multiplication
array ( ) * false - TypeError Unsupported operand type array for multiplication
array ( ) * true - TypeError Unsupported operand type array for multiplication
array ( ) * 0 - TypeError Unsupported operand type array for multiplication
array ( ) * 10 - TypeError Unsupported operand type array for multiplication
array ( ) * 0.0 - TypeError Unsupported operand type array for multiplication
array ( ) * 10.0 - TypeError Unsupported operand type array for multiplication
array ( ) * 3.14 - TypeError Unsupported operand type array for multiplication
array ( ) * '0' - TypeError Unsupported operand type array for multiplication
array ( ) * '10' - TypeError Unsupported operand type array for multiplication
array ( ) * '010' - TypeError Unsupported operand type array for multiplication
array ( ) * '10 elephants' - TypeError Unsupported operand type array for multiplication
array ( ) * 'foo' - TypeError Unsupported operand type array for multiplication
array ( ) * array ( ) - TypeError Unsupported operand type array for multiplication
array ( ) * array ( 0 => 1 ) - TypeError Unsupported operand type array for multiplication
array ( ) * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for multiplication
array ( ) * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for multiplication
array ( ) * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for multiplication
array ( ) * (object) array ( ) - TypeError Unsupported operand type array for multiplication
array ( ) * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for multiplication
array ( ) * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for multiplication
array ( ) * DateTime - TypeError Unsupported operand type array for multiplication
array ( ) * resource - TypeError Unsupported operand type array for multiplication
array ( ) * NULL - TypeError Unsupported operand type array for multiplication
array ( 0 => 1 ) * false - TypeError Unsupported operand type array for multiplication
array ( 0 => 1 ) * true - TypeError Unsupported operand type array for multiplication
array ( 0 => 1 ) * 0 - TypeError Unsupported operand type array for multiplication
array ( 0 => 1 ) * 10 - TypeError Unsupported operand type array for multiplication
array ( 0 => 1 ) * 0.0 - TypeError Unsupported operand type array for multiplication
array ( 0 => 1 ) * 10.0 - TypeError Unsupported operand type array for multiplication
array ( 0 => 1 ) * 3.14 - TypeError Unsupported operand type array for multiplication
array ( 0 => 1 ) * '0' - TypeError Unsupported operand type array for multiplication
array ( 0 => 1 ) * '10' - TypeError Unsupported operand type array for multiplication
array ( 0 => 1 ) * '010' - TypeError Unsupported operand type array for multiplication
array ( 0 => 1 ) * '10 elephants' - TypeError Unsupported operand type array for multiplication
array ( 0 => 1 ) * 'foo' - TypeError Unsupported operand type array for multiplication
array ( 0 => 1 ) * array ( ) - TypeError Unsupported operand type array for multiplication
array ( 0 => 1 ) * array ( 0 => 1 ) - TypeError Unsupported operand type array for multiplication
array ( 0 => 1 ) * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for multiplication
array ( 0 => 1 ) * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for multiplication
array ( 0 => 1 ) * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for multiplication
array ( 0 => 1 ) * (object) array ( ) - TypeError Unsupported operand type array for multiplication
array ( 0 => 1 ) * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for multiplication
array ( 0 => 1 ) * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for multiplication
array ( 0 => 1 ) * DateTime - TypeError Unsupported operand type array for multiplication
array ( 0 => 1 ) * resource - TypeError Unsupported operand type array for multiplication
array ( 0 => 1 ) * NULL - TypeError Unsupported operand type array for multiplication
array ( 0 => 1, 1 => 100 ) * false - TypeError Unsupported operand type array for multiplication
array ( 0 => 1, 1 => 100 ) * true - TypeError Unsupported operand type array for multiplication
array ( 0 => 1, 1 => 100 ) * 0 - TypeError Unsupported operand type array for multiplication
array ( 0 => 1, 1 => 100 ) * 10 - TypeError Unsupported operand type array for multiplication
array ( 0 => 1, 1 => 100 ) * 0.0 - TypeError Unsupported operand type array for multiplication
array ( 0 => 1, 1 => 100 ) * 10.0 - TypeError Unsupported operand type array for multiplication
array ( 0 => 1, 1 => 100 ) * 3.14 - TypeError Unsupported operand type array for multiplication
array ( 0 => 1, 1 => 100 ) * '0' - TypeError Unsupported operand type array for multiplication
array ( 0 => 1, 1 => 100 ) * '10' - TypeError Unsupported operand type array for multiplication
array ( 0 => 1, 1 => 100 ) * '010' - TypeError Unsupported operand type array for multiplication
array ( 0 => 1, 1 => 100 ) * '10 elephants' - TypeError Unsupported operand type array for multiplication
array ( 0 => 1, 1 => 100 ) * 'foo' - TypeError Unsupported operand type array for multiplication
array ( 0 => 1, 1 => 100 ) * array ( ) - TypeError Unsupported operand type array for multiplication
array ( 0 => 1, 1 => 100 ) * array ( 0 => 1 ) - TypeError Unsupported operand type array for multiplication
array ( 0 => 1, 1 => 100 ) * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for multiplication
array ( 0 => 1, 1 => 100 ) * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for multiplication
array ( 0 => 1, 1 => 100 ) * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for multiplication
array ( 0 => 1, 1 => 100 ) * (object) array ( ) - TypeError Unsupported operand type array for multiplication
array ( 0 => 1, 1 => 100 ) * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for multiplication
array ( 0 => 1, 1 => 100 ) * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for multiplication
array ( 0 => 1, 1 => 100 ) * DateTime - TypeError Unsupported operand type array for multiplication
array ( 0 => 1, 1 => 100 ) * resource - TypeError Unsupported operand type array for multiplication
array ( 0 => 1, 1 => 100 ) * NULL - TypeError Unsupported operand type array for multiplication
array ( 'foo' => 1, 'bar' => 2 ) * false - TypeError Unsupported operand type array for multiplication
array ( 'foo' => 1, 'bar' => 2 ) * true - TypeError Unsupported operand type array for multiplication
array ( 'foo' => 1, 'bar' => 2 ) * 0 - TypeError Unsupported operand type array for multiplication
array ( 'foo' => 1, 'bar' => 2 ) * 10 - TypeError Unsupported operand type array for multiplication
array ( 'foo' => 1, 'bar' => 2 ) * 0.0 - TypeError Unsupported operand type array for multiplication
array ( 'foo' => 1, 'bar' => 2 ) * 10.0 - TypeError Unsupported operand type array for multiplication
array ( 'foo' => 1, 'bar' => 2 ) * 3.14 - TypeError Unsupported operand type array for multiplication
array ( 'foo' => 1, 'bar' => 2 ) * '0' - TypeError Unsupported operand type array for multiplication
array ( 'foo' => 1, 'bar' => 2 ) * '10' - TypeError Unsupported operand type array for multiplication
array ( 'foo' => 1, 'bar' => 2 ) * '010' - TypeError Unsupported operand type array for multiplication
array ( 'foo' => 1, 'bar' => 2 ) * '10 elephants' - TypeError Unsupported operand type array for multiplication
array ( 'foo' => 1, 'bar' => 2 ) * 'foo' - TypeError Unsupported operand type array for multiplication
array ( 'foo' => 1, 'bar' => 2 ) * array ( ) - TypeError Unsupported operand type array for multiplication
array ( 'foo' => 1, 'bar' => 2 ) * array ( 0 => 1 ) - TypeError Unsupported operand type array for multiplication
array ( 'foo' => 1, 'bar' => 2 ) * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for multiplication
array ( 'foo' => 1, 'bar' => 2 ) * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for multiplication
array ( 'foo' => 1, 'bar' => 2 ) * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for multiplication
array ( 'foo' => 1, 'bar' => 2 ) * (object) array ( ) - TypeError Unsupported operand type array for multiplication
array ( 'foo' => 1, 'bar' => 2 ) * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for multiplication
array ( 'foo' => 1, 'bar' => 2 ) * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for multiplication
array ( 'foo' => 1, 'bar' => 2 ) * DateTime - TypeError Unsupported operand type array for multiplication
array ( 'foo' => 1, 'bar' => 2 ) * resource - TypeError Unsupported operand type array for multiplication
array ( 'foo' => 1, 'bar' => 2 ) * NULL - TypeError Unsupported operand type array for multiplication
array ( 'bar' => 1, 'foo' => 2 ) * false - TypeError Unsupported operand type array for multiplication
array ( 'bar' => 1, 'foo' => 2 ) * true - TypeError Unsupported operand type array for multiplication
array ( 'bar' => 1, 'foo' => 2 ) * 0 - TypeError Unsupported operand type array for multiplication
array ( 'bar' => 1, 'foo' => 2 ) * 10 - TypeError Unsupported operand type array for multiplication
array ( 'bar' => 1, 'foo' => 2 ) * 0.0 - TypeError Unsupported operand type array for multiplication
array ( 'bar' => 1, 'foo' => 2 ) * 10.0 - TypeError Unsupported operand type array for multiplication
array ( 'bar' => 1, 'foo' => 2 ) * 3.14 - TypeError Unsupported operand type array for multiplication
array ( 'bar' => 1, 'foo' => 2 ) * '0' - TypeError Unsupported operand type array for multiplication
array ( 'bar' => 1, 'foo' => 2 ) * '10' - TypeError Unsupported operand type array for multiplication
array ( 'bar' => 1, 'foo' => 2 ) * '010' - TypeError Unsupported operand type array for multiplication
array ( 'bar' => 1, 'foo' => 2 ) * '10 elephants' - TypeError Unsupported operand type array for multiplication
array ( 'bar' => 1, 'foo' => 2 ) * 'foo' - TypeError Unsupported operand type array for multiplication
array ( 'bar' => 1, 'foo' => 2 ) * array ( ) - TypeError Unsupported operand type array for multiplication
array ( 'bar' => 1, 'foo' => 2 ) * array ( 0 => 1 ) - TypeError Unsupported operand type array for multiplication
array ( 'bar' => 1, 'foo' => 2 ) * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for multiplication
array ( 'bar' => 1, 'foo' => 2 ) * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for multiplication
array ( 'bar' => 1, 'foo' => 2 ) * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for multiplication
array ( 'bar' => 1, 'foo' => 2 ) * (object) array ( ) - TypeError Unsupported operand type array for multiplication
array ( 'bar' => 1, 'foo' => 2 ) * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for multiplication
array ( 'bar' => 1, 'foo' => 2 ) * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for multiplication
array ( 'bar' => 1, 'foo' => 2 ) * DateTime - TypeError Unsupported operand type array for multiplication
array ( 'bar' => 1, 'foo' => 2 ) * resource - TypeError Unsupported operand type array for multiplication
array ( 'bar' => 1, 'foo' => 2 ) * NULL - TypeError Unsupported operand type array for multiplication
(object) array ( ) * false - TypeError Unsupported operand type object for multiplication
(object) array ( ) * true - TypeError Unsupported operand type object for multiplication
(object) array ( ) * 0 - TypeError Unsupported operand type object for multiplication
(object) array ( ) * 10 - TypeError Unsupported operand type object for multiplication
(object) array ( ) * 0.0 - TypeError Unsupported operand type object for multiplication
(object) array ( ) * 10.0 - TypeError Unsupported operand type object for multiplication
(object) array ( ) * 3.14 - TypeError Unsupported operand type object for multiplication
(object) array ( ) * '0' - TypeError Unsupported operand type object for multiplication
(object) array ( ) * '10' - TypeError Unsupported operand type object for multiplication
(object) array ( ) * '010' - TypeError Unsupported operand type object for multiplication
(object) array ( ) * '10 elephants' - TypeError Unsupported operand type object for multiplication
(object) array ( ) * 'foo' - TypeError Unsupported operand type object for multiplication
(object) array ( ) * array ( ) - TypeError Unsupported operand type object for multiplication
(object) array ( ) * array ( 0 => 1 ) - TypeError Unsupported operand type object for multiplication
(object) array ( ) * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for multiplication
(object) array ( ) * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for multiplication
(object) array ( ) * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for multiplication
(object) array ( ) * (object) array ( ) - TypeError Unsupported operand type object for multiplication
(object) array ( ) * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for multiplication
(object) array ( ) * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for multiplication
(object) array ( ) * DateTime - TypeError Unsupported operand type object for multiplication
(object) array ( ) * resource - TypeError Unsupported operand type object for multiplication
(object) array ( ) * NULL - TypeError Unsupported operand type object for multiplication
(object) array ( 'foo' => 1, 'bar' => 2 ) * false - TypeError Unsupported operand type object for multiplication
(object) array ( 'foo' => 1, 'bar' => 2 ) * true - TypeError Unsupported operand type object for multiplication
(object) array ( 'foo' => 1, 'bar' => 2 ) * 0 - TypeError Unsupported operand type object for multiplication
(object) array ( 'foo' => 1, 'bar' => 2 ) * 10 - TypeError Unsupported operand type object for multiplication
(object) array ( 'foo' => 1, 'bar' => 2 ) * 0.0 - TypeError Unsupported operand type object for multiplication
(object) array ( 'foo' => 1, 'bar' => 2 ) * 10.0 - TypeError Unsupported operand type object for multiplication
(object) array ( 'foo' => 1, 'bar' => 2 ) * 3.14 - TypeError Unsupported operand type object for multiplication
(object) array ( 'foo' => 1, 'bar' => 2 ) * '0' - TypeError Unsupported operand type object for multiplication
(object) array ( 'foo' => 1, 'bar' => 2 ) * '10' - TypeError Unsupported operand type object for multiplication
(object) array ( 'foo' => 1, 'bar' => 2 ) * '010' - TypeError Unsupported operand type object for multiplication
(object) array ( 'foo' => 1, 'bar' => 2 ) * '10 elephants' - TypeError Unsupported operand type object for multiplication
(object) array ( 'foo' => 1, 'bar' => 2 ) * 'foo' - TypeError Unsupported operand type object for multiplication
(object) array ( 'foo' => 1, 'bar' => 2 ) * array ( ) - TypeError Unsupported operand type object for multiplication
(object) array ( 'foo' => 1, 'bar' => 2 ) * array ( 0 => 1 ) - TypeError Unsupported operand type object for multiplication
(object) array ( 'foo' => 1, 'bar' => 2 ) * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for multiplication
(object) array ( 'foo' => 1, 'bar' => 2 ) * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for multiplication
(object) array ( 'foo' => 1, 'bar' => 2 ) * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for multiplication
(object) array ( 'foo' => 1, 'bar' => 2 ) * (object) array ( ) - TypeError Unsupported operand type object for multiplication
(object) array ( 'foo' => 1, 'bar' => 2 ) * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for multiplication
(object) array ( 'foo' => 1, 'bar' => 2 ) * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for multiplication
(object) array ( 'foo' => 1, 'bar' => 2 ) * DateTime - TypeError Unsupported operand type object for multiplication
(object) array ( 'foo' => 1, 'bar' => 2 ) * resource - TypeError Unsupported operand type object for multiplication
(object) array ( 'foo' => 1, 'bar' => 2 ) * NULL - TypeError Unsupported operand type object for multiplication
(object) array ( 'bar' => 1, 'foo' => 2 ) * false - TypeError Unsupported operand type object for multiplication
(object) array ( 'bar' => 1, 'foo' => 2 ) * true - TypeError Unsupported operand type object for multiplication
(object) array ( 'bar' => 1, 'foo' => 2 ) * 0 - TypeError Unsupported operand type object for multiplication
(object) array ( 'bar' => 1, 'foo' => 2 ) * 10 - TypeError Unsupported operand type object for multiplication
(object) array ( 'bar' => 1, 'foo' => 2 ) * 0.0 - TypeError Unsupported operand type object for multiplication
(object) array ( 'bar' => 1, 'foo' => 2 ) * 10.0 - TypeError Unsupported operand type object for multiplication
(object) array ( 'bar' => 1, 'foo' => 2 ) * 3.14 - TypeError Unsupported operand type object for multiplication
(object) array ( 'bar' => 1, 'foo' => 2 ) * '0' - TypeError Unsupported operand type object for multiplication
(object) array ( 'bar' => 1, 'foo' => 2 ) * '10' - TypeError Unsupported operand type object for multiplication
(object) array ( 'bar' => 1, 'foo' => 2 ) * '010' - TypeError Unsupported operand type object for multiplication
(object) array ( 'bar' => 1, 'foo' => 2 ) * '10 elephants' - TypeError Unsupported operand type object for multiplication
(object) array ( 'bar' => 1, 'foo' => 2 ) * 'foo' - TypeError Unsupported operand type object for multiplication
(object) array ( 'bar' => 1, 'foo' => 2 ) * array ( ) - TypeError Unsupported operand type object for multiplication
(object) array ( 'bar' => 1, 'foo' => 2 ) * array ( 0 => 1 ) - TypeError Unsupported operand type object for multiplication
(object) array ( 'bar' => 1, 'foo' => 2 ) * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for multiplication
(object) array ( 'bar' => 1, 'foo' => 2 ) * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for multiplication
(object) array ( 'bar' => 1, 'foo' => 2 ) * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for multiplication
(object) array ( 'bar' => 1, 'foo' => 2 ) * (object) array ( ) - TypeError Unsupported operand type object for multiplication
(object) array ( 'bar' => 1, 'foo' => 2 ) * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for multiplication
(object) array ( 'bar' => 1, 'foo' => 2 ) * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for multiplication
(object) array ( 'bar' => 1, 'foo' => 2 ) * DateTime - TypeError Unsupported operand type object for multiplication
(object) array ( 'bar' => 1, 'foo' => 2 ) * resource - TypeError Unsupported operand type object for multiplication
(object) array ( 'bar' => 1, 'foo' => 2 ) * NULL - TypeError Unsupported operand type object for multiplication
DateTime * false - TypeError Unsupported operand type object for multiplication
DateTime * true - TypeError Unsupported operand type object for multiplication
DateTime * 0 - TypeError Unsupported operand type object for multiplication
DateTime * 10 - TypeError Unsupported operand type object for multiplication
DateTime * 0.0 - TypeError Unsupported operand type object for multiplication
DateTime * 10.0 - TypeError Unsupported operand type object for multiplication
DateTime * 3.14 - TypeError Unsupported operand type object for multiplication
DateTime * '0' - TypeError Unsupported operand type object for multiplication
DateTime * '10' - TypeError Unsupported operand type object for multiplication
DateTime * '010' - TypeError Unsupported operand type object for multiplication
DateTime * '10 elephants' - TypeError Unsupported operand type object for multiplication
DateTime * 'foo' - TypeError Unsupported operand type object for multiplication
DateTime * array ( ) - TypeError Unsupported operand type object for multiplication
DateTime * array ( 0 => 1 ) - TypeError Unsupported operand type object for multiplication
DateTime * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for multiplication
DateTime * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for multiplication
DateTime * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for multiplication
DateTime * (object) array ( ) - TypeError Unsupported operand type object for multiplication
DateTime * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for multiplication
DateTime * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for multiplication
DateTime * DateTime - TypeError Unsupported operand type object for multiplication
DateTime * resource - TypeError Unsupported operand type object for multiplication
DateTime * NULL - TypeError Unsupported operand type object for multiplication
resource * false - TypeError Unsupported operand type resource for multiplication
resource * true - TypeError Unsupported operand type resource for multiplication
resource * 0 - TypeError Unsupported operand type resource for multiplication
resource * 10 - TypeError Unsupported operand type resource for multiplication
resource * 0.0 - TypeError Unsupported operand type resource for multiplication
resource * 10.0 - TypeError Unsupported operand type resource for multiplication
resource * 3.14 - TypeError Unsupported operand type resource for multiplication
resource * '0' - TypeError Unsupported operand type resource for multiplication
resource * '10' - TypeError Unsupported operand type resource for multiplication
resource * '010' - TypeError Unsupported operand type resource for multiplication
resource * '10 elephants' - TypeError Unsupported operand type resource for multiplication
resource * 'foo' - TypeError Unsupported operand type resource for multiplication
resource * array ( ) - TypeError Unsupported operand type resource for multiplication
resource * array ( 0 => 1 ) - TypeError Unsupported operand type resource for multiplication
resource * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type resource for multiplication
resource * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for multiplication
resource * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for multiplication
resource * (object) array ( ) - TypeError Unsupported operand type resource for multiplication
resource * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for multiplication
resource * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for multiplication
resource * DateTime - TypeError Unsupported operand type resource for multiplication
resource * resource - TypeError Unsupported operand type resource for multiplication
resource * NULL - TypeError Unsupported operand type resource for multiplication
NULL * false - TypeError Unsupported operand type null for multiplication
NULL * true - TypeError Unsupported operand type null for multiplication
NULL * 0 - TypeError Unsupported operand type null for multiplication
NULL * 10 - TypeError Unsupported operand type null for multiplication
NULL * 0.0 - TypeError Unsupported operand type null for multiplication
NULL * 10.0 - TypeError Unsupported operand type null for multiplication
NULL * 3.14 - TypeError Unsupported operand type null for multiplication
NULL * '0' - TypeError Unsupported operand type null for multiplication
NULL * '10' - TypeError Unsupported operand type null for multiplication
NULL * '010' - TypeError Unsupported operand type null for multiplication
NULL * '10 elephants' - TypeError Unsupported operand type null for multiplication
NULL * 'foo' - TypeError Unsupported operand type null for multiplication
NULL * array ( ) - TypeError Unsupported operand type null for multiplication
NULL * array ( 0 => 1 ) - TypeError Unsupported operand type null for multiplication
NULL * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type null for multiplication
NULL * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for multiplication
NULL * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for multiplication
NULL * (object) array ( ) - TypeError Unsupported operand type null for multiplication
NULL * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for multiplication
NULL * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for multiplication
NULL * DateTime - TypeError Unsupported operand type null for multiplication
NULL * resource - TypeError Unsupported operand type null for multiplication
NULL * NULL - TypeError Unsupported operand type null for multiplication