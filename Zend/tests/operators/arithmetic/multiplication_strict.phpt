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
false * false - TypeError Unsupported operand type bool for '*' (multiplication) operator
false * true - TypeError Unsupported operand type bool for '*' (multiplication) operator
false * 0 - TypeError Unsupported operand type bool for '*' (multiplication) operator
false * 10 - TypeError Unsupported operand type bool for '*' (multiplication) operator
false * 0.0 - TypeError Unsupported operand type bool for '*' (multiplication) operator
false * 10.0 - TypeError Unsupported operand type bool for '*' (multiplication) operator
false * 3.14 - TypeError Unsupported operand type bool for '*' (multiplication) operator
false * '0' - TypeError Unsupported operand type bool for '*' (multiplication) operator
false * '10' - TypeError Unsupported operand type bool for '*' (multiplication) operator
false * '010' - TypeError Unsupported operand type bool for '*' (multiplication) operator
false * '10 elephants' - TypeError Unsupported operand type bool for '*' (multiplication) operator
false * 'foo' - TypeError Unsupported operand type bool for '*' (multiplication) operator
false * array ( ) - TypeError Unsupported operand type bool for '*' (multiplication) operator
false * array ( 0 => 1 ) - TypeError Unsupported operand type bool for '*' (multiplication) operator
false * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for '*' (multiplication) operator
false * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for '*' (multiplication) operator
false * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for '*' (multiplication) operator
false * (object) array ( ) - TypeError Unsupported operand type bool for '*' (multiplication) operator
false * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for '*' (multiplication) operator
false * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for '*' (multiplication) operator
false * DateTime - TypeError Unsupported operand type bool for '*' (multiplication) operator
false * resource - TypeError Unsupported operand type bool for '*' (multiplication) operator
false * NULL - TypeError Unsupported operand type bool for '*' (multiplication) operator
true * false - TypeError Unsupported operand type bool for '*' (multiplication) operator
true * true - TypeError Unsupported operand type bool for '*' (multiplication) operator
true * 0 - TypeError Unsupported operand type bool for '*' (multiplication) operator
true * 10 - TypeError Unsupported operand type bool for '*' (multiplication) operator
true * 0.0 - TypeError Unsupported operand type bool for '*' (multiplication) operator
true * 10.0 - TypeError Unsupported operand type bool for '*' (multiplication) operator
true * 3.14 - TypeError Unsupported operand type bool for '*' (multiplication) operator
true * '0' - TypeError Unsupported operand type bool for '*' (multiplication) operator
true * '10' - TypeError Unsupported operand type bool for '*' (multiplication) operator
true * '010' - TypeError Unsupported operand type bool for '*' (multiplication) operator
true * '10 elephants' - TypeError Unsupported operand type bool for '*' (multiplication) operator
true * 'foo' - TypeError Unsupported operand type bool for '*' (multiplication) operator
true * array ( ) - TypeError Unsupported operand type bool for '*' (multiplication) operator
true * array ( 0 => 1 ) - TypeError Unsupported operand type bool for '*' (multiplication) operator
true * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for '*' (multiplication) operator
true * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for '*' (multiplication) operator
true * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for '*' (multiplication) operator
true * (object) array ( ) - TypeError Unsupported operand type bool for '*' (multiplication) operator
true * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for '*' (multiplication) operator
true * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for '*' (multiplication) operator
true * DateTime - TypeError Unsupported operand type bool for '*' (multiplication) operator
true * resource - TypeError Unsupported operand type bool for '*' (multiplication) operator
true * NULL - TypeError Unsupported operand type bool for '*' (multiplication) operator
0 * false - TypeError Unsupported operand type bool for '*' (multiplication) operator
0 * true - TypeError Unsupported operand type bool for '*' (multiplication) operator
0 * 0 = 0
0 * 10 = 0
0 * 0.0 = 0.0
0 * 10.0 = 0.0
0 * 3.14 = 0.0
0 * '0' - TypeError Unsupported operand type string for '*' (multiplication) operator
0 * '10' - TypeError Unsupported operand type string for '*' (multiplication) operator
0 * '010' - TypeError Unsupported operand type string for '*' (multiplication) operator
0 * '10 elephants' - TypeError Unsupported operand type string for '*' (multiplication) operator
0 * 'foo' - TypeError Unsupported operand type string for '*' (multiplication) operator
0 * array ( ) - TypeError Unsupported operand type array for '*' (multiplication) operator
0 * array ( 0 => 1 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
0 * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
0 * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
0 * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
0 * (object) array ( ) - TypeError Unsupported operand type object for '*' (multiplication) operator
0 * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
0 * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
0 * DateTime - TypeError Unsupported operand type object for '*' (multiplication) operator
0 * resource - TypeError Unsupported operand type resource for '*' (multiplication) operator
0 * NULL - TypeError Unsupported operand type null for '*' (multiplication) operator
10 * false - TypeError Unsupported operand type bool for '*' (multiplication) operator
10 * true - TypeError Unsupported operand type bool for '*' (multiplication) operator
10 * 0 = 0
10 * 10 = 100
10 * 0.0 = 0.0
10 * 10.0 = 100.0
10 * 3.14 = 31.400000000000002
10 * '0' - TypeError Unsupported operand type string for '*' (multiplication) operator
10 * '10' - TypeError Unsupported operand type string for '*' (multiplication) operator
10 * '010' - TypeError Unsupported operand type string for '*' (multiplication) operator
10 * '10 elephants' - TypeError Unsupported operand type string for '*' (multiplication) operator
10 * 'foo' - TypeError Unsupported operand type string for '*' (multiplication) operator
10 * array ( ) - TypeError Unsupported operand type array for '*' (multiplication) operator
10 * array ( 0 => 1 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
10 * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
10 * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
10 * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
10 * (object) array ( ) - TypeError Unsupported operand type object for '*' (multiplication) operator
10 * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
10 * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
10 * DateTime - TypeError Unsupported operand type object for '*' (multiplication) operator
10 * resource - TypeError Unsupported operand type resource for '*' (multiplication) operator
10 * NULL - TypeError Unsupported operand type null for '*' (multiplication) operator
0.0 * false - TypeError Unsupported operand type bool for '*' (multiplication) operator
0.0 * true - TypeError Unsupported operand type bool for '*' (multiplication) operator
0.0 * 0 = 0.0
0.0 * 10 = 0.0
0.0 * 0.0 = 0.0
0.0 * 10.0 = 0.0
0.0 * 3.14 = 0.0
0.0 * '0' - TypeError Unsupported operand type string for '*' (multiplication) operator
0.0 * '10' - TypeError Unsupported operand type string for '*' (multiplication) operator
0.0 * '010' - TypeError Unsupported operand type string for '*' (multiplication) operator
0.0 * '10 elephants' - TypeError Unsupported operand type string for '*' (multiplication) operator
0.0 * 'foo' - TypeError Unsupported operand type string for '*' (multiplication) operator
0.0 * array ( ) - TypeError Unsupported operand type array for '*' (multiplication) operator
0.0 * array ( 0 => 1 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
0.0 * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
0.0 * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
0.0 * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
0.0 * (object) array ( ) - TypeError Unsupported operand type object for '*' (multiplication) operator
0.0 * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
0.0 * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
0.0 * DateTime - TypeError Unsupported operand type object for '*' (multiplication) operator
0.0 * resource - TypeError Unsupported operand type resource for '*' (multiplication) operator
0.0 * NULL - TypeError Unsupported operand type null for '*' (multiplication) operator
10.0 * false - TypeError Unsupported operand type bool for '*' (multiplication) operator
10.0 * true - TypeError Unsupported operand type bool for '*' (multiplication) operator
10.0 * 0 = 0.0
10.0 * 10 = 100.0
10.0 * 0.0 = 0.0
10.0 * 10.0 = 100.0
10.0 * 3.14 = 31.400000000000002
10.0 * '0' - TypeError Unsupported operand type string for '*' (multiplication) operator
10.0 * '10' - TypeError Unsupported operand type string for '*' (multiplication) operator
10.0 * '010' - TypeError Unsupported operand type string for '*' (multiplication) operator
10.0 * '10 elephants' - TypeError Unsupported operand type string for '*' (multiplication) operator
10.0 * 'foo' - TypeError Unsupported operand type string for '*' (multiplication) operator
10.0 * array ( ) - TypeError Unsupported operand type array for '*' (multiplication) operator
10.0 * array ( 0 => 1 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
10.0 * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
10.0 * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
10.0 * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
10.0 * (object) array ( ) - TypeError Unsupported operand type object for '*' (multiplication) operator
10.0 * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
10.0 * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
10.0 * DateTime - TypeError Unsupported operand type object for '*' (multiplication) operator
10.0 * resource - TypeError Unsupported operand type resource for '*' (multiplication) operator
10.0 * NULL - TypeError Unsupported operand type null for '*' (multiplication) operator
3.14 * false - TypeError Unsupported operand type bool for '*' (multiplication) operator
3.14 * true - TypeError Unsupported operand type bool for '*' (multiplication) operator
3.14 * 0 = 0.0
3.14 * 10 = 31.400000000000002
3.14 * 0.0 = 0.0
3.14 * 10.0 = 31.400000000000002
3.14 * 3.14 = 9.8596
3.14 * '0' - TypeError Unsupported operand type string for '*' (multiplication) operator
3.14 * '10' - TypeError Unsupported operand type string for '*' (multiplication) operator
3.14 * '010' - TypeError Unsupported operand type string for '*' (multiplication) operator
3.14 * '10 elephants' - TypeError Unsupported operand type string for '*' (multiplication) operator
3.14 * 'foo' - TypeError Unsupported operand type string for '*' (multiplication) operator
3.14 * array ( ) - TypeError Unsupported operand type array for '*' (multiplication) operator
3.14 * array ( 0 => 1 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
3.14 * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
3.14 * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
3.14 * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
3.14 * (object) array ( ) - TypeError Unsupported operand type object for '*' (multiplication) operator
3.14 * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
3.14 * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
3.14 * DateTime - TypeError Unsupported operand type object for '*' (multiplication) operator
3.14 * resource - TypeError Unsupported operand type resource for '*' (multiplication) operator
3.14 * NULL - TypeError Unsupported operand type null for '*' (multiplication) operator
'0' * false - TypeError Unsupported operand type string for '*' (multiplication) operator
'0' * true - TypeError Unsupported operand type string for '*' (multiplication) operator
'0' * 0 - TypeError Unsupported operand type string for '*' (multiplication) operator
'0' * 10 - TypeError Unsupported operand type string for '*' (multiplication) operator
'0' * 0.0 - TypeError Unsupported operand type string for '*' (multiplication) operator
'0' * 10.0 - TypeError Unsupported operand type string for '*' (multiplication) operator
'0' * 3.14 - TypeError Unsupported operand type string for '*' (multiplication) operator
'0' * '0' - TypeError Unsupported operand type string for '*' (multiplication) operator
'0' * '10' - TypeError Unsupported operand type string for '*' (multiplication) operator
'0' * '010' - TypeError Unsupported operand type string for '*' (multiplication) operator
'0' * '10 elephants' - TypeError Unsupported operand type string for '*' (multiplication) operator
'0' * 'foo' - TypeError Unsupported operand type string for '*' (multiplication) operator
'0' * array ( ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'0' * array ( 0 => 1 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'0' * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'0' * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'0' * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'0' * (object) array ( ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'0' * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'0' * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'0' * DateTime - TypeError Unsupported operand type string for '*' (multiplication) operator
'0' * resource - TypeError Unsupported operand type string for '*' (multiplication) operator
'0' * NULL - TypeError Unsupported operand type string for '*' (multiplication) operator
'10' * false - TypeError Unsupported operand type string for '*' (multiplication) operator
'10' * true - TypeError Unsupported operand type string for '*' (multiplication) operator
'10' * 0 - TypeError Unsupported operand type string for '*' (multiplication) operator
'10' * 10 - TypeError Unsupported operand type string for '*' (multiplication) operator
'10' * 0.0 - TypeError Unsupported operand type string for '*' (multiplication) operator
'10' * 10.0 - TypeError Unsupported operand type string for '*' (multiplication) operator
'10' * 3.14 - TypeError Unsupported operand type string for '*' (multiplication) operator
'10' * '0' - TypeError Unsupported operand type string for '*' (multiplication) operator
'10' * '10' - TypeError Unsupported operand type string for '*' (multiplication) operator
'10' * '010' - TypeError Unsupported operand type string for '*' (multiplication) operator
'10' * '10 elephants' - TypeError Unsupported operand type string for '*' (multiplication) operator
'10' * 'foo' - TypeError Unsupported operand type string for '*' (multiplication) operator
'10' * array ( ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'10' * array ( 0 => 1 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'10' * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'10' * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'10' * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'10' * (object) array ( ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'10' * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'10' * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'10' * DateTime - TypeError Unsupported operand type string for '*' (multiplication) operator
'10' * resource - TypeError Unsupported operand type string for '*' (multiplication) operator
'10' * NULL - TypeError Unsupported operand type string for '*' (multiplication) operator
'010' * false - TypeError Unsupported operand type string for '*' (multiplication) operator
'010' * true - TypeError Unsupported operand type string for '*' (multiplication) operator
'010' * 0 - TypeError Unsupported operand type string for '*' (multiplication) operator
'010' * 10 - TypeError Unsupported operand type string for '*' (multiplication) operator
'010' * 0.0 - TypeError Unsupported operand type string for '*' (multiplication) operator
'010' * 10.0 - TypeError Unsupported operand type string for '*' (multiplication) operator
'010' * 3.14 - TypeError Unsupported operand type string for '*' (multiplication) operator
'010' * '0' - TypeError Unsupported operand type string for '*' (multiplication) operator
'010' * '10' - TypeError Unsupported operand type string for '*' (multiplication) operator
'010' * '010' - TypeError Unsupported operand type string for '*' (multiplication) operator
'010' * '10 elephants' - TypeError Unsupported operand type string for '*' (multiplication) operator
'010' * 'foo' - TypeError Unsupported operand type string for '*' (multiplication) operator
'010' * array ( ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'010' * array ( 0 => 1 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'010' * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'010' * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'010' * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'010' * (object) array ( ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'010' * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'010' * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'010' * DateTime - TypeError Unsupported operand type string for '*' (multiplication) operator
'010' * resource - TypeError Unsupported operand type string for '*' (multiplication) operator
'010' * NULL - TypeError Unsupported operand type string for '*' (multiplication) operator
'10 elephants' * false - TypeError Unsupported operand type string for '*' (multiplication) operator
'10 elephants' * true - TypeError Unsupported operand type string for '*' (multiplication) operator
'10 elephants' * 0 - TypeError Unsupported operand type string for '*' (multiplication) operator
'10 elephants' * 10 - TypeError Unsupported operand type string for '*' (multiplication) operator
'10 elephants' * 0.0 - TypeError Unsupported operand type string for '*' (multiplication) operator
'10 elephants' * 10.0 - TypeError Unsupported operand type string for '*' (multiplication) operator
'10 elephants' * 3.14 - TypeError Unsupported operand type string for '*' (multiplication) operator
'10 elephants' * '0' - TypeError Unsupported operand type string for '*' (multiplication) operator
'10 elephants' * '10' - TypeError Unsupported operand type string for '*' (multiplication) operator
'10 elephants' * '010' - TypeError Unsupported operand type string for '*' (multiplication) operator
'10 elephants' * '10 elephants' - TypeError Unsupported operand type string for '*' (multiplication) operator
'10 elephants' * 'foo' - TypeError Unsupported operand type string for '*' (multiplication) operator
'10 elephants' * array ( ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'10 elephants' * array ( 0 => 1 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'10 elephants' * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'10 elephants' * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'10 elephants' * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'10 elephants' * (object) array ( ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'10 elephants' * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'10 elephants' * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'10 elephants' * DateTime - TypeError Unsupported operand type string for '*' (multiplication) operator
'10 elephants' * resource - TypeError Unsupported operand type string for '*' (multiplication) operator
'10 elephants' * NULL - TypeError Unsupported operand type string for '*' (multiplication) operator
'foo' * false - TypeError Unsupported operand type string for '*' (multiplication) operator
'foo' * true - TypeError Unsupported operand type string for '*' (multiplication) operator
'foo' * 0 - TypeError Unsupported operand type string for '*' (multiplication) operator
'foo' * 10 - TypeError Unsupported operand type string for '*' (multiplication) operator
'foo' * 0.0 - TypeError Unsupported operand type string for '*' (multiplication) operator
'foo' * 10.0 - TypeError Unsupported operand type string for '*' (multiplication) operator
'foo' * 3.14 - TypeError Unsupported operand type string for '*' (multiplication) operator
'foo' * '0' - TypeError Unsupported operand type string for '*' (multiplication) operator
'foo' * '10' - TypeError Unsupported operand type string for '*' (multiplication) operator
'foo' * '010' - TypeError Unsupported operand type string for '*' (multiplication) operator
'foo' * '10 elephants' - TypeError Unsupported operand type string for '*' (multiplication) operator
'foo' * 'foo' - TypeError Unsupported operand type string for '*' (multiplication) operator
'foo' * array ( ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'foo' * array ( 0 => 1 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'foo' * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'foo' * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'foo' * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'foo' * (object) array ( ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'foo' * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'foo' * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for '*' (multiplication) operator
'foo' * DateTime - TypeError Unsupported operand type string for '*' (multiplication) operator
'foo' * resource - TypeError Unsupported operand type string for '*' (multiplication) operator
'foo' * NULL - TypeError Unsupported operand type string for '*' (multiplication) operator
array ( ) * false - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( ) * true - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( ) * 0 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( ) * 10 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( ) * 0.0 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( ) * 10.0 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( ) * 3.14 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( ) * '0' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( ) * '10' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( ) * '010' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( ) * '10 elephants' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( ) * 'foo' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( ) * array ( ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( ) * array ( 0 => 1 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( ) * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( ) * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( ) * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( ) * (object) array ( ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( ) * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( ) * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( ) * DateTime - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( ) * resource - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( ) * NULL - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1 ) * false - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1 ) * true - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1 ) * 0 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1 ) * 10 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1 ) * 0.0 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1 ) * 10.0 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1 ) * 3.14 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1 ) * '0' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1 ) * '10' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1 ) * '010' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1 ) * '10 elephants' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1 ) * 'foo' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1 ) * array ( ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1 ) * array ( 0 => 1 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1 ) * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1 ) * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1 ) * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1 ) * (object) array ( ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1 ) * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1 ) * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1 ) * DateTime - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1 ) * resource - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1 ) * NULL - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1, 1 => 100 ) * false - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1, 1 => 100 ) * true - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1, 1 => 100 ) * 0 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1, 1 => 100 ) * 10 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1, 1 => 100 ) * 0.0 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1, 1 => 100 ) * 10.0 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1, 1 => 100 ) * 3.14 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1, 1 => 100 ) * '0' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1, 1 => 100 ) * '10' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1, 1 => 100 ) * '010' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1, 1 => 100 ) * '10 elephants' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1, 1 => 100 ) * 'foo' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1, 1 => 100 ) * array ( ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1, 1 => 100 ) * array ( 0 => 1 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1, 1 => 100 ) * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1, 1 => 100 ) * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1, 1 => 100 ) * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1, 1 => 100 ) * (object) array ( ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1, 1 => 100 ) * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1, 1 => 100 ) * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1, 1 => 100 ) * DateTime - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1, 1 => 100 ) * resource - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 0 => 1, 1 => 100 ) * NULL - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'foo' => 1, 'bar' => 2 ) * false - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'foo' => 1, 'bar' => 2 ) * true - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'foo' => 1, 'bar' => 2 ) * 0 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'foo' => 1, 'bar' => 2 ) * 10 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'foo' => 1, 'bar' => 2 ) * 0.0 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'foo' => 1, 'bar' => 2 ) * 10.0 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'foo' => 1, 'bar' => 2 ) * 3.14 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'foo' => 1, 'bar' => 2 ) * '0' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'foo' => 1, 'bar' => 2 ) * '10' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'foo' => 1, 'bar' => 2 ) * '010' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'foo' => 1, 'bar' => 2 ) * '10 elephants' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'foo' => 1, 'bar' => 2 ) * 'foo' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'foo' => 1, 'bar' => 2 ) * array ( ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'foo' => 1, 'bar' => 2 ) * array ( 0 => 1 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'foo' => 1, 'bar' => 2 ) * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'foo' => 1, 'bar' => 2 ) * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'foo' => 1, 'bar' => 2 ) * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'foo' => 1, 'bar' => 2 ) * (object) array ( ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'foo' => 1, 'bar' => 2 ) * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'foo' => 1, 'bar' => 2 ) * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'foo' => 1, 'bar' => 2 ) * DateTime - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'foo' => 1, 'bar' => 2 ) * resource - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'foo' => 1, 'bar' => 2 ) * NULL - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'bar' => 1, 'foo' => 2 ) * false - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'bar' => 1, 'foo' => 2 ) * true - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'bar' => 1, 'foo' => 2 ) * 0 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'bar' => 1, 'foo' => 2 ) * 10 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'bar' => 1, 'foo' => 2 ) * 0.0 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'bar' => 1, 'foo' => 2 ) * 10.0 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'bar' => 1, 'foo' => 2 ) * 3.14 - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'bar' => 1, 'foo' => 2 ) * '0' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'bar' => 1, 'foo' => 2 ) * '10' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'bar' => 1, 'foo' => 2 ) * '010' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'bar' => 1, 'foo' => 2 ) * '10 elephants' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'bar' => 1, 'foo' => 2 ) * 'foo' - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'bar' => 1, 'foo' => 2 ) * array ( ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'bar' => 1, 'foo' => 2 ) * array ( 0 => 1 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'bar' => 1, 'foo' => 2 ) * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'bar' => 1, 'foo' => 2 ) * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'bar' => 1, 'foo' => 2 ) * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'bar' => 1, 'foo' => 2 ) * (object) array ( ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'bar' => 1, 'foo' => 2 ) * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'bar' => 1, 'foo' => 2 ) * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'bar' => 1, 'foo' => 2 ) * DateTime - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'bar' => 1, 'foo' => 2 ) * resource - TypeError Unsupported operand type array for '*' (multiplication) operator
array ( 'bar' => 1, 'foo' => 2 ) * NULL - TypeError Unsupported operand type array for '*' (multiplication) operator
(object) array ( ) * false - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( ) * true - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( ) * 0 - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( ) * 10 - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( ) * 0.0 - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( ) * 10.0 - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( ) * 3.14 - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( ) * '0' - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( ) * '10' - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( ) * '010' - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( ) * '10 elephants' - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( ) * 'foo' - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( ) * array ( ) - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( ) * array ( 0 => 1 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( ) * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( ) * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( ) * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( ) * (object) array ( ) - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( ) * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( ) * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( ) * DateTime - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( ) * resource - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( ) * NULL - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) * false - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) * true - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) * 0 - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) * 10 - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) * 0.0 - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) * 10.0 - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) * 3.14 - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) * '0' - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) * '10' - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) * '010' - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) * '10 elephants' - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) * 'foo' - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) * array ( ) - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) * array ( 0 => 1 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) * (object) array ( ) - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) * DateTime - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) * resource - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) * NULL - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) * false - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) * true - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) * 0 - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) * 10 - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) * 0.0 - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) * 10.0 - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) * 3.14 - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) * '0' - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) * '10' - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) * '010' - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) * '10 elephants' - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) * 'foo' - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) * array ( ) - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) * array ( 0 => 1 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) * (object) array ( ) - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) * DateTime - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) * resource - TypeError Unsupported operand type object for '*' (multiplication) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) * NULL - TypeError Unsupported operand type object for '*' (multiplication) operator
DateTime * false - TypeError Unsupported operand type object for '*' (multiplication) operator
DateTime * true - TypeError Unsupported operand type object for '*' (multiplication) operator
DateTime * 0 - TypeError Unsupported operand type object for '*' (multiplication) operator
DateTime * 10 - TypeError Unsupported operand type object for '*' (multiplication) operator
DateTime * 0.0 - TypeError Unsupported operand type object for '*' (multiplication) operator
DateTime * 10.0 - TypeError Unsupported operand type object for '*' (multiplication) operator
DateTime * 3.14 - TypeError Unsupported operand type object for '*' (multiplication) operator
DateTime * '0' - TypeError Unsupported operand type object for '*' (multiplication) operator
DateTime * '10' - TypeError Unsupported operand type object for '*' (multiplication) operator
DateTime * '010' - TypeError Unsupported operand type object for '*' (multiplication) operator
DateTime * '10 elephants' - TypeError Unsupported operand type object for '*' (multiplication) operator
DateTime * 'foo' - TypeError Unsupported operand type object for '*' (multiplication) operator
DateTime * array ( ) - TypeError Unsupported operand type object for '*' (multiplication) operator
DateTime * array ( 0 => 1 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
DateTime * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
DateTime * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
DateTime * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
DateTime * (object) array ( ) - TypeError Unsupported operand type object for '*' (multiplication) operator
DateTime * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
DateTime * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '*' (multiplication) operator
DateTime * DateTime - TypeError Unsupported operand type object for '*' (multiplication) operator
DateTime * resource - TypeError Unsupported operand type object for '*' (multiplication) operator
DateTime * NULL - TypeError Unsupported operand type object for '*' (multiplication) operator
resource * false - TypeError Unsupported operand type resource for '*' (multiplication) operator
resource * true - TypeError Unsupported operand type resource for '*' (multiplication) operator
resource * 0 - TypeError Unsupported operand type resource for '*' (multiplication) operator
resource * 10 - TypeError Unsupported operand type resource for '*' (multiplication) operator
resource * 0.0 - TypeError Unsupported operand type resource for '*' (multiplication) operator
resource * 10.0 - TypeError Unsupported operand type resource for '*' (multiplication) operator
resource * 3.14 - TypeError Unsupported operand type resource for '*' (multiplication) operator
resource * '0' - TypeError Unsupported operand type resource for '*' (multiplication) operator
resource * '10' - TypeError Unsupported operand type resource for '*' (multiplication) operator
resource * '010' - TypeError Unsupported operand type resource for '*' (multiplication) operator
resource * '10 elephants' - TypeError Unsupported operand type resource for '*' (multiplication) operator
resource * 'foo' - TypeError Unsupported operand type resource for '*' (multiplication) operator
resource * array ( ) - TypeError Unsupported operand type resource for '*' (multiplication) operator
resource * array ( 0 => 1 ) - TypeError Unsupported operand type resource for '*' (multiplication) operator
resource * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type resource for '*' (multiplication) operator
resource * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for '*' (multiplication) operator
resource * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for '*' (multiplication) operator
resource * (object) array ( ) - TypeError Unsupported operand type resource for '*' (multiplication) operator
resource * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for '*' (multiplication) operator
resource * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for '*' (multiplication) operator
resource * DateTime - TypeError Unsupported operand type resource for '*' (multiplication) operator
resource * resource - TypeError Unsupported operand type resource for '*' (multiplication) operator
resource * NULL - TypeError Unsupported operand type resource for '*' (multiplication) operator
NULL * false - TypeError Unsupported operand type null for '*' (multiplication) operator
NULL * true - TypeError Unsupported operand type null for '*' (multiplication) operator
NULL * 0 - TypeError Unsupported operand type null for '*' (multiplication) operator
NULL * 10 - TypeError Unsupported operand type null for '*' (multiplication) operator
NULL * 0.0 - TypeError Unsupported operand type null for '*' (multiplication) operator
NULL * 10.0 - TypeError Unsupported operand type null for '*' (multiplication) operator
NULL * 3.14 - TypeError Unsupported operand type null for '*' (multiplication) operator
NULL * '0' - TypeError Unsupported operand type null for '*' (multiplication) operator
NULL * '10' - TypeError Unsupported operand type null for '*' (multiplication) operator
NULL * '010' - TypeError Unsupported operand type null for '*' (multiplication) operator
NULL * '10 elephants' - TypeError Unsupported operand type null for '*' (multiplication) operator
NULL * 'foo' - TypeError Unsupported operand type null for '*' (multiplication) operator
NULL * array ( ) - TypeError Unsupported operand type null for '*' (multiplication) operator
NULL * array ( 0 => 1 ) - TypeError Unsupported operand type null for '*' (multiplication) operator
NULL * array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type null for '*' (multiplication) operator
NULL * array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for '*' (multiplication) operator
NULL * array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for '*' (multiplication) operator
NULL * (object) array ( ) - TypeError Unsupported operand type null for '*' (multiplication) operator
NULL * (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for '*' (multiplication) operator
NULL * (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for '*' (multiplication) operator
NULL * DateTime - TypeError Unsupported operand type null for '*' (multiplication) operator
NULL * resource - TypeError Unsupported operand type null for '*' (multiplication) operator
NULL * NULL - TypeError Unsupported operand type null for '*' (multiplication) operator