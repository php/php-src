--TEST--
bitwise shift left operator with strict operators
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a << $b', function($a, $b) { return $a << $b; }, 'var_out_base64');

--EXPECT--
false << false - TypeError Unsupported operand type bool for bit shift left
false << true - TypeError Unsupported operand type bool for bit shift left
false << 0 - TypeError Unsupported operand type bool for bit shift left
false << 10 - TypeError Unsupported operand type bool for bit shift left
false << 0.0 - TypeError Unsupported operand type bool for bit shift left
false << 10.0 - TypeError Unsupported operand type bool for bit shift left
false << 3.14 - TypeError Unsupported operand type bool for bit shift left
false << '0' - TypeError Unsupported operand type bool for bit shift left
false << '10' - TypeError Unsupported operand type bool for bit shift left
false << '010' - TypeError Unsupported operand type bool for bit shift left
false << '10 elephants' - TypeError Unsupported operand type bool for bit shift left
false << 'foo' - TypeError Unsupported operand type bool for bit shift left
false << array ( ) - TypeError Unsupported operand type bool for bit shift left
false << array ( 0 => 1 ) - TypeError Unsupported operand type bool for bit shift left
false << array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for bit shift left
false << array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for bit shift left
false << array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for bit shift left
false << (object) array ( ) - TypeError Unsupported operand type bool for bit shift left
false << (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for bit shift left
false << (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for bit shift left
false << DateTime - TypeError Unsupported operand type bool for bit shift left
false << resource - TypeError Unsupported operand type bool for bit shift left
false << NULL - TypeError Unsupported operand type bool for bit shift left
true << false - TypeError Unsupported operand type bool for bit shift left
true << true - TypeError Unsupported operand type bool for bit shift left
true << 0 - TypeError Unsupported operand type bool for bit shift left
true << 10 - TypeError Unsupported operand type bool for bit shift left
true << 0.0 - TypeError Unsupported operand type bool for bit shift left
true << 10.0 - TypeError Unsupported operand type bool for bit shift left
true << 3.14 - TypeError Unsupported operand type bool for bit shift left
true << '0' - TypeError Unsupported operand type bool for bit shift left
true << '10' - TypeError Unsupported operand type bool for bit shift left
true << '010' - TypeError Unsupported operand type bool for bit shift left
true << '10 elephants' - TypeError Unsupported operand type bool for bit shift left
true << 'foo' - TypeError Unsupported operand type bool for bit shift left
true << array ( ) - TypeError Unsupported operand type bool for bit shift left
true << array ( 0 => 1 ) - TypeError Unsupported operand type bool for bit shift left
true << array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for bit shift left
true << array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for bit shift left
true << array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for bit shift left
true << (object) array ( ) - TypeError Unsupported operand type bool for bit shift left
true << (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for bit shift left
true << (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for bit shift left
true << DateTime - TypeError Unsupported operand type bool for bit shift left
true << resource - TypeError Unsupported operand type bool for bit shift left
true << NULL - TypeError Unsupported operand type bool for bit shift left
0 << false - TypeError Unsupported operand type bool for bit shift left
0 << true - TypeError Unsupported operand type bool for bit shift left
0 << 0 = 0
0 << 10 = 0
0 << 0.0 - TypeError Unsupported operand type float for bit shift left
0 << 10.0 - TypeError Unsupported operand type float for bit shift left
0 << 3.14 - TypeError Unsupported operand type float for bit shift left
0 << '0' - TypeError Unsupported operand type string for bit shift left
0 << '10' - TypeError Unsupported operand type string for bit shift left
0 << '010' - TypeError Unsupported operand type string for bit shift left
0 << '10 elephants' - TypeError Unsupported operand type string for bit shift left
0 << 'foo' - TypeError Unsupported operand type string for bit shift left
0 << array ( ) - TypeError Unsupported operand type array for bit shift left
0 << array ( 0 => 1 ) - TypeError Unsupported operand type array for bit shift left
0 << array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bit shift left
0 << array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bit shift left
0 << array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bit shift left
0 << (object) array ( ) - TypeError Unsupported operand type object for bit shift left
0 << (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bit shift left
0 << (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bit shift left
0 << DateTime - TypeError Unsupported operand type object for bit shift left
0 << resource - TypeError Unsupported operand type resource for bit shift left
0 << NULL - TypeError Unsupported operand type null for bit shift left
10 << false - TypeError Unsupported operand type bool for bit shift left
10 << true - TypeError Unsupported operand type bool for bit shift left
10 << 0 = 10
10 << 10 = 10240
10 << 0.0 - TypeError Unsupported operand type float for bit shift left
10 << 10.0 - TypeError Unsupported operand type float for bit shift left
10 << 3.14 - TypeError Unsupported operand type float for bit shift left
10 << '0' - TypeError Unsupported operand type string for bit shift left
10 << '10' - TypeError Unsupported operand type string for bit shift left
10 << '010' - TypeError Unsupported operand type string for bit shift left
10 << '10 elephants' - TypeError Unsupported operand type string for bit shift left
10 << 'foo' - TypeError Unsupported operand type string for bit shift left
10 << array ( ) - TypeError Unsupported operand type array for bit shift left
10 << array ( 0 => 1 ) - TypeError Unsupported operand type array for bit shift left
10 << array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bit shift left
10 << array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bit shift left
10 << array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bit shift left
10 << (object) array ( ) - TypeError Unsupported operand type object for bit shift left
10 << (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bit shift left
10 << (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bit shift left
10 << DateTime - TypeError Unsupported operand type object for bit shift left
10 << resource - TypeError Unsupported operand type resource for bit shift left
10 << NULL - TypeError Unsupported operand type null for bit shift left
0.0 << false - TypeError Unsupported operand type float for bit shift left
0.0 << true - TypeError Unsupported operand type float for bit shift left
0.0 << 0 - TypeError Unsupported operand type float for bit shift left
0.0 << 10 - TypeError Unsupported operand type float for bit shift left
0.0 << 0.0 - TypeError Unsupported operand type float for bit shift left
0.0 << 10.0 - TypeError Unsupported operand type float for bit shift left
0.0 << 3.14 - TypeError Unsupported operand type float for bit shift left
0.0 << '0' - TypeError Unsupported operand type float for bit shift left
0.0 << '10' - TypeError Unsupported operand type float for bit shift left
0.0 << '010' - TypeError Unsupported operand type float for bit shift left
0.0 << '10 elephants' - TypeError Unsupported operand type float for bit shift left
0.0 << 'foo' - TypeError Unsupported operand type float for bit shift left
0.0 << array ( ) - TypeError Unsupported operand type float for bit shift left
0.0 << array ( 0 => 1 ) - TypeError Unsupported operand type float for bit shift left
0.0 << array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type float for bit shift left
0.0 << array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bit shift left
0.0 << array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bit shift left
0.0 << (object) array ( ) - TypeError Unsupported operand type float for bit shift left
0.0 << (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bit shift left
0.0 << (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bit shift left
0.0 << DateTime - TypeError Unsupported operand type float for bit shift left
0.0 << resource - TypeError Unsupported operand type float for bit shift left
0.0 << NULL - TypeError Unsupported operand type float for bit shift left
10.0 << false - TypeError Unsupported operand type float for bit shift left
10.0 << true - TypeError Unsupported operand type float for bit shift left
10.0 << 0 - TypeError Unsupported operand type float for bit shift left
10.0 << 10 - TypeError Unsupported operand type float for bit shift left
10.0 << 0.0 - TypeError Unsupported operand type float for bit shift left
10.0 << 10.0 - TypeError Unsupported operand type float for bit shift left
10.0 << 3.14 - TypeError Unsupported operand type float for bit shift left
10.0 << '0' - TypeError Unsupported operand type float for bit shift left
10.0 << '10' - TypeError Unsupported operand type float for bit shift left
10.0 << '010' - TypeError Unsupported operand type float for bit shift left
10.0 << '10 elephants' - TypeError Unsupported operand type float for bit shift left
10.0 << 'foo' - TypeError Unsupported operand type float for bit shift left
10.0 << array ( ) - TypeError Unsupported operand type float for bit shift left
10.0 << array ( 0 => 1 ) - TypeError Unsupported operand type float for bit shift left
10.0 << array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type float for bit shift left
10.0 << array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bit shift left
10.0 << array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bit shift left
10.0 << (object) array ( ) - TypeError Unsupported operand type float for bit shift left
10.0 << (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bit shift left
10.0 << (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bit shift left
10.0 << DateTime - TypeError Unsupported operand type float for bit shift left
10.0 << resource - TypeError Unsupported operand type float for bit shift left
10.0 << NULL - TypeError Unsupported operand type float for bit shift left
3.14 << false - TypeError Unsupported operand type float for bit shift left
3.14 << true - TypeError Unsupported operand type float for bit shift left
3.14 << 0 - TypeError Unsupported operand type float for bit shift left
3.14 << 10 - TypeError Unsupported operand type float for bit shift left
3.14 << 0.0 - TypeError Unsupported operand type float for bit shift left
3.14 << 10.0 - TypeError Unsupported operand type float for bit shift left
3.14 << 3.14 - TypeError Unsupported operand type float for bit shift left
3.14 << '0' - TypeError Unsupported operand type float for bit shift left
3.14 << '10' - TypeError Unsupported operand type float for bit shift left
3.14 << '010' - TypeError Unsupported operand type float for bit shift left
3.14 << '10 elephants' - TypeError Unsupported operand type float for bit shift left
3.14 << 'foo' - TypeError Unsupported operand type float for bit shift left
3.14 << array ( ) - TypeError Unsupported operand type float for bit shift left
3.14 << array ( 0 => 1 ) - TypeError Unsupported operand type float for bit shift left
3.14 << array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type float for bit shift left
3.14 << array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bit shift left
3.14 << array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bit shift left
3.14 << (object) array ( ) - TypeError Unsupported operand type float for bit shift left
3.14 << (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bit shift left
3.14 << (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bit shift left
3.14 << DateTime - TypeError Unsupported operand type float for bit shift left
3.14 << resource - TypeError Unsupported operand type float for bit shift left
3.14 << NULL - TypeError Unsupported operand type float for bit shift left
'0' << false - TypeError Unsupported operand type string for bit shift left
'0' << true - TypeError Unsupported operand type string for bit shift left
'0' << 0 - TypeError Unsupported operand type string for bit shift left
'0' << 10 - TypeError Unsupported operand type string for bit shift left
'0' << 0.0 - TypeError Unsupported operand type string for bit shift left
'0' << 10.0 - TypeError Unsupported operand type string for bit shift left
'0' << 3.14 - TypeError Unsupported operand type string for bit shift left
'0' << '0' - TypeError Unsupported operand type string for bit shift left
'0' << '10' - TypeError Unsupported operand type string for bit shift left
'0' << '010' - TypeError Unsupported operand type string for bit shift left
'0' << '10 elephants' - TypeError Unsupported operand type string for bit shift left
'0' << 'foo' - TypeError Unsupported operand type string for bit shift left
'0' << array ( ) - TypeError Unsupported operand type string for bit shift left
'0' << array ( 0 => 1 ) - TypeError Unsupported operand type string for bit shift left
'0' << array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for bit shift left
'0' << array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for bit shift left
'0' << array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for bit shift left
'0' << (object) array ( ) - TypeError Unsupported operand type string for bit shift left
'0' << (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for bit shift left
'0' << (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for bit shift left
'0' << DateTime - TypeError Unsupported operand type string for bit shift left
'0' << resource - TypeError Unsupported operand type string for bit shift left
'0' << NULL - TypeError Unsupported operand type string for bit shift left
'10' << false - TypeError Unsupported operand type string for bit shift left
'10' << true - TypeError Unsupported operand type string for bit shift left
'10' << 0 - TypeError Unsupported operand type string for bit shift left
'10' << 10 - TypeError Unsupported operand type string for bit shift left
'10' << 0.0 - TypeError Unsupported operand type string for bit shift left
'10' << 10.0 - TypeError Unsupported operand type string for bit shift left
'10' << 3.14 - TypeError Unsupported operand type string for bit shift left
'10' << '0' - TypeError Unsupported operand type string for bit shift left
'10' << '10' - TypeError Unsupported operand type string for bit shift left
'10' << '010' - TypeError Unsupported operand type string for bit shift left
'10' << '10 elephants' - TypeError Unsupported operand type string for bit shift left
'10' << 'foo' - TypeError Unsupported operand type string for bit shift left
'10' << array ( ) - TypeError Unsupported operand type string for bit shift left
'10' << array ( 0 => 1 ) - TypeError Unsupported operand type string for bit shift left
'10' << array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for bit shift left
'10' << array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for bit shift left
'10' << array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for bit shift left
'10' << (object) array ( ) - TypeError Unsupported operand type string for bit shift left
'10' << (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for bit shift left
'10' << (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for bit shift left
'10' << DateTime - TypeError Unsupported operand type string for bit shift left
'10' << resource - TypeError Unsupported operand type string for bit shift left
'10' << NULL - TypeError Unsupported operand type string for bit shift left
'010' << false - TypeError Unsupported operand type string for bit shift left
'010' << true - TypeError Unsupported operand type string for bit shift left
'010' << 0 - TypeError Unsupported operand type string for bit shift left
'010' << 10 - TypeError Unsupported operand type string for bit shift left
'010' << 0.0 - TypeError Unsupported operand type string for bit shift left
'010' << 10.0 - TypeError Unsupported operand type string for bit shift left
'010' << 3.14 - TypeError Unsupported operand type string for bit shift left
'010' << '0' - TypeError Unsupported operand type string for bit shift left
'010' << '10' - TypeError Unsupported operand type string for bit shift left
'010' << '010' - TypeError Unsupported operand type string for bit shift left
'010' << '10 elephants' - TypeError Unsupported operand type string for bit shift left
'010' << 'foo' - TypeError Unsupported operand type string for bit shift left
'010' << array ( ) - TypeError Unsupported operand type string for bit shift left
'010' << array ( 0 => 1 ) - TypeError Unsupported operand type string for bit shift left
'010' << array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for bit shift left
'010' << array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for bit shift left
'010' << array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for bit shift left
'010' << (object) array ( ) - TypeError Unsupported operand type string for bit shift left
'010' << (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for bit shift left
'010' << (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for bit shift left
'010' << DateTime - TypeError Unsupported operand type string for bit shift left
'010' << resource - TypeError Unsupported operand type string for bit shift left
'010' << NULL - TypeError Unsupported operand type string for bit shift left
'10 elephants' << false - TypeError Unsupported operand type string for bit shift left
'10 elephants' << true - TypeError Unsupported operand type string for bit shift left
'10 elephants' << 0 - TypeError Unsupported operand type string for bit shift left
'10 elephants' << 10 - TypeError Unsupported operand type string for bit shift left
'10 elephants' << 0.0 - TypeError Unsupported operand type string for bit shift left
'10 elephants' << 10.0 - TypeError Unsupported operand type string for bit shift left
'10 elephants' << 3.14 - TypeError Unsupported operand type string for bit shift left
'10 elephants' << '0' - TypeError Unsupported operand type string for bit shift left
'10 elephants' << '10' - TypeError Unsupported operand type string for bit shift left
'10 elephants' << '010' - TypeError Unsupported operand type string for bit shift left
'10 elephants' << '10 elephants' - TypeError Unsupported operand type string for bit shift left
'10 elephants' << 'foo' - TypeError Unsupported operand type string for bit shift left
'10 elephants' << array ( ) - TypeError Unsupported operand type string for bit shift left
'10 elephants' << array ( 0 => 1 ) - TypeError Unsupported operand type string for bit shift left
'10 elephants' << array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for bit shift left
'10 elephants' << array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for bit shift left
'10 elephants' << array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for bit shift left
'10 elephants' << (object) array ( ) - TypeError Unsupported operand type string for bit shift left
'10 elephants' << (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for bit shift left
'10 elephants' << (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for bit shift left
'10 elephants' << DateTime - TypeError Unsupported operand type string for bit shift left
'10 elephants' << resource - TypeError Unsupported operand type string for bit shift left
'10 elephants' << NULL - TypeError Unsupported operand type string for bit shift left
'foo' << false - TypeError Unsupported operand type string for bit shift left
'foo' << true - TypeError Unsupported operand type string for bit shift left
'foo' << 0 - TypeError Unsupported operand type string for bit shift left
'foo' << 10 - TypeError Unsupported operand type string for bit shift left
'foo' << 0.0 - TypeError Unsupported operand type string for bit shift left
'foo' << 10.0 - TypeError Unsupported operand type string for bit shift left
'foo' << 3.14 - TypeError Unsupported operand type string for bit shift left
'foo' << '0' - TypeError Unsupported operand type string for bit shift left
'foo' << '10' - TypeError Unsupported operand type string for bit shift left
'foo' << '010' - TypeError Unsupported operand type string for bit shift left
'foo' << '10 elephants' - TypeError Unsupported operand type string for bit shift left
'foo' << 'foo' - TypeError Unsupported operand type string for bit shift left
'foo' << array ( ) - TypeError Unsupported operand type string for bit shift left
'foo' << array ( 0 => 1 ) - TypeError Unsupported operand type string for bit shift left
'foo' << array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for bit shift left
'foo' << array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for bit shift left
'foo' << array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for bit shift left
'foo' << (object) array ( ) - TypeError Unsupported operand type string for bit shift left
'foo' << (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for bit shift left
'foo' << (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for bit shift left
'foo' << DateTime - TypeError Unsupported operand type string for bit shift left
'foo' << resource - TypeError Unsupported operand type string for bit shift left
'foo' << NULL - TypeError Unsupported operand type string for bit shift left
array ( ) << false - TypeError Unsupported operand type array for bit shift left
array ( ) << true - TypeError Unsupported operand type array for bit shift left
array ( ) << 0 - TypeError Unsupported operand type array for bit shift left
array ( ) << 10 - TypeError Unsupported operand type array for bit shift left
array ( ) << 0.0 - TypeError Unsupported operand type array for bit shift left
array ( ) << 10.0 - TypeError Unsupported operand type array for bit shift left
array ( ) << 3.14 - TypeError Unsupported operand type array for bit shift left
array ( ) << '0' - TypeError Unsupported operand type array for bit shift left
array ( ) << '10' - TypeError Unsupported operand type array for bit shift left
array ( ) << '010' - TypeError Unsupported operand type array for bit shift left
array ( ) << '10 elephants' - TypeError Unsupported operand type array for bit shift left
array ( ) << 'foo' - TypeError Unsupported operand type array for bit shift left
array ( ) << array ( ) - TypeError Unsupported operand type array for bit shift left
array ( ) << array ( 0 => 1 ) - TypeError Unsupported operand type array for bit shift left
array ( ) << array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bit shift left
array ( ) << array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bit shift left
array ( ) << array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bit shift left
array ( ) << (object) array ( ) - TypeError Unsupported operand type array for bit shift left
array ( ) << (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bit shift left
array ( ) << (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bit shift left
array ( ) << DateTime - TypeError Unsupported operand type array for bit shift left
array ( ) << resource - TypeError Unsupported operand type array for bit shift left
array ( ) << NULL - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1 ) << false - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1 ) << true - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1 ) << 0 - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1 ) << 10 - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1 ) << 0.0 - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1 ) << 10.0 - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1 ) << 3.14 - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1 ) << '0' - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1 ) << '10' - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1 ) << '010' - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1 ) << '10 elephants' - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1 ) << 'foo' - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1 ) << array ( ) - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1 ) << array ( 0 => 1 ) - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1 ) << array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1 ) << array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1 ) << array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1 ) << (object) array ( ) - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1 ) << (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1 ) << (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1 ) << DateTime - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1 ) << resource - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1 ) << NULL - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1, 1 => 100 ) << false - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1, 1 => 100 ) << true - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1, 1 => 100 ) << 0 - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1, 1 => 100 ) << 10 - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1, 1 => 100 ) << 0.0 - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1, 1 => 100 ) << 10.0 - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1, 1 => 100 ) << 3.14 - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1, 1 => 100 ) << '0' - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1, 1 => 100 ) << '10' - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1, 1 => 100 ) << '010' - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1, 1 => 100 ) << '10 elephants' - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1, 1 => 100 ) << 'foo' - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1, 1 => 100 ) << array ( ) - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1, 1 => 100 ) << array ( 0 => 1 ) - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1, 1 => 100 ) << array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1, 1 => 100 ) << array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1, 1 => 100 ) << array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1, 1 => 100 ) << (object) array ( ) - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1, 1 => 100 ) << (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1, 1 => 100 ) << (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1, 1 => 100 ) << DateTime - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1, 1 => 100 ) << resource - TypeError Unsupported operand type array for bit shift left
array ( 0 => 1, 1 => 100 ) << NULL - TypeError Unsupported operand type array for bit shift left
array ( 'foo' => 1, 'bar' => 2 ) << false - TypeError Unsupported operand type array for bit shift left
array ( 'foo' => 1, 'bar' => 2 ) << true - TypeError Unsupported operand type array for bit shift left
array ( 'foo' => 1, 'bar' => 2 ) << 0 - TypeError Unsupported operand type array for bit shift left
array ( 'foo' => 1, 'bar' => 2 ) << 10 - TypeError Unsupported operand type array for bit shift left
array ( 'foo' => 1, 'bar' => 2 ) << 0.0 - TypeError Unsupported operand type array for bit shift left
array ( 'foo' => 1, 'bar' => 2 ) << 10.0 - TypeError Unsupported operand type array for bit shift left
array ( 'foo' => 1, 'bar' => 2 ) << 3.14 - TypeError Unsupported operand type array for bit shift left
array ( 'foo' => 1, 'bar' => 2 ) << '0' - TypeError Unsupported operand type array for bit shift left
array ( 'foo' => 1, 'bar' => 2 ) << '10' - TypeError Unsupported operand type array for bit shift left
array ( 'foo' => 1, 'bar' => 2 ) << '010' - TypeError Unsupported operand type array for bit shift left
array ( 'foo' => 1, 'bar' => 2 ) << '10 elephants' - TypeError Unsupported operand type array for bit shift left
array ( 'foo' => 1, 'bar' => 2 ) << 'foo' - TypeError Unsupported operand type array for bit shift left
array ( 'foo' => 1, 'bar' => 2 ) << array ( ) - TypeError Unsupported operand type array for bit shift left
array ( 'foo' => 1, 'bar' => 2 ) << array ( 0 => 1 ) - TypeError Unsupported operand type array for bit shift left
array ( 'foo' => 1, 'bar' => 2 ) << array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bit shift left
array ( 'foo' => 1, 'bar' => 2 ) << array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bit shift left
array ( 'foo' => 1, 'bar' => 2 ) << array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bit shift left
array ( 'foo' => 1, 'bar' => 2 ) << (object) array ( ) - TypeError Unsupported operand type array for bit shift left
array ( 'foo' => 1, 'bar' => 2 ) << (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bit shift left
array ( 'foo' => 1, 'bar' => 2 ) << (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bit shift left
array ( 'foo' => 1, 'bar' => 2 ) << DateTime - TypeError Unsupported operand type array for bit shift left
array ( 'foo' => 1, 'bar' => 2 ) << resource - TypeError Unsupported operand type array for bit shift left
array ( 'foo' => 1, 'bar' => 2 ) << NULL - TypeError Unsupported operand type array for bit shift left
array ( 'bar' => 1, 'foo' => 2 ) << false - TypeError Unsupported operand type array for bit shift left
array ( 'bar' => 1, 'foo' => 2 ) << true - TypeError Unsupported operand type array for bit shift left
array ( 'bar' => 1, 'foo' => 2 ) << 0 - TypeError Unsupported operand type array for bit shift left
array ( 'bar' => 1, 'foo' => 2 ) << 10 - TypeError Unsupported operand type array for bit shift left
array ( 'bar' => 1, 'foo' => 2 ) << 0.0 - TypeError Unsupported operand type array for bit shift left
array ( 'bar' => 1, 'foo' => 2 ) << 10.0 - TypeError Unsupported operand type array for bit shift left
array ( 'bar' => 1, 'foo' => 2 ) << 3.14 - TypeError Unsupported operand type array for bit shift left
array ( 'bar' => 1, 'foo' => 2 ) << '0' - TypeError Unsupported operand type array for bit shift left
array ( 'bar' => 1, 'foo' => 2 ) << '10' - TypeError Unsupported operand type array for bit shift left
array ( 'bar' => 1, 'foo' => 2 ) << '010' - TypeError Unsupported operand type array for bit shift left
array ( 'bar' => 1, 'foo' => 2 ) << '10 elephants' - TypeError Unsupported operand type array for bit shift left
array ( 'bar' => 1, 'foo' => 2 ) << 'foo' - TypeError Unsupported operand type array for bit shift left
array ( 'bar' => 1, 'foo' => 2 ) << array ( ) - TypeError Unsupported operand type array for bit shift left
array ( 'bar' => 1, 'foo' => 2 ) << array ( 0 => 1 ) - TypeError Unsupported operand type array for bit shift left
array ( 'bar' => 1, 'foo' => 2 ) << array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bit shift left
array ( 'bar' => 1, 'foo' => 2 ) << array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bit shift left
array ( 'bar' => 1, 'foo' => 2 ) << array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bit shift left
array ( 'bar' => 1, 'foo' => 2 ) << (object) array ( ) - TypeError Unsupported operand type array for bit shift left
array ( 'bar' => 1, 'foo' => 2 ) << (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bit shift left
array ( 'bar' => 1, 'foo' => 2 ) << (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bit shift left
array ( 'bar' => 1, 'foo' => 2 ) << DateTime - TypeError Unsupported operand type array for bit shift left
array ( 'bar' => 1, 'foo' => 2 ) << resource - TypeError Unsupported operand type array for bit shift left
array ( 'bar' => 1, 'foo' => 2 ) << NULL - TypeError Unsupported operand type array for bit shift left
(object) array ( ) << false - TypeError Unsupported operand type object for bit shift left
(object) array ( ) << true - TypeError Unsupported operand type object for bit shift left
(object) array ( ) << 0 - TypeError Unsupported operand type object for bit shift left
(object) array ( ) << 10 - TypeError Unsupported operand type object for bit shift left
(object) array ( ) << 0.0 - TypeError Unsupported operand type object for bit shift left
(object) array ( ) << 10.0 - TypeError Unsupported operand type object for bit shift left
(object) array ( ) << 3.14 - TypeError Unsupported operand type object for bit shift left
(object) array ( ) << '0' - TypeError Unsupported operand type object for bit shift left
(object) array ( ) << '10' - TypeError Unsupported operand type object for bit shift left
(object) array ( ) << '010' - TypeError Unsupported operand type object for bit shift left
(object) array ( ) << '10 elephants' - TypeError Unsupported operand type object for bit shift left
(object) array ( ) << 'foo' - TypeError Unsupported operand type object for bit shift left
(object) array ( ) << array ( ) - TypeError Unsupported operand type object for bit shift left
(object) array ( ) << array ( 0 => 1 ) - TypeError Unsupported operand type object for bit shift left
(object) array ( ) << array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for bit shift left
(object) array ( ) << array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bit shift left
(object) array ( ) << array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bit shift left
(object) array ( ) << (object) array ( ) - TypeError Unsupported operand type object for bit shift left
(object) array ( ) << (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bit shift left
(object) array ( ) << (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bit shift left
(object) array ( ) << DateTime - TypeError Unsupported operand type object for bit shift left
(object) array ( ) << resource - TypeError Unsupported operand type object for bit shift left
(object) array ( ) << NULL - TypeError Unsupported operand type object for bit shift left
(object) array ( 'foo' => 1, 'bar' => 2 ) << false - TypeError Unsupported operand type object for bit shift left
(object) array ( 'foo' => 1, 'bar' => 2 ) << true - TypeError Unsupported operand type object for bit shift left
(object) array ( 'foo' => 1, 'bar' => 2 ) << 0 - TypeError Unsupported operand type object for bit shift left
(object) array ( 'foo' => 1, 'bar' => 2 ) << 10 - TypeError Unsupported operand type object for bit shift left
(object) array ( 'foo' => 1, 'bar' => 2 ) << 0.0 - TypeError Unsupported operand type object for bit shift left
(object) array ( 'foo' => 1, 'bar' => 2 ) << 10.0 - TypeError Unsupported operand type object for bit shift left
(object) array ( 'foo' => 1, 'bar' => 2 ) << 3.14 - TypeError Unsupported operand type object for bit shift left
(object) array ( 'foo' => 1, 'bar' => 2 ) << '0' - TypeError Unsupported operand type object for bit shift left
(object) array ( 'foo' => 1, 'bar' => 2 ) << '10' - TypeError Unsupported operand type object for bit shift left
(object) array ( 'foo' => 1, 'bar' => 2 ) << '010' - TypeError Unsupported operand type object for bit shift left
(object) array ( 'foo' => 1, 'bar' => 2 ) << '10 elephants' - TypeError Unsupported operand type object for bit shift left
(object) array ( 'foo' => 1, 'bar' => 2 ) << 'foo' - TypeError Unsupported operand type object for bit shift left
(object) array ( 'foo' => 1, 'bar' => 2 ) << array ( ) - TypeError Unsupported operand type object for bit shift left
(object) array ( 'foo' => 1, 'bar' => 2 ) << array ( 0 => 1 ) - TypeError Unsupported operand type object for bit shift left
(object) array ( 'foo' => 1, 'bar' => 2 ) << array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for bit shift left
(object) array ( 'foo' => 1, 'bar' => 2 ) << array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bit shift left
(object) array ( 'foo' => 1, 'bar' => 2 ) << array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bit shift left
(object) array ( 'foo' => 1, 'bar' => 2 ) << (object) array ( ) - TypeError Unsupported operand type object for bit shift left
(object) array ( 'foo' => 1, 'bar' => 2 ) << (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bit shift left
(object) array ( 'foo' => 1, 'bar' => 2 ) << (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bit shift left
(object) array ( 'foo' => 1, 'bar' => 2 ) << DateTime - TypeError Unsupported operand type object for bit shift left
(object) array ( 'foo' => 1, 'bar' => 2 ) << resource - TypeError Unsupported operand type object for bit shift left
(object) array ( 'foo' => 1, 'bar' => 2 ) << NULL - TypeError Unsupported operand type object for bit shift left
(object) array ( 'bar' => 1, 'foo' => 2 ) << false - TypeError Unsupported operand type object for bit shift left
(object) array ( 'bar' => 1, 'foo' => 2 ) << true - TypeError Unsupported operand type object for bit shift left
(object) array ( 'bar' => 1, 'foo' => 2 ) << 0 - TypeError Unsupported operand type object for bit shift left
(object) array ( 'bar' => 1, 'foo' => 2 ) << 10 - TypeError Unsupported operand type object for bit shift left
(object) array ( 'bar' => 1, 'foo' => 2 ) << 0.0 - TypeError Unsupported operand type object for bit shift left
(object) array ( 'bar' => 1, 'foo' => 2 ) << 10.0 - TypeError Unsupported operand type object for bit shift left
(object) array ( 'bar' => 1, 'foo' => 2 ) << 3.14 - TypeError Unsupported operand type object for bit shift left
(object) array ( 'bar' => 1, 'foo' => 2 ) << '0' - TypeError Unsupported operand type object for bit shift left
(object) array ( 'bar' => 1, 'foo' => 2 ) << '10' - TypeError Unsupported operand type object for bit shift left
(object) array ( 'bar' => 1, 'foo' => 2 ) << '010' - TypeError Unsupported operand type object for bit shift left
(object) array ( 'bar' => 1, 'foo' => 2 ) << '10 elephants' - TypeError Unsupported operand type object for bit shift left
(object) array ( 'bar' => 1, 'foo' => 2 ) << 'foo' - TypeError Unsupported operand type object for bit shift left
(object) array ( 'bar' => 1, 'foo' => 2 ) << array ( ) - TypeError Unsupported operand type object for bit shift left
(object) array ( 'bar' => 1, 'foo' => 2 ) << array ( 0 => 1 ) - TypeError Unsupported operand type object for bit shift left
(object) array ( 'bar' => 1, 'foo' => 2 ) << array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for bit shift left
(object) array ( 'bar' => 1, 'foo' => 2 ) << array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bit shift left
(object) array ( 'bar' => 1, 'foo' => 2 ) << array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bit shift left
(object) array ( 'bar' => 1, 'foo' => 2 ) << (object) array ( ) - TypeError Unsupported operand type object for bit shift left
(object) array ( 'bar' => 1, 'foo' => 2 ) << (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bit shift left
(object) array ( 'bar' => 1, 'foo' => 2 ) << (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bit shift left
(object) array ( 'bar' => 1, 'foo' => 2 ) << DateTime - TypeError Unsupported operand type object for bit shift left
(object) array ( 'bar' => 1, 'foo' => 2 ) << resource - TypeError Unsupported operand type object for bit shift left
(object) array ( 'bar' => 1, 'foo' => 2 ) << NULL - TypeError Unsupported operand type object for bit shift left
DateTime << false - TypeError Unsupported operand type object for bit shift left
DateTime << true - TypeError Unsupported operand type object for bit shift left
DateTime << 0 - TypeError Unsupported operand type object for bit shift left
DateTime << 10 - TypeError Unsupported operand type object for bit shift left
DateTime << 0.0 - TypeError Unsupported operand type object for bit shift left
DateTime << 10.0 - TypeError Unsupported operand type object for bit shift left
DateTime << 3.14 - TypeError Unsupported operand type object for bit shift left
DateTime << '0' - TypeError Unsupported operand type object for bit shift left
DateTime << '10' - TypeError Unsupported operand type object for bit shift left
DateTime << '010' - TypeError Unsupported operand type object for bit shift left
DateTime << '10 elephants' - TypeError Unsupported operand type object for bit shift left
DateTime << 'foo' - TypeError Unsupported operand type object for bit shift left
DateTime << array ( ) - TypeError Unsupported operand type object for bit shift left
DateTime << array ( 0 => 1 ) - TypeError Unsupported operand type object for bit shift left
DateTime << array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for bit shift left
DateTime << array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bit shift left
DateTime << array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bit shift left
DateTime << (object) array ( ) - TypeError Unsupported operand type object for bit shift left
DateTime << (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bit shift left
DateTime << (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bit shift left
DateTime << DateTime - TypeError Unsupported operand type object for bit shift left
DateTime << resource - TypeError Unsupported operand type object for bit shift left
DateTime << NULL - TypeError Unsupported operand type object for bit shift left
resource << false - TypeError Unsupported operand type resource for bit shift left
resource << true - TypeError Unsupported operand type resource for bit shift left
resource << 0 - TypeError Unsupported operand type resource for bit shift left
resource << 10 - TypeError Unsupported operand type resource for bit shift left
resource << 0.0 - TypeError Unsupported operand type resource for bit shift left
resource << 10.0 - TypeError Unsupported operand type resource for bit shift left
resource << 3.14 - TypeError Unsupported operand type resource for bit shift left
resource << '0' - TypeError Unsupported operand type resource for bit shift left
resource << '10' - TypeError Unsupported operand type resource for bit shift left
resource << '010' - TypeError Unsupported operand type resource for bit shift left
resource << '10 elephants' - TypeError Unsupported operand type resource for bit shift left
resource << 'foo' - TypeError Unsupported operand type resource for bit shift left
resource << array ( ) - TypeError Unsupported operand type resource for bit shift left
resource << array ( 0 => 1 ) - TypeError Unsupported operand type resource for bit shift left
resource << array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type resource for bit shift left
resource << array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for bit shift left
resource << array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for bit shift left
resource << (object) array ( ) - TypeError Unsupported operand type resource for bit shift left
resource << (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for bit shift left
resource << (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for bit shift left
resource << DateTime - TypeError Unsupported operand type resource for bit shift left
resource << resource - TypeError Unsupported operand type resource for bit shift left
resource << NULL - TypeError Unsupported operand type resource for bit shift left
NULL << false - TypeError Unsupported operand type null for bit shift left
NULL << true - TypeError Unsupported operand type null for bit shift left
NULL << 0 - TypeError Unsupported operand type null for bit shift left
NULL << 10 - TypeError Unsupported operand type null for bit shift left
NULL << 0.0 - TypeError Unsupported operand type null for bit shift left
NULL << 10.0 - TypeError Unsupported operand type null for bit shift left
NULL << 3.14 - TypeError Unsupported operand type null for bit shift left
NULL << '0' - TypeError Unsupported operand type null for bit shift left
NULL << '10' - TypeError Unsupported operand type null for bit shift left
NULL << '010' - TypeError Unsupported operand type null for bit shift left
NULL << '10 elephants' - TypeError Unsupported operand type null for bit shift left
NULL << 'foo' - TypeError Unsupported operand type null for bit shift left
NULL << array ( ) - TypeError Unsupported operand type null for bit shift left
NULL << array ( 0 => 1 ) - TypeError Unsupported operand type null for bit shift left
NULL << array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type null for bit shift left
NULL << array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for bit shift left
NULL << array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for bit shift left
NULL << (object) array ( ) - TypeError Unsupported operand type null for bit shift left
NULL << (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for bit shift left
NULL << (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for bit shift left
NULL << DateTime - TypeError Unsupported operand type null for bit shift left
NULL << resource - TypeError Unsupported operand type null for bit shift left
NULL << NULL - TypeError Unsupported operand type null for bit shift left