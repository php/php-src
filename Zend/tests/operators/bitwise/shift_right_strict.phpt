--TEST--
bitwise shift right operator with strict operators
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a >> $b', function($a, $b) { return $a >> $b; }, 'var_out_base64');

--EXPECT--
false >> false - TypeError Unsupported operand type bool for bit shift right
false >> true - TypeError Unsupported operand type bool for bit shift right
false >> 0 - TypeError Unsupported operand type bool for bit shift right
false >> 10 - TypeError Unsupported operand type bool for bit shift right
false >> 0.0 - TypeError Unsupported operand type bool for bit shift right
false >> 10.0 - TypeError Unsupported operand type bool for bit shift right
false >> 3.14 - TypeError Unsupported operand type bool for bit shift right
false >> '0' - TypeError Unsupported operand type bool for bit shift right
false >> '10' - TypeError Unsupported operand type bool for bit shift right
false >> '010' - TypeError Unsupported operand type bool for bit shift right
false >> '10 elephants' - TypeError Unsupported operand type bool for bit shift right
false >> 'foo' - TypeError Unsupported operand type bool for bit shift right
false >> array ( ) - TypeError Unsupported operand type bool for bit shift right
false >> array ( 0 => 1 ) - TypeError Unsupported operand type bool for bit shift right
false >> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for bit shift right
false >> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for bit shift right
false >> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for bit shift right
false >> (object) array ( ) - TypeError Unsupported operand type bool for bit shift right
false >> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for bit shift right
false >> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for bit shift right
false >> DateTime - TypeError Unsupported operand type bool for bit shift right
false >> resource - TypeError Unsupported operand type bool for bit shift right
false >> NULL - TypeError Unsupported operand type bool for bit shift right
true >> false - TypeError Unsupported operand type bool for bit shift right
true >> true - TypeError Unsupported operand type bool for bit shift right
true >> 0 - TypeError Unsupported operand type bool for bit shift right
true >> 10 - TypeError Unsupported operand type bool for bit shift right
true >> 0.0 - TypeError Unsupported operand type bool for bit shift right
true >> 10.0 - TypeError Unsupported operand type bool for bit shift right
true >> 3.14 - TypeError Unsupported operand type bool for bit shift right
true >> '0' - TypeError Unsupported operand type bool for bit shift right
true >> '10' - TypeError Unsupported operand type bool for bit shift right
true >> '010' - TypeError Unsupported operand type bool for bit shift right
true >> '10 elephants' - TypeError Unsupported operand type bool for bit shift right
true >> 'foo' - TypeError Unsupported operand type bool for bit shift right
true >> array ( ) - TypeError Unsupported operand type bool for bit shift right
true >> array ( 0 => 1 ) - TypeError Unsupported operand type bool for bit shift right
true >> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for bit shift right
true >> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for bit shift right
true >> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for bit shift right
true >> (object) array ( ) - TypeError Unsupported operand type bool for bit shift right
true >> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for bit shift right
true >> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for bit shift right
true >> DateTime - TypeError Unsupported operand type bool for bit shift right
true >> resource - TypeError Unsupported operand type bool for bit shift right
true >> NULL - TypeError Unsupported operand type bool for bit shift right
0 >> false - TypeError Unsupported operand type bool for bit shift right
0 >> true - TypeError Unsupported operand type bool for bit shift right
0 >> 0 = 0
0 >> 10 = 0
0 >> 0.0 - TypeError Unsupported operand type float for bit shift right
0 >> 10.0 - TypeError Unsupported operand type float for bit shift right
0 >> 3.14 - TypeError Unsupported operand type float for bit shift right
0 >> '0' - TypeError Unsupported operand type string for bit shift right
0 >> '10' - TypeError Unsupported operand type string for bit shift right
0 >> '010' - TypeError Unsupported operand type string for bit shift right
0 >> '10 elephants' - TypeError Unsupported operand type string for bit shift right
0 >> 'foo' - TypeError Unsupported operand type string for bit shift right
0 >> array ( ) - TypeError Unsupported operand type array for bit shift right
0 >> array ( 0 => 1 ) - TypeError Unsupported operand type array for bit shift right
0 >> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bit shift right
0 >> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bit shift right
0 >> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bit shift right
0 >> (object) array ( ) - TypeError Unsupported operand type object for bit shift right
0 >> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bit shift right
0 >> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bit shift right
0 >> DateTime - TypeError Unsupported operand type object for bit shift right
0 >> resource - TypeError Unsupported operand type resource for bit shift right
0 >> NULL - TypeError Unsupported operand type null for bit shift right
10 >> false - TypeError Unsupported operand type bool for bit shift right
10 >> true - TypeError Unsupported operand type bool for bit shift right
10 >> 0 = 10
10 >> 10 = 0
10 >> 0.0 - TypeError Unsupported operand type float for bit shift right
10 >> 10.0 - TypeError Unsupported operand type float for bit shift right
10 >> 3.14 - TypeError Unsupported operand type float for bit shift right
10 >> '0' - TypeError Unsupported operand type string for bit shift right
10 >> '10' - TypeError Unsupported operand type string for bit shift right
10 >> '010' - TypeError Unsupported operand type string for bit shift right
10 >> '10 elephants' - TypeError Unsupported operand type string for bit shift right
10 >> 'foo' - TypeError Unsupported operand type string for bit shift right
10 >> array ( ) - TypeError Unsupported operand type array for bit shift right
10 >> array ( 0 => 1 ) - TypeError Unsupported operand type array for bit shift right
10 >> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bit shift right
10 >> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bit shift right
10 >> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bit shift right
10 >> (object) array ( ) - TypeError Unsupported operand type object for bit shift right
10 >> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bit shift right
10 >> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bit shift right
10 >> DateTime - TypeError Unsupported operand type object for bit shift right
10 >> resource - TypeError Unsupported operand type resource for bit shift right
10 >> NULL - TypeError Unsupported operand type null for bit shift right
0.0 >> false - TypeError Unsupported operand type float for bit shift right
0.0 >> true - TypeError Unsupported operand type float for bit shift right
0.0 >> 0 - TypeError Unsupported operand type float for bit shift right
0.0 >> 10 - TypeError Unsupported operand type float for bit shift right
0.0 >> 0.0 - TypeError Unsupported operand type float for bit shift right
0.0 >> 10.0 - TypeError Unsupported operand type float for bit shift right
0.0 >> 3.14 - TypeError Unsupported operand type float for bit shift right
0.0 >> '0' - TypeError Unsupported operand type float for bit shift right
0.0 >> '10' - TypeError Unsupported operand type float for bit shift right
0.0 >> '010' - TypeError Unsupported operand type float for bit shift right
0.0 >> '10 elephants' - TypeError Unsupported operand type float for bit shift right
0.0 >> 'foo' - TypeError Unsupported operand type float for bit shift right
0.0 >> array ( ) - TypeError Unsupported operand type float for bit shift right
0.0 >> array ( 0 => 1 ) - TypeError Unsupported operand type float for bit shift right
0.0 >> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type float for bit shift right
0.0 >> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bit shift right
0.0 >> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bit shift right
0.0 >> (object) array ( ) - TypeError Unsupported operand type float for bit shift right
0.0 >> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bit shift right
0.0 >> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bit shift right
0.0 >> DateTime - TypeError Unsupported operand type float for bit shift right
0.0 >> resource - TypeError Unsupported operand type float for bit shift right
0.0 >> NULL - TypeError Unsupported operand type float for bit shift right
10.0 >> false - TypeError Unsupported operand type float for bit shift right
10.0 >> true - TypeError Unsupported operand type float for bit shift right
10.0 >> 0 - TypeError Unsupported operand type float for bit shift right
10.0 >> 10 - TypeError Unsupported operand type float for bit shift right
10.0 >> 0.0 - TypeError Unsupported operand type float for bit shift right
10.0 >> 10.0 - TypeError Unsupported operand type float for bit shift right
10.0 >> 3.14 - TypeError Unsupported operand type float for bit shift right
10.0 >> '0' - TypeError Unsupported operand type float for bit shift right
10.0 >> '10' - TypeError Unsupported operand type float for bit shift right
10.0 >> '010' - TypeError Unsupported operand type float for bit shift right
10.0 >> '10 elephants' - TypeError Unsupported operand type float for bit shift right
10.0 >> 'foo' - TypeError Unsupported operand type float for bit shift right
10.0 >> array ( ) - TypeError Unsupported operand type float for bit shift right
10.0 >> array ( 0 => 1 ) - TypeError Unsupported operand type float for bit shift right
10.0 >> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type float for bit shift right
10.0 >> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bit shift right
10.0 >> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bit shift right
10.0 >> (object) array ( ) - TypeError Unsupported operand type float for bit shift right
10.0 >> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bit shift right
10.0 >> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bit shift right
10.0 >> DateTime - TypeError Unsupported operand type float for bit shift right
10.0 >> resource - TypeError Unsupported operand type float for bit shift right
10.0 >> NULL - TypeError Unsupported operand type float for bit shift right
3.14 >> false - TypeError Unsupported operand type float for bit shift right
3.14 >> true - TypeError Unsupported operand type float for bit shift right
3.14 >> 0 - TypeError Unsupported operand type float for bit shift right
3.14 >> 10 - TypeError Unsupported operand type float for bit shift right
3.14 >> 0.0 - TypeError Unsupported operand type float for bit shift right
3.14 >> 10.0 - TypeError Unsupported operand type float for bit shift right
3.14 >> 3.14 - TypeError Unsupported operand type float for bit shift right
3.14 >> '0' - TypeError Unsupported operand type float for bit shift right
3.14 >> '10' - TypeError Unsupported operand type float for bit shift right
3.14 >> '010' - TypeError Unsupported operand type float for bit shift right
3.14 >> '10 elephants' - TypeError Unsupported operand type float for bit shift right
3.14 >> 'foo' - TypeError Unsupported operand type float for bit shift right
3.14 >> array ( ) - TypeError Unsupported operand type float for bit shift right
3.14 >> array ( 0 => 1 ) - TypeError Unsupported operand type float for bit shift right
3.14 >> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type float for bit shift right
3.14 >> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bit shift right
3.14 >> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bit shift right
3.14 >> (object) array ( ) - TypeError Unsupported operand type float for bit shift right
3.14 >> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bit shift right
3.14 >> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bit shift right
3.14 >> DateTime - TypeError Unsupported operand type float for bit shift right
3.14 >> resource - TypeError Unsupported operand type float for bit shift right
3.14 >> NULL - TypeError Unsupported operand type float for bit shift right
'0' >> false - TypeError Unsupported operand type string for bit shift right
'0' >> true - TypeError Unsupported operand type string for bit shift right
'0' >> 0 - TypeError Unsupported operand type string for bit shift right
'0' >> 10 - TypeError Unsupported operand type string for bit shift right
'0' >> 0.0 - TypeError Unsupported operand type string for bit shift right
'0' >> 10.0 - TypeError Unsupported operand type string for bit shift right
'0' >> 3.14 - TypeError Unsupported operand type string for bit shift right
'0' >> '0' - TypeError Unsupported operand type string for bit shift right
'0' >> '10' - TypeError Unsupported operand type string for bit shift right
'0' >> '010' - TypeError Unsupported operand type string for bit shift right
'0' >> '10 elephants' - TypeError Unsupported operand type string for bit shift right
'0' >> 'foo' - TypeError Unsupported operand type string for bit shift right
'0' >> array ( ) - TypeError Unsupported operand type string for bit shift right
'0' >> array ( 0 => 1 ) - TypeError Unsupported operand type string for bit shift right
'0' >> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for bit shift right
'0' >> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for bit shift right
'0' >> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for bit shift right
'0' >> (object) array ( ) - TypeError Unsupported operand type string for bit shift right
'0' >> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for bit shift right
'0' >> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for bit shift right
'0' >> DateTime - TypeError Unsupported operand type string for bit shift right
'0' >> resource - TypeError Unsupported operand type string for bit shift right
'0' >> NULL - TypeError Unsupported operand type string for bit shift right
'10' >> false - TypeError Unsupported operand type string for bit shift right
'10' >> true - TypeError Unsupported operand type string for bit shift right
'10' >> 0 - TypeError Unsupported operand type string for bit shift right
'10' >> 10 - TypeError Unsupported operand type string for bit shift right
'10' >> 0.0 - TypeError Unsupported operand type string for bit shift right
'10' >> 10.0 - TypeError Unsupported operand type string for bit shift right
'10' >> 3.14 - TypeError Unsupported operand type string for bit shift right
'10' >> '0' - TypeError Unsupported operand type string for bit shift right
'10' >> '10' - TypeError Unsupported operand type string for bit shift right
'10' >> '010' - TypeError Unsupported operand type string for bit shift right
'10' >> '10 elephants' - TypeError Unsupported operand type string for bit shift right
'10' >> 'foo' - TypeError Unsupported operand type string for bit shift right
'10' >> array ( ) - TypeError Unsupported operand type string for bit shift right
'10' >> array ( 0 => 1 ) - TypeError Unsupported operand type string for bit shift right
'10' >> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for bit shift right
'10' >> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for bit shift right
'10' >> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for bit shift right
'10' >> (object) array ( ) - TypeError Unsupported operand type string for bit shift right
'10' >> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for bit shift right
'10' >> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for bit shift right
'10' >> DateTime - TypeError Unsupported operand type string for bit shift right
'10' >> resource - TypeError Unsupported operand type string for bit shift right
'10' >> NULL - TypeError Unsupported operand type string for bit shift right
'010' >> false - TypeError Unsupported operand type string for bit shift right
'010' >> true - TypeError Unsupported operand type string for bit shift right
'010' >> 0 - TypeError Unsupported operand type string for bit shift right
'010' >> 10 - TypeError Unsupported operand type string for bit shift right
'010' >> 0.0 - TypeError Unsupported operand type string for bit shift right
'010' >> 10.0 - TypeError Unsupported operand type string for bit shift right
'010' >> 3.14 - TypeError Unsupported operand type string for bit shift right
'010' >> '0' - TypeError Unsupported operand type string for bit shift right
'010' >> '10' - TypeError Unsupported operand type string for bit shift right
'010' >> '010' - TypeError Unsupported operand type string for bit shift right
'010' >> '10 elephants' - TypeError Unsupported operand type string for bit shift right
'010' >> 'foo' - TypeError Unsupported operand type string for bit shift right
'010' >> array ( ) - TypeError Unsupported operand type string for bit shift right
'010' >> array ( 0 => 1 ) - TypeError Unsupported operand type string for bit shift right
'010' >> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for bit shift right
'010' >> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for bit shift right
'010' >> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for bit shift right
'010' >> (object) array ( ) - TypeError Unsupported operand type string for bit shift right
'010' >> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for bit shift right
'010' >> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for bit shift right
'010' >> DateTime - TypeError Unsupported operand type string for bit shift right
'010' >> resource - TypeError Unsupported operand type string for bit shift right
'010' >> NULL - TypeError Unsupported operand type string for bit shift right
'10 elephants' >> false - TypeError Unsupported operand type string for bit shift right
'10 elephants' >> true - TypeError Unsupported operand type string for bit shift right
'10 elephants' >> 0 - TypeError Unsupported operand type string for bit shift right
'10 elephants' >> 10 - TypeError Unsupported operand type string for bit shift right
'10 elephants' >> 0.0 - TypeError Unsupported operand type string for bit shift right
'10 elephants' >> 10.0 - TypeError Unsupported operand type string for bit shift right
'10 elephants' >> 3.14 - TypeError Unsupported operand type string for bit shift right
'10 elephants' >> '0' - TypeError Unsupported operand type string for bit shift right
'10 elephants' >> '10' - TypeError Unsupported operand type string for bit shift right
'10 elephants' >> '010' - TypeError Unsupported operand type string for bit shift right
'10 elephants' >> '10 elephants' - TypeError Unsupported operand type string for bit shift right
'10 elephants' >> 'foo' - TypeError Unsupported operand type string for bit shift right
'10 elephants' >> array ( ) - TypeError Unsupported operand type string for bit shift right
'10 elephants' >> array ( 0 => 1 ) - TypeError Unsupported operand type string for bit shift right
'10 elephants' >> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for bit shift right
'10 elephants' >> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for bit shift right
'10 elephants' >> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for bit shift right
'10 elephants' >> (object) array ( ) - TypeError Unsupported operand type string for bit shift right
'10 elephants' >> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for bit shift right
'10 elephants' >> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for bit shift right
'10 elephants' >> DateTime - TypeError Unsupported operand type string for bit shift right
'10 elephants' >> resource - TypeError Unsupported operand type string for bit shift right
'10 elephants' >> NULL - TypeError Unsupported operand type string for bit shift right
'foo' >> false - TypeError Unsupported operand type string for bit shift right
'foo' >> true - TypeError Unsupported operand type string for bit shift right
'foo' >> 0 - TypeError Unsupported operand type string for bit shift right
'foo' >> 10 - TypeError Unsupported operand type string for bit shift right
'foo' >> 0.0 - TypeError Unsupported operand type string for bit shift right
'foo' >> 10.0 - TypeError Unsupported operand type string for bit shift right
'foo' >> 3.14 - TypeError Unsupported operand type string for bit shift right
'foo' >> '0' - TypeError Unsupported operand type string for bit shift right
'foo' >> '10' - TypeError Unsupported operand type string for bit shift right
'foo' >> '010' - TypeError Unsupported operand type string for bit shift right
'foo' >> '10 elephants' - TypeError Unsupported operand type string for bit shift right
'foo' >> 'foo' - TypeError Unsupported operand type string for bit shift right
'foo' >> array ( ) - TypeError Unsupported operand type string for bit shift right
'foo' >> array ( 0 => 1 ) - TypeError Unsupported operand type string for bit shift right
'foo' >> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for bit shift right
'foo' >> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for bit shift right
'foo' >> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for bit shift right
'foo' >> (object) array ( ) - TypeError Unsupported operand type string for bit shift right
'foo' >> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for bit shift right
'foo' >> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for bit shift right
'foo' >> DateTime - TypeError Unsupported operand type string for bit shift right
'foo' >> resource - TypeError Unsupported operand type string for bit shift right
'foo' >> NULL - TypeError Unsupported operand type string for bit shift right
array ( ) >> false - TypeError Unsupported operand type array for bit shift right
array ( ) >> true - TypeError Unsupported operand type array for bit shift right
array ( ) >> 0 - TypeError Unsupported operand type array for bit shift right
array ( ) >> 10 - TypeError Unsupported operand type array for bit shift right
array ( ) >> 0.0 - TypeError Unsupported operand type array for bit shift right
array ( ) >> 10.0 - TypeError Unsupported operand type array for bit shift right
array ( ) >> 3.14 - TypeError Unsupported operand type array for bit shift right
array ( ) >> '0' - TypeError Unsupported operand type array for bit shift right
array ( ) >> '10' - TypeError Unsupported operand type array for bit shift right
array ( ) >> '010' - TypeError Unsupported operand type array for bit shift right
array ( ) >> '10 elephants' - TypeError Unsupported operand type array for bit shift right
array ( ) >> 'foo' - TypeError Unsupported operand type array for bit shift right
array ( ) >> array ( ) - TypeError Unsupported operand type array for bit shift right
array ( ) >> array ( 0 => 1 ) - TypeError Unsupported operand type array for bit shift right
array ( ) >> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bit shift right
array ( ) >> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bit shift right
array ( ) >> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bit shift right
array ( ) >> (object) array ( ) - TypeError Unsupported operand type array for bit shift right
array ( ) >> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bit shift right
array ( ) >> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bit shift right
array ( ) >> DateTime - TypeError Unsupported operand type array for bit shift right
array ( ) >> resource - TypeError Unsupported operand type array for bit shift right
array ( ) >> NULL - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1 ) >> false - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1 ) >> true - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1 ) >> 0 - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1 ) >> 10 - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1 ) >> 0.0 - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1 ) >> 10.0 - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1 ) >> 3.14 - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1 ) >> '0' - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1 ) >> '10' - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1 ) >> '010' - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1 ) >> '10 elephants' - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1 ) >> 'foo' - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1 ) >> array ( ) - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1 ) >> array ( 0 => 1 ) - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1 ) >> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1 ) >> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1 ) >> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1 ) >> (object) array ( ) - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1 ) >> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1 ) >> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1 ) >> DateTime - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1 ) >> resource - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1 ) >> NULL - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1, 1 => 100 ) >> false - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1, 1 => 100 ) >> true - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1, 1 => 100 ) >> 0 - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1, 1 => 100 ) >> 10 - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1, 1 => 100 ) >> 0.0 - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1, 1 => 100 ) >> 10.0 - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1, 1 => 100 ) >> 3.14 - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1, 1 => 100 ) >> '0' - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1, 1 => 100 ) >> '10' - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1, 1 => 100 ) >> '010' - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1, 1 => 100 ) >> '10 elephants' - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1, 1 => 100 ) >> 'foo' - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1, 1 => 100 ) >> array ( ) - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1, 1 => 100 ) >> array ( 0 => 1 ) - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1, 1 => 100 ) >> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1, 1 => 100 ) >> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1, 1 => 100 ) >> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1, 1 => 100 ) >> (object) array ( ) - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1, 1 => 100 ) >> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1, 1 => 100 ) >> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1, 1 => 100 ) >> DateTime - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1, 1 => 100 ) >> resource - TypeError Unsupported operand type array for bit shift right
array ( 0 => 1, 1 => 100 ) >> NULL - TypeError Unsupported operand type array for bit shift right
array ( 'foo' => 1, 'bar' => 2 ) >> false - TypeError Unsupported operand type array for bit shift right
array ( 'foo' => 1, 'bar' => 2 ) >> true - TypeError Unsupported operand type array for bit shift right
array ( 'foo' => 1, 'bar' => 2 ) >> 0 - TypeError Unsupported operand type array for bit shift right
array ( 'foo' => 1, 'bar' => 2 ) >> 10 - TypeError Unsupported operand type array for bit shift right
array ( 'foo' => 1, 'bar' => 2 ) >> 0.0 - TypeError Unsupported operand type array for bit shift right
array ( 'foo' => 1, 'bar' => 2 ) >> 10.0 - TypeError Unsupported operand type array for bit shift right
array ( 'foo' => 1, 'bar' => 2 ) >> 3.14 - TypeError Unsupported operand type array for bit shift right
array ( 'foo' => 1, 'bar' => 2 ) >> '0' - TypeError Unsupported operand type array for bit shift right
array ( 'foo' => 1, 'bar' => 2 ) >> '10' - TypeError Unsupported operand type array for bit shift right
array ( 'foo' => 1, 'bar' => 2 ) >> '010' - TypeError Unsupported operand type array for bit shift right
array ( 'foo' => 1, 'bar' => 2 ) >> '10 elephants' - TypeError Unsupported operand type array for bit shift right
array ( 'foo' => 1, 'bar' => 2 ) >> 'foo' - TypeError Unsupported operand type array for bit shift right
array ( 'foo' => 1, 'bar' => 2 ) >> array ( ) - TypeError Unsupported operand type array for bit shift right
array ( 'foo' => 1, 'bar' => 2 ) >> array ( 0 => 1 ) - TypeError Unsupported operand type array for bit shift right
array ( 'foo' => 1, 'bar' => 2 ) >> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bit shift right
array ( 'foo' => 1, 'bar' => 2 ) >> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bit shift right
array ( 'foo' => 1, 'bar' => 2 ) >> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bit shift right
array ( 'foo' => 1, 'bar' => 2 ) >> (object) array ( ) - TypeError Unsupported operand type array for bit shift right
array ( 'foo' => 1, 'bar' => 2 ) >> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bit shift right
array ( 'foo' => 1, 'bar' => 2 ) >> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bit shift right
array ( 'foo' => 1, 'bar' => 2 ) >> DateTime - TypeError Unsupported operand type array for bit shift right
array ( 'foo' => 1, 'bar' => 2 ) >> resource - TypeError Unsupported operand type array for bit shift right
array ( 'foo' => 1, 'bar' => 2 ) >> NULL - TypeError Unsupported operand type array for bit shift right
array ( 'bar' => 1, 'foo' => 2 ) >> false - TypeError Unsupported operand type array for bit shift right
array ( 'bar' => 1, 'foo' => 2 ) >> true - TypeError Unsupported operand type array for bit shift right
array ( 'bar' => 1, 'foo' => 2 ) >> 0 - TypeError Unsupported operand type array for bit shift right
array ( 'bar' => 1, 'foo' => 2 ) >> 10 - TypeError Unsupported operand type array for bit shift right
array ( 'bar' => 1, 'foo' => 2 ) >> 0.0 - TypeError Unsupported operand type array for bit shift right
array ( 'bar' => 1, 'foo' => 2 ) >> 10.0 - TypeError Unsupported operand type array for bit shift right
array ( 'bar' => 1, 'foo' => 2 ) >> 3.14 - TypeError Unsupported operand type array for bit shift right
array ( 'bar' => 1, 'foo' => 2 ) >> '0' - TypeError Unsupported operand type array for bit shift right
array ( 'bar' => 1, 'foo' => 2 ) >> '10' - TypeError Unsupported operand type array for bit shift right
array ( 'bar' => 1, 'foo' => 2 ) >> '010' - TypeError Unsupported operand type array for bit shift right
array ( 'bar' => 1, 'foo' => 2 ) >> '10 elephants' - TypeError Unsupported operand type array for bit shift right
array ( 'bar' => 1, 'foo' => 2 ) >> 'foo' - TypeError Unsupported operand type array for bit shift right
array ( 'bar' => 1, 'foo' => 2 ) >> array ( ) - TypeError Unsupported operand type array for bit shift right
array ( 'bar' => 1, 'foo' => 2 ) >> array ( 0 => 1 ) - TypeError Unsupported operand type array for bit shift right
array ( 'bar' => 1, 'foo' => 2 ) >> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bit shift right
array ( 'bar' => 1, 'foo' => 2 ) >> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bit shift right
array ( 'bar' => 1, 'foo' => 2 ) >> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bit shift right
array ( 'bar' => 1, 'foo' => 2 ) >> (object) array ( ) - TypeError Unsupported operand type array for bit shift right
array ( 'bar' => 1, 'foo' => 2 ) >> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bit shift right
array ( 'bar' => 1, 'foo' => 2 ) >> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bit shift right
array ( 'bar' => 1, 'foo' => 2 ) >> DateTime - TypeError Unsupported operand type array for bit shift right
array ( 'bar' => 1, 'foo' => 2 ) >> resource - TypeError Unsupported operand type array for bit shift right
array ( 'bar' => 1, 'foo' => 2 ) >> NULL - TypeError Unsupported operand type array for bit shift right
(object) array ( ) >> false - TypeError Unsupported operand type object for bit shift right
(object) array ( ) >> true - TypeError Unsupported operand type object for bit shift right
(object) array ( ) >> 0 - TypeError Unsupported operand type object for bit shift right
(object) array ( ) >> 10 - TypeError Unsupported operand type object for bit shift right
(object) array ( ) >> 0.0 - TypeError Unsupported operand type object for bit shift right
(object) array ( ) >> 10.0 - TypeError Unsupported operand type object for bit shift right
(object) array ( ) >> 3.14 - TypeError Unsupported operand type object for bit shift right
(object) array ( ) >> '0' - TypeError Unsupported operand type object for bit shift right
(object) array ( ) >> '10' - TypeError Unsupported operand type object for bit shift right
(object) array ( ) >> '010' - TypeError Unsupported operand type object for bit shift right
(object) array ( ) >> '10 elephants' - TypeError Unsupported operand type object for bit shift right
(object) array ( ) >> 'foo' - TypeError Unsupported operand type object for bit shift right
(object) array ( ) >> array ( ) - TypeError Unsupported operand type object for bit shift right
(object) array ( ) >> array ( 0 => 1 ) - TypeError Unsupported operand type object for bit shift right
(object) array ( ) >> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for bit shift right
(object) array ( ) >> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bit shift right
(object) array ( ) >> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bit shift right
(object) array ( ) >> (object) array ( ) - TypeError Unsupported operand type object for bit shift right
(object) array ( ) >> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bit shift right
(object) array ( ) >> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bit shift right
(object) array ( ) >> DateTime - TypeError Unsupported operand type object for bit shift right
(object) array ( ) >> resource - TypeError Unsupported operand type object for bit shift right
(object) array ( ) >> NULL - TypeError Unsupported operand type object for bit shift right
(object) array ( 'foo' => 1, 'bar' => 2 ) >> false - TypeError Unsupported operand type object for bit shift right
(object) array ( 'foo' => 1, 'bar' => 2 ) >> true - TypeError Unsupported operand type object for bit shift right
(object) array ( 'foo' => 1, 'bar' => 2 ) >> 0 - TypeError Unsupported operand type object for bit shift right
(object) array ( 'foo' => 1, 'bar' => 2 ) >> 10 - TypeError Unsupported operand type object for bit shift right
(object) array ( 'foo' => 1, 'bar' => 2 ) >> 0.0 - TypeError Unsupported operand type object for bit shift right
(object) array ( 'foo' => 1, 'bar' => 2 ) >> 10.0 - TypeError Unsupported operand type object for bit shift right
(object) array ( 'foo' => 1, 'bar' => 2 ) >> 3.14 - TypeError Unsupported operand type object for bit shift right
(object) array ( 'foo' => 1, 'bar' => 2 ) >> '0' - TypeError Unsupported operand type object for bit shift right
(object) array ( 'foo' => 1, 'bar' => 2 ) >> '10' - TypeError Unsupported operand type object for bit shift right
(object) array ( 'foo' => 1, 'bar' => 2 ) >> '010' - TypeError Unsupported operand type object for bit shift right
(object) array ( 'foo' => 1, 'bar' => 2 ) >> '10 elephants' - TypeError Unsupported operand type object for bit shift right
(object) array ( 'foo' => 1, 'bar' => 2 ) >> 'foo' - TypeError Unsupported operand type object for bit shift right
(object) array ( 'foo' => 1, 'bar' => 2 ) >> array ( ) - TypeError Unsupported operand type object for bit shift right
(object) array ( 'foo' => 1, 'bar' => 2 ) >> array ( 0 => 1 ) - TypeError Unsupported operand type object for bit shift right
(object) array ( 'foo' => 1, 'bar' => 2 ) >> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for bit shift right
(object) array ( 'foo' => 1, 'bar' => 2 ) >> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bit shift right
(object) array ( 'foo' => 1, 'bar' => 2 ) >> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bit shift right
(object) array ( 'foo' => 1, 'bar' => 2 ) >> (object) array ( ) - TypeError Unsupported operand type object for bit shift right
(object) array ( 'foo' => 1, 'bar' => 2 ) >> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bit shift right
(object) array ( 'foo' => 1, 'bar' => 2 ) >> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bit shift right
(object) array ( 'foo' => 1, 'bar' => 2 ) >> DateTime - TypeError Unsupported operand type object for bit shift right
(object) array ( 'foo' => 1, 'bar' => 2 ) >> resource - TypeError Unsupported operand type object for bit shift right
(object) array ( 'foo' => 1, 'bar' => 2 ) >> NULL - TypeError Unsupported operand type object for bit shift right
(object) array ( 'bar' => 1, 'foo' => 2 ) >> false - TypeError Unsupported operand type object for bit shift right
(object) array ( 'bar' => 1, 'foo' => 2 ) >> true - TypeError Unsupported operand type object for bit shift right
(object) array ( 'bar' => 1, 'foo' => 2 ) >> 0 - TypeError Unsupported operand type object for bit shift right
(object) array ( 'bar' => 1, 'foo' => 2 ) >> 10 - TypeError Unsupported operand type object for bit shift right
(object) array ( 'bar' => 1, 'foo' => 2 ) >> 0.0 - TypeError Unsupported operand type object for bit shift right
(object) array ( 'bar' => 1, 'foo' => 2 ) >> 10.0 - TypeError Unsupported operand type object for bit shift right
(object) array ( 'bar' => 1, 'foo' => 2 ) >> 3.14 - TypeError Unsupported operand type object for bit shift right
(object) array ( 'bar' => 1, 'foo' => 2 ) >> '0' - TypeError Unsupported operand type object for bit shift right
(object) array ( 'bar' => 1, 'foo' => 2 ) >> '10' - TypeError Unsupported operand type object for bit shift right
(object) array ( 'bar' => 1, 'foo' => 2 ) >> '010' - TypeError Unsupported operand type object for bit shift right
(object) array ( 'bar' => 1, 'foo' => 2 ) >> '10 elephants' - TypeError Unsupported operand type object for bit shift right
(object) array ( 'bar' => 1, 'foo' => 2 ) >> 'foo' - TypeError Unsupported operand type object for bit shift right
(object) array ( 'bar' => 1, 'foo' => 2 ) >> array ( ) - TypeError Unsupported operand type object for bit shift right
(object) array ( 'bar' => 1, 'foo' => 2 ) >> array ( 0 => 1 ) - TypeError Unsupported operand type object for bit shift right
(object) array ( 'bar' => 1, 'foo' => 2 ) >> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for bit shift right
(object) array ( 'bar' => 1, 'foo' => 2 ) >> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bit shift right
(object) array ( 'bar' => 1, 'foo' => 2 ) >> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bit shift right
(object) array ( 'bar' => 1, 'foo' => 2 ) >> (object) array ( ) - TypeError Unsupported operand type object for bit shift right
(object) array ( 'bar' => 1, 'foo' => 2 ) >> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bit shift right
(object) array ( 'bar' => 1, 'foo' => 2 ) >> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bit shift right
(object) array ( 'bar' => 1, 'foo' => 2 ) >> DateTime - TypeError Unsupported operand type object for bit shift right
(object) array ( 'bar' => 1, 'foo' => 2 ) >> resource - TypeError Unsupported operand type object for bit shift right
(object) array ( 'bar' => 1, 'foo' => 2 ) >> NULL - TypeError Unsupported operand type object for bit shift right
DateTime >> false - TypeError Unsupported operand type object for bit shift right
DateTime >> true - TypeError Unsupported operand type object for bit shift right
DateTime >> 0 - TypeError Unsupported operand type object for bit shift right
DateTime >> 10 - TypeError Unsupported operand type object for bit shift right
DateTime >> 0.0 - TypeError Unsupported operand type object for bit shift right
DateTime >> 10.0 - TypeError Unsupported operand type object for bit shift right
DateTime >> 3.14 - TypeError Unsupported operand type object for bit shift right
DateTime >> '0' - TypeError Unsupported operand type object for bit shift right
DateTime >> '10' - TypeError Unsupported operand type object for bit shift right
DateTime >> '010' - TypeError Unsupported operand type object for bit shift right
DateTime >> '10 elephants' - TypeError Unsupported operand type object for bit shift right
DateTime >> 'foo' - TypeError Unsupported operand type object for bit shift right
DateTime >> array ( ) - TypeError Unsupported operand type object for bit shift right
DateTime >> array ( 0 => 1 ) - TypeError Unsupported operand type object for bit shift right
DateTime >> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for bit shift right
DateTime >> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bit shift right
DateTime >> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bit shift right
DateTime >> (object) array ( ) - TypeError Unsupported operand type object for bit shift right
DateTime >> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bit shift right
DateTime >> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bit shift right
DateTime >> DateTime - TypeError Unsupported operand type object for bit shift right
DateTime >> resource - TypeError Unsupported operand type object for bit shift right
DateTime >> NULL - TypeError Unsupported operand type object for bit shift right
resource >> false - TypeError Unsupported operand type resource for bit shift right
resource >> true - TypeError Unsupported operand type resource for bit shift right
resource >> 0 - TypeError Unsupported operand type resource for bit shift right
resource >> 10 - TypeError Unsupported operand type resource for bit shift right
resource >> 0.0 - TypeError Unsupported operand type resource for bit shift right
resource >> 10.0 - TypeError Unsupported operand type resource for bit shift right
resource >> 3.14 - TypeError Unsupported operand type resource for bit shift right
resource >> '0' - TypeError Unsupported operand type resource for bit shift right
resource >> '10' - TypeError Unsupported operand type resource for bit shift right
resource >> '010' - TypeError Unsupported operand type resource for bit shift right
resource >> '10 elephants' - TypeError Unsupported operand type resource for bit shift right
resource >> 'foo' - TypeError Unsupported operand type resource for bit shift right
resource >> array ( ) - TypeError Unsupported operand type resource for bit shift right
resource >> array ( 0 => 1 ) - TypeError Unsupported operand type resource for bit shift right
resource >> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type resource for bit shift right
resource >> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for bit shift right
resource >> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for bit shift right
resource >> (object) array ( ) - TypeError Unsupported operand type resource for bit shift right
resource >> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for bit shift right
resource >> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for bit shift right
resource >> DateTime - TypeError Unsupported operand type resource for bit shift right
resource >> resource - TypeError Unsupported operand type resource for bit shift right
resource >> NULL - TypeError Unsupported operand type resource for bit shift right
NULL >> false - TypeError Unsupported operand type null for bit shift right
NULL >> true - TypeError Unsupported operand type null for bit shift right
NULL >> 0 - TypeError Unsupported operand type null for bit shift right
NULL >> 10 - TypeError Unsupported operand type null for bit shift right
NULL >> 0.0 - TypeError Unsupported operand type null for bit shift right
NULL >> 10.0 - TypeError Unsupported operand type null for bit shift right
NULL >> 3.14 - TypeError Unsupported operand type null for bit shift right
NULL >> '0' - TypeError Unsupported operand type null for bit shift right
NULL >> '10' - TypeError Unsupported operand type null for bit shift right
NULL >> '010' - TypeError Unsupported operand type null for bit shift right
NULL >> '10 elephants' - TypeError Unsupported operand type null for bit shift right
NULL >> 'foo' - TypeError Unsupported operand type null for bit shift right
NULL >> array ( ) - TypeError Unsupported operand type null for bit shift right
NULL >> array ( 0 => 1 ) - TypeError Unsupported operand type null for bit shift right
NULL >> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type null for bit shift right
NULL >> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for bit shift right
NULL >> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for bit shift right
NULL >> (object) array ( ) - TypeError Unsupported operand type null for bit shift right
NULL >> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for bit shift right
NULL >> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for bit shift right
NULL >> DateTime - TypeError Unsupported operand type null for bit shift right
NULL >> resource - TypeError Unsupported operand type null for bit shift right
NULL >> NULL - TypeError Unsupported operand type null for bit shift right