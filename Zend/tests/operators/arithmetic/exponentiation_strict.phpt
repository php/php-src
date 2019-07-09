--TEST--
exponentiation '**' operator with strict_operators
--INI--
precision=14
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a ** $b', function($a, $b) { return $a ** $b; });

--EXPECT--
false ** false - TypeError Unsupported operand type bool for exponentiation
false ** true - TypeError Unsupported operand type bool for exponentiation
false ** 0 - TypeError Unsupported operand type bool for exponentiation
false ** 10 - TypeError Unsupported operand type bool for exponentiation
false ** 0.0 - TypeError Unsupported operand type bool for exponentiation
false ** 10.0 - TypeError Unsupported operand type bool for exponentiation
false ** 3.14 - TypeError Unsupported operand type bool for exponentiation
false ** '0' - TypeError Unsupported operand type bool for exponentiation
false ** '10' - TypeError Unsupported operand type bool for exponentiation
false ** '010' - TypeError Unsupported operand type bool for exponentiation
false ** '10 elephants' - TypeError Unsupported operand type bool for exponentiation
false ** 'foo' - TypeError Unsupported operand type bool for exponentiation
false ** array ( ) - TypeError Unsupported operand type bool for exponentiation
false ** array ( 0 => 1 ) - TypeError Unsupported operand type bool for exponentiation
false ** array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for exponentiation
false ** array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for exponentiation
false ** array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for exponentiation
false ** (object) array ( ) - TypeError Unsupported operand type bool for exponentiation
false ** (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for exponentiation
false ** (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for exponentiation
false ** DateTime - TypeError Unsupported operand type bool for exponentiation
false ** resource - TypeError Unsupported operand type bool for exponentiation
false ** NULL - TypeError Unsupported operand type bool for exponentiation
true ** false - TypeError Unsupported operand type bool for exponentiation
true ** true - TypeError Unsupported operand type bool for exponentiation
true ** 0 - TypeError Unsupported operand type bool for exponentiation
true ** 10 - TypeError Unsupported operand type bool for exponentiation
true ** 0.0 - TypeError Unsupported operand type bool for exponentiation
true ** 10.0 - TypeError Unsupported operand type bool for exponentiation
true ** 3.14 - TypeError Unsupported operand type bool for exponentiation
true ** '0' - TypeError Unsupported operand type bool for exponentiation
true ** '10' - TypeError Unsupported operand type bool for exponentiation
true ** '010' - TypeError Unsupported operand type bool for exponentiation
true ** '10 elephants' - TypeError Unsupported operand type bool for exponentiation
true ** 'foo' - TypeError Unsupported operand type bool for exponentiation
true ** array ( ) - TypeError Unsupported operand type bool for exponentiation
true ** array ( 0 => 1 ) - TypeError Unsupported operand type bool for exponentiation
true ** array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for exponentiation
true ** array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for exponentiation
true ** array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for exponentiation
true ** (object) array ( ) - TypeError Unsupported operand type bool for exponentiation
true ** (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for exponentiation
true ** (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for exponentiation
true ** DateTime - TypeError Unsupported operand type bool for exponentiation
true ** resource - TypeError Unsupported operand type bool for exponentiation
true ** NULL - TypeError Unsupported operand type bool for exponentiation
0 ** false - TypeError Unsupported operand type bool for exponentiation
0 ** true - TypeError Unsupported operand type bool for exponentiation
0 ** 0 = 1
0 ** 10 = 0
0 ** 0.0 = 1.0
0 ** 10.0 = 0.0
0 ** 3.14 = 0.0
0 ** '0' - TypeError Unsupported operand type string for exponentiation
0 ** '10' - TypeError Unsupported operand type string for exponentiation
0 ** '010' - TypeError Unsupported operand type string for exponentiation
0 ** '10 elephants' - TypeError Unsupported operand type string for exponentiation
0 ** 'foo' - TypeError Unsupported operand type string for exponentiation
0 ** array ( ) - TypeError Unsupported operand type array for exponentiation
0 ** array ( 0 => 1 ) - TypeError Unsupported operand type array for exponentiation
0 ** array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for exponentiation
0 ** array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for exponentiation
0 ** array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for exponentiation
0 ** (object) array ( ) - TypeError Unsupported operand type object for exponentiation
0 ** (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for exponentiation
0 ** (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for exponentiation
0 ** DateTime - TypeError Unsupported operand type object for exponentiation
0 ** resource - TypeError Unsupported operand type resource for exponentiation
0 ** NULL - TypeError Unsupported operand type null for exponentiation
10 ** false - TypeError Unsupported operand type bool for exponentiation
10 ** true - TypeError Unsupported operand type bool for exponentiation
10 ** 0 = 1
10 ** 10 = 10000000000
10 ** 0.0 = 1.0
10 ** 10.0 = 10000000000.0
10 ** 3.14 = 1380.3842646028852
10 ** '0' - TypeError Unsupported operand type string for exponentiation
10 ** '10' - TypeError Unsupported operand type string for exponentiation
10 ** '010' - TypeError Unsupported operand type string for exponentiation
10 ** '10 elephants' - TypeError Unsupported operand type string for exponentiation
10 ** 'foo' - TypeError Unsupported operand type string for exponentiation
10 ** array ( ) - TypeError Unsupported operand type array for exponentiation
10 ** array ( 0 => 1 ) - TypeError Unsupported operand type array for exponentiation
10 ** array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for exponentiation
10 ** array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for exponentiation
10 ** array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for exponentiation
10 ** (object) array ( ) - TypeError Unsupported operand type object for exponentiation
10 ** (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for exponentiation
10 ** (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for exponentiation
10 ** DateTime - TypeError Unsupported operand type object for exponentiation
10 ** resource - TypeError Unsupported operand type resource for exponentiation
10 ** NULL - TypeError Unsupported operand type null for exponentiation
0.0 ** false - TypeError Unsupported operand type bool for exponentiation
0.0 ** true - TypeError Unsupported operand type bool for exponentiation
0.0 ** 0 = 1.0
0.0 ** 10 = 0.0
0.0 ** 0.0 = 1.0
0.0 ** 10.0 = 0.0
0.0 ** 3.14 = 0.0
0.0 ** '0' - TypeError Unsupported operand type string for exponentiation
0.0 ** '10' - TypeError Unsupported operand type string for exponentiation
0.0 ** '010' - TypeError Unsupported operand type string for exponentiation
0.0 ** '10 elephants' - TypeError Unsupported operand type string for exponentiation
0.0 ** 'foo' - TypeError Unsupported operand type string for exponentiation
0.0 ** array ( ) - TypeError Unsupported operand type array for exponentiation
0.0 ** array ( 0 => 1 ) - TypeError Unsupported operand type array for exponentiation
0.0 ** array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for exponentiation
0.0 ** array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for exponentiation
0.0 ** array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for exponentiation
0.0 ** (object) array ( ) - TypeError Unsupported operand type object for exponentiation
0.0 ** (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for exponentiation
0.0 ** (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for exponentiation
0.0 ** DateTime - TypeError Unsupported operand type object for exponentiation
0.0 ** resource - TypeError Unsupported operand type resource for exponentiation
0.0 ** NULL - TypeError Unsupported operand type null for exponentiation
10.0 ** false - TypeError Unsupported operand type bool for exponentiation
10.0 ** true - TypeError Unsupported operand type bool for exponentiation
10.0 ** 0 = 1.0
10.0 ** 10 = 10000000000.0
10.0 ** 0.0 = 1.0
10.0 ** 10.0 = 10000000000.0
10.0 ** 3.14 = 1380.3842646028852
10.0 ** '0' - TypeError Unsupported operand type string for exponentiation
10.0 ** '10' - TypeError Unsupported operand type string for exponentiation
10.0 ** '010' - TypeError Unsupported operand type string for exponentiation
10.0 ** '10 elephants' - TypeError Unsupported operand type string for exponentiation
10.0 ** 'foo' - TypeError Unsupported operand type string for exponentiation
10.0 ** array ( ) - TypeError Unsupported operand type array for exponentiation
10.0 ** array ( 0 => 1 ) - TypeError Unsupported operand type array for exponentiation
10.0 ** array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for exponentiation
10.0 ** array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for exponentiation
10.0 ** array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for exponentiation
10.0 ** (object) array ( ) - TypeError Unsupported operand type object for exponentiation
10.0 ** (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for exponentiation
10.0 ** (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for exponentiation
10.0 ** DateTime - TypeError Unsupported operand type object for exponentiation
10.0 ** resource - TypeError Unsupported operand type resource for exponentiation
10.0 ** NULL - TypeError Unsupported operand type null for exponentiation
3.14 ** false - TypeError Unsupported operand type bool for exponentiation
3.14 ** true - TypeError Unsupported operand type bool for exponentiation
3.14 ** 0 = 1.0
3.14 ** 10 = 93174.3733866435
3.14 ** 0.0 = 1.0
3.14 ** 10.0 = 93174.3733866435
3.14 ** 3.14 = 36.33783888017471
3.14 ** '0' - TypeError Unsupported operand type string for exponentiation
3.14 ** '10' - TypeError Unsupported operand type string for exponentiation
3.14 ** '010' - TypeError Unsupported operand type string for exponentiation
3.14 ** '10 elephants' - TypeError Unsupported operand type string for exponentiation
3.14 ** 'foo' - TypeError Unsupported operand type string for exponentiation
3.14 ** array ( ) - TypeError Unsupported operand type array for exponentiation
3.14 ** array ( 0 => 1 ) - TypeError Unsupported operand type array for exponentiation
3.14 ** array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for exponentiation
3.14 ** array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for exponentiation
3.14 ** array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for exponentiation
3.14 ** (object) array ( ) - TypeError Unsupported operand type object for exponentiation
3.14 ** (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for exponentiation
3.14 ** (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for exponentiation
3.14 ** DateTime - TypeError Unsupported operand type object for exponentiation
3.14 ** resource - TypeError Unsupported operand type resource for exponentiation
3.14 ** NULL - TypeError Unsupported operand type null for exponentiation
'0' ** false - TypeError Unsupported operand type string for exponentiation
'0' ** true - TypeError Unsupported operand type string for exponentiation
'0' ** 0 - TypeError Unsupported operand type string for exponentiation
'0' ** 10 - TypeError Unsupported operand type string for exponentiation
'0' ** 0.0 - TypeError Unsupported operand type string for exponentiation
'0' ** 10.0 - TypeError Unsupported operand type string for exponentiation
'0' ** 3.14 - TypeError Unsupported operand type string for exponentiation
'0' ** '0' - TypeError Unsupported operand type string for exponentiation
'0' ** '10' - TypeError Unsupported operand type string for exponentiation
'0' ** '010' - TypeError Unsupported operand type string for exponentiation
'0' ** '10 elephants' - TypeError Unsupported operand type string for exponentiation
'0' ** 'foo' - TypeError Unsupported operand type string for exponentiation
'0' ** array ( ) - TypeError Unsupported operand type string for exponentiation
'0' ** array ( 0 => 1 ) - TypeError Unsupported operand type string for exponentiation
'0' ** array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for exponentiation
'0' ** array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for exponentiation
'0' ** array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for exponentiation
'0' ** (object) array ( ) - TypeError Unsupported operand type string for exponentiation
'0' ** (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for exponentiation
'0' ** (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for exponentiation
'0' ** DateTime - TypeError Unsupported operand type string for exponentiation
'0' ** resource - TypeError Unsupported operand type string for exponentiation
'0' ** NULL - TypeError Unsupported operand type string for exponentiation
'10' ** false - TypeError Unsupported operand type string for exponentiation
'10' ** true - TypeError Unsupported operand type string for exponentiation
'10' ** 0 - TypeError Unsupported operand type string for exponentiation
'10' ** 10 - TypeError Unsupported operand type string for exponentiation
'10' ** 0.0 - TypeError Unsupported operand type string for exponentiation
'10' ** 10.0 - TypeError Unsupported operand type string for exponentiation
'10' ** 3.14 - TypeError Unsupported operand type string for exponentiation
'10' ** '0' - TypeError Unsupported operand type string for exponentiation
'10' ** '10' - TypeError Unsupported operand type string for exponentiation
'10' ** '010' - TypeError Unsupported operand type string for exponentiation
'10' ** '10 elephants' - TypeError Unsupported operand type string for exponentiation
'10' ** 'foo' - TypeError Unsupported operand type string for exponentiation
'10' ** array ( ) - TypeError Unsupported operand type string for exponentiation
'10' ** array ( 0 => 1 ) - TypeError Unsupported operand type string for exponentiation
'10' ** array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for exponentiation
'10' ** array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for exponentiation
'10' ** array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for exponentiation
'10' ** (object) array ( ) - TypeError Unsupported operand type string for exponentiation
'10' ** (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for exponentiation
'10' ** (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for exponentiation
'10' ** DateTime - TypeError Unsupported operand type string for exponentiation
'10' ** resource - TypeError Unsupported operand type string for exponentiation
'10' ** NULL - TypeError Unsupported operand type string for exponentiation
'010' ** false - TypeError Unsupported operand type string for exponentiation
'010' ** true - TypeError Unsupported operand type string for exponentiation
'010' ** 0 - TypeError Unsupported operand type string for exponentiation
'010' ** 10 - TypeError Unsupported operand type string for exponentiation
'010' ** 0.0 - TypeError Unsupported operand type string for exponentiation
'010' ** 10.0 - TypeError Unsupported operand type string for exponentiation
'010' ** 3.14 - TypeError Unsupported operand type string for exponentiation
'010' ** '0' - TypeError Unsupported operand type string for exponentiation
'010' ** '10' - TypeError Unsupported operand type string for exponentiation
'010' ** '010' - TypeError Unsupported operand type string for exponentiation
'010' ** '10 elephants' - TypeError Unsupported operand type string for exponentiation
'010' ** 'foo' - TypeError Unsupported operand type string for exponentiation
'010' ** array ( ) - TypeError Unsupported operand type string for exponentiation
'010' ** array ( 0 => 1 ) - TypeError Unsupported operand type string for exponentiation
'010' ** array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for exponentiation
'010' ** array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for exponentiation
'010' ** array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for exponentiation
'010' ** (object) array ( ) - TypeError Unsupported operand type string for exponentiation
'010' ** (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for exponentiation
'010' ** (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for exponentiation
'010' ** DateTime - TypeError Unsupported operand type string for exponentiation
'010' ** resource - TypeError Unsupported operand type string for exponentiation
'010' ** NULL - TypeError Unsupported operand type string for exponentiation
'10 elephants' ** false - TypeError Unsupported operand type string for exponentiation
'10 elephants' ** true - TypeError Unsupported operand type string for exponentiation
'10 elephants' ** 0 - TypeError Unsupported operand type string for exponentiation
'10 elephants' ** 10 - TypeError Unsupported operand type string for exponentiation
'10 elephants' ** 0.0 - TypeError Unsupported operand type string for exponentiation
'10 elephants' ** 10.0 - TypeError Unsupported operand type string for exponentiation
'10 elephants' ** 3.14 - TypeError Unsupported operand type string for exponentiation
'10 elephants' ** '0' - TypeError Unsupported operand type string for exponentiation
'10 elephants' ** '10' - TypeError Unsupported operand type string for exponentiation
'10 elephants' ** '010' - TypeError Unsupported operand type string for exponentiation
'10 elephants' ** '10 elephants' - TypeError Unsupported operand type string for exponentiation
'10 elephants' ** 'foo' - TypeError Unsupported operand type string for exponentiation
'10 elephants' ** array ( ) - TypeError Unsupported operand type string for exponentiation
'10 elephants' ** array ( 0 => 1 ) - TypeError Unsupported operand type string for exponentiation
'10 elephants' ** array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for exponentiation
'10 elephants' ** array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for exponentiation
'10 elephants' ** array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for exponentiation
'10 elephants' ** (object) array ( ) - TypeError Unsupported operand type string for exponentiation
'10 elephants' ** (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for exponentiation
'10 elephants' ** (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for exponentiation
'10 elephants' ** DateTime - TypeError Unsupported operand type string for exponentiation
'10 elephants' ** resource - TypeError Unsupported operand type string for exponentiation
'10 elephants' ** NULL - TypeError Unsupported operand type string for exponentiation
'foo' ** false - TypeError Unsupported operand type string for exponentiation
'foo' ** true - TypeError Unsupported operand type string for exponentiation
'foo' ** 0 - TypeError Unsupported operand type string for exponentiation
'foo' ** 10 - TypeError Unsupported operand type string for exponentiation
'foo' ** 0.0 - TypeError Unsupported operand type string for exponentiation
'foo' ** 10.0 - TypeError Unsupported operand type string for exponentiation
'foo' ** 3.14 - TypeError Unsupported operand type string for exponentiation
'foo' ** '0' - TypeError Unsupported operand type string for exponentiation
'foo' ** '10' - TypeError Unsupported operand type string for exponentiation
'foo' ** '010' - TypeError Unsupported operand type string for exponentiation
'foo' ** '10 elephants' - TypeError Unsupported operand type string for exponentiation
'foo' ** 'foo' - TypeError Unsupported operand type string for exponentiation
'foo' ** array ( ) - TypeError Unsupported operand type string for exponentiation
'foo' ** array ( 0 => 1 ) - TypeError Unsupported operand type string for exponentiation
'foo' ** array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for exponentiation
'foo' ** array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for exponentiation
'foo' ** array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for exponentiation
'foo' ** (object) array ( ) - TypeError Unsupported operand type string for exponentiation
'foo' ** (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for exponentiation
'foo' ** (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for exponentiation
'foo' ** DateTime - TypeError Unsupported operand type string for exponentiation
'foo' ** resource - TypeError Unsupported operand type string for exponentiation
'foo' ** NULL - TypeError Unsupported operand type string for exponentiation
array ( ) ** false - TypeError Unsupported operand type array for exponentiation
array ( ) ** true - TypeError Unsupported operand type array for exponentiation
array ( ) ** 0 - TypeError Unsupported operand type array for exponentiation
array ( ) ** 10 - TypeError Unsupported operand type array for exponentiation
array ( ) ** 0.0 - TypeError Unsupported operand type array for exponentiation
array ( ) ** 10.0 - TypeError Unsupported operand type array for exponentiation
array ( ) ** 3.14 - TypeError Unsupported operand type array for exponentiation
array ( ) ** '0' - TypeError Unsupported operand type array for exponentiation
array ( ) ** '10' - TypeError Unsupported operand type array for exponentiation
array ( ) ** '010' - TypeError Unsupported operand type array for exponentiation
array ( ) ** '10 elephants' - TypeError Unsupported operand type array for exponentiation
array ( ) ** 'foo' - TypeError Unsupported operand type array for exponentiation
array ( ) ** array ( ) - TypeError Unsupported operand type array for exponentiation
array ( ) ** array ( 0 => 1 ) - TypeError Unsupported operand type array for exponentiation
array ( ) ** array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for exponentiation
array ( ) ** array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for exponentiation
array ( ) ** array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for exponentiation
array ( ) ** (object) array ( ) - TypeError Unsupported operand type array for exponentiation
array ( ) ** (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for exponentiation
array ( ) ** (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for exponentiation
array ( ) ** DateTime - TypeError Unsupported operand type array for exponentiation
array ( ) ** resource - TypeError Unsupported operand type array for exponentiation
array ( ) ** NULL - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1 ) ** false - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1 ) ** true - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1 ) ** 0 - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1 ) ** 10 - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1 ) ** 0.0 - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1 ) ** 10.0 - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1 ) ** 3.14 - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1 ) ** '0' - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1 ) ** '10' - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1 ) ** '010' - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1 ) ** '10 elephants' - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1 ) ** 'foo' - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1 ) ** array ( ) - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1 ) ** array ( 0 => 1 ) - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1 ) ** array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1 ) ** array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1 ) ** array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1 ) ** (object) array ( ) - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1 ) ** (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1 ) ** (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1 ) ** DateTime - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1 ) ** resource - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1 ) ** NULL - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1, 1 => 100 ) ** false - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1, 1 => 100 ) ** true - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1, 1 => 100 ) ** 0 - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1, 1 => 100 ) ** 10 - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1, 1 => 100 ) ** 0.0 - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1, 1 => 100 ) ** 10.0 - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1, 1 => 100 ) ** 3.14 - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1, 1 => 100 ) ** '0' - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1, 1 => 100 ) ** '10' - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1, 1 => 100 ) ** '010' - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1, 1 => 100 ) ** '10 elephants' - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1, 1 => 100 ) ** 'foo' - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1, 1 => 100 ) ** array ( ) - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1, 1 => 100 ) ** array ( 0 => 1 ) - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1, 1 => 100 ) ** array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1, 1 => 100 ) ** array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1, 1 => 100 ) ** array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1, 1 => 100 ) ** (object) array ( ) - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1, 1 => 100 ) ** (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1, 1 => 100 ) ** (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1, 1 => 100 ) ** DateTime - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1, 1 => 100 ) ** resource - TypeError Unsupported operand type array for exponentiation
array ( 0 => 1, 1 => 100 ) ** NULL - TypeError Unsupported operand type array for exponentiation
array ( 'foo' => 1, 'bar' => 2 ) ** false - TypeError Unsupported operand type array for exponentiation
array ( 'foo' => 1, 'bar' => 2 ) ** true - TypeError Unsupported operand type array for exponentiation
array ( 'foo' => 1, 'bar' => 2 ) ** 0 - TypeError Unsupported operand type array for exponentiation
array ( 'foo' => 1, 'bar' => 2 ) ** 10 - TypeError Unsupported operand type array for exponentiation
array ( 'foo' => 1, 'bar' => 2 ) ** 0.0 - TypeError Unsupported operand type array for exponentiation
array ( 'foo' => 1, 'bar' => 2 ) ** 10.0 - TypeError Unsupported operand type array for exponentiation
array ( 'foo' => 1, 'bar' => 2 ) ** 3.14 - TypeError Unsupported operand type array for exponentiation
array ( 'foo' => 1, 'bar' => 2 ) ** '0' - TypeError Unsupported operand type array for exponentiation
array ( 'foo' => 1, 'bar' => 2 ) ** '10' - TypeError Unsupported operand type array for exponentiation
array ( 'foo' => 1, 'bar' => 2 ) ** '010' - TypeError Unsupported operand type array for exponentiation
array ( 'foo' => 1, 'bar' => 2 ) ** '10 elephants' - TypeError Unsupported operand type array for exponentiation
array ( 'foo' => 1, 'bar' => 2 ) ** 'foo' - TypeError Unsupported operand type array for exponentiation
array ( 'foo' => 1, 'bar' => 2 ) ** array ( ) - TypeError Unsupported operand type array for exponentiation
array ( 'foo' => 1, 'bar' => 2 ) ** array ( 0 => 1 ) - TypeError Unsupported operand type array for exponentiation
array ( 'foo' => 1, 'bar' => 2 ) ** array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for exponentiation
array ( 'foo' => 1, 'bar' => 2 ) ** array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for exponentiation
array ( 'foo' => 1, 'bar' => 2 ) ** array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for exponentiation
array ( 'foo' => 1, 'bar' => 2 ) ** (object) array ( ) - TypeError Unsupported operand type array for exponentiation
array ( 'foo' => 1, 'bar' => 2 ) ** (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for exponentiation
array ( 'foo' => 1, 'bar' => 2 ) ** (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for exponentiation
array ( 'foo' => 1, 'bar' => 2 ) ** DateTime - TypeError Unsupported operand type array for exponentiation
array ( 'foo' => 1, 'bar' => 2 ) ** resource - TypeError Unsupported operand type array for exponentiation
array ( 'foo' => 1, 'bar' => 2 ) ** NULL - TypeError Unsupported operand type array for exponentiation
array ( 'bar' => 1, 'foo' => 2 ) ** false - TypeError Unsupported operand type array for exponentiation
array ( 'bar' => 1, 'foo' => 2 ) ** true - TypeError Unsupported operand type array for exponentiation
array ( 'bar' => 1, 'foo' => 2 ) ** 0 - TypeError Unsupported operand type array for exponentiation
array ( 'bar' => 1, 'foo' => 2 ) ** 10 - TypeError Unsupported operand type array for exponentiation
array ( 'bar' => 1, 'foo' => 2 ) ** 0.0 - TypeError Unsupported operand type array for exponentiation
array ( 'bar' => 1, 'foo' => 2 ) ** 10.0 - TypeError Unsupported operand type array for exponentiation
array ( 'bar' => 1, 'foo' => 2 ) ** 3.14 - TypeError Unsupported operand type array for exponentiation
array ( 'bar' => 1, 'foo' => 2 ) ** '0' - TypeError Unsupported operand type array for exponentiation
array ( 'bar' => 1, 'foo' => 2 ) ** '10' - TypeError Unsupported operand type array for exponentiation
array ( 'bar' => 1, 'foo' => 2 ) ** '010' - TypeError Unsupported operand type array for exponentiation
array ( 'bar' => 1, 'foo' => 2 ) ** '10 elephants' - TypeError Unsupported operand type array for exponentiation
array ( 'bar' => 1, 'foo' => 2 ) ** 'foo' - TypeError Unsupported operand type array for exponentiation
array ( 'bar' => 1, 'foo' => 2 ) ** array ( ) - TypeError Unsupported operand type array for exponentiation
array ( 'bar' => 1, 'foo' => 2 ) ** array ( 0 => 1 ) - TypeError Unsupported operand type array for exponentiation
array ( 'bar' => 1, 'foo' => 2 ) ** array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for exponentiation
array ( 'bar' => 1, 'foo' => 2 ) ** array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for exponentiation
array ( 'bar' => 1, 'foo' => 2 ) ** array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for exponentiation
array ( 'bar' => 1, 'foo' => 2 ) ** (object) array ( ) - TypeError Unsupported operand type array for exponentiation
array ( 'bar' => 1, 'foo' => 2 ) ** (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for exponentiation
array ( 'bar' => 1, 'foo' => 2 ) ** (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for exponentiation
array ( 'bar' => 1, 'foo' => 2 ) ** DateTime - TypeError Unsupported operand type array for exponentiation
array ( 'bar' => 1, 'foo' => 2 ) ** resource - TypeError Unsupported operand type array for exponentiation
array ( 'bar' => 1, 'foo' => 2 ) ** NULL - TypeError Unsupported operand type array for exponentiation
(object) array ( ) ** false - TypeError Unsupported operand type object for exponentiation
(object) array ( ) ** true - TypeError Unsupported operand type object for exponentiation
(object) array ( ) ** 0 - TypeError Unsupported operand type object for exponentiation
(object) array ( ) ** 10 - TypeError Unsupported operand type object for exponentiation
(object) array ( ) ** 0.0 - TypeError Unsupported operand type object for exponentiation
(object) array ( ) ** 10.0 - TypeError Unsupported operand type object for exponentiation
(object) array ( ) ** 3.14 - TypeError Unsupported operand type object for exponentiation
(object) array ( ) ** '0' - TypeError Unsupported operand type object for exponentiation
(object) array ( ) ** '10' - TypeError Unsupported operand type object for exponentiation
(object) array ( ) ** '010' - TypeError Unsupported operand type object for exponentiation
(object) array ( ) ** '10 elephants' - TypeError Unsupported operand type object for exponentiation
(object) array ( ) ** 'foo' - TypeError Unsupported operand type object for exponentiation
(object) array ( ) ** array ( ) - TypeError Unsupported operand type object for exponentiation
(object) array ( ) ** array ( 0 => 1 ) - TypeError Unsupported operand type object for exponentiation
(object) array ( ) ** array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for exponentiation
(object) array ( ) ** array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for exponentiation
(object) array ( ) ** array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for exponentiation
(object) array ( ) ** (object) array ( ) - TypeError Unsupported operand type object for exponentiation
(object) array ( ) ** (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for exponentiation
(object) array ( ) ** (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for exponentiation
(object) array ( ) ** DateTime - TypeError Unsupported operand type object for exponentiation
(object) array ( ) ** resource - TypeError Unsupported operand type object for exponentiation
(object) array ( ) ** NULL - TypeError Unsupported operand type object for exponentiation
(object) array ( 'foo' => 1, 'bar' => 2 ) ** false - TypeError Unsupported operand type object for exponentiation
(object) array ( 'foo' => 1, 'bar' => 2 ) ** true - TypeError Unsupported operand type object for exponentiation
(object) array ( 'foo' => 1, 'bar' => 2 ) ** 0 - TypeError Unsupported operand type object for exponentiation
(object) array ( 'foo' => 1, 'bar' => 2 ) ** 10 - TypeError Unsupported operand type object for exponentiation
(object) array ( 'foo' => 1, 'bar' => 2 ) ** 0.0 - TypeError Unsupported operand type object for exponentiation
(object) array ( 'foo' => 1, 'bar' => 2 ) ** 10.0 - TypeError Unsupported operand type object for exponentiation
(object) array ( 'foo' => 1, 'bar' => 2 ) ** 3.14 - TypeError Unsupported operand type object for exponentiation
(object) array ( 'foo' => 1, 'bar' => 2 ) ** '0' - TypeError Unsupported operand type object for exponentiation
(object) array ( 'foo' => 1, 'bar' => 2 ) ** '10' - TypeError Unsupported operand type object for exponentiation
(object) array ( 'foo' => 1, 'bar' => 2 ) ** '010' - TypeError Unsupported operand type object for exponentiation
(object) array ( 'foo' => 1, 'bar' => 2 ) ** '10 elephants' - TypeError Unsupported operand type object for exponentiation
(object) array ( 'foo' => 1, 'bar' => 2 ) ** 'foo' - TypeError Unsupported operand type object for exponentiation
(object) array ( 'foo' => 1, 'bar' => 2 ) ** array ( ) - TypeError Unsupported operand type object for exponentiation
(object) array ( 'foo' => 1, 'bar' => 2 ) ** array ( 0 => 1 ) - TypeError Unsupported operand type object for exponentiation
(object) array ( 'foo' => 1, 'bar' => 2 ) ** array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for exponentiation
(object) array ( 'foo' => 1, 'bar' => 2 ) ** array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for exponentiation
(object) array ( 'foo' => 1, 'bar' => 2 ) ** array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for exponentiation
(object) array ( 'foo' => 1, 'bar' => 2 ) ** (object) array ( ) - TypeError Unsupported operand type object for exponentiation
(object) array ( 'foo' => 1, 'bar' => 2 ) ** (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for exponentiation
(object) array ( 'foo' => 1, 'bar' => 2 ) ** (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for exponentiation
(object) array ( 'foo' => 1, 'bar' => 2 ) ** DateTime - TypeError Unsupported operand type object for exponentiation
(object) array ( 'foo' => 1, 'bar' => 2 ) ** resource - TypeError Unsupported operand type object for exponentiation
(object) array ( 'foo' => 1, 'bar' => 2 ) ** NULL - TypeError Unsupported operand type object for exponentiation
(object) array ( 'bar' => 1, 'foo' => 2 ) ** false - TypeError Unsupported operand type object for exponentiation
(object) array ( 'bar' => 1, 'foo' => 2 ) ** true - TypeError Unsupported operand type object for exponentiation
(object) array ( 'bar' => 1, 'foo' => 2 ) ** 0 - TypeError Unsupported operand type object for exponentiation
(object) array ( 'bar' => 1, 'foo' => 2 ) ** 10 - TypeError Unsupported operand type object for exponentiation
(object) array ( 'bar' => 1, 'foo' => 2 ) ** 0.0 - TypeError Unsupported operand type object for exponentiation
(object) array ( 'bar' => 1, 'foo' => 2 ) ** 10.0 - TypeError Unsupported operand type object for exponentiation
(object) array ( 'bar' => 1, 'foo' => 2 ) ** 3.14 - TypeError Unsupported operand type object for exponentiation
(object) array ( 'bar' => 1, 'foo' => 2 ) ** '0' - TypeError Unsupported operand type object for exponentiation
(object) array ( 'bar' => 1, 'foo' => 2 ) ** '10' - TypeError Unsupported operand type object for exponentiation
(object) array ( 'bar' => 1, 'foo' => 2 ) ** '010' - TypeError Unsupported operand type object for exponentiation
(object) array ( 'bar' => 1, 'foo' => 2 ) ** '10 elephants' - TypeError Unsupported operand type object for exponentiation
(object) array ( 'bar' => 1, 'foo' => 2 ) ** 'foo' - TypeError Unsupported operand type object for exponentiation
(object) array ( 'bar' => 1, 'foo' => 2 ) ** array ( ) - TypeError Unsupported operand type object for exponentiation
(object) array ( 'bar' => 1, 'foo' => 2 ) ** array ( 0 => 1 ) - TypeError Unsupported operand type object for exponentiation
(object) array ( 'bar' => 1, 'foo' => 2 ) ** array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for exponentiation
(object) array ( 'bar' => 1, 'foo' => 2 ) ** array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for exponentiation
(object) array ( 'bar' => 1, 'foo' => 2 ) ** array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for exponentiation
(object) array ( 'bar' => 1, 'foo' => 2 ) ** (object) array ( ) - TypeError Unsupported operand type object for exponentiation
(object) array ( 'bar' => 1, 'foo' => 2 ) ** (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for exponentiation
(object) array ( 'bar' => 1, 'foo' => 2 ) ** (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for exponentiation
(object) array ( 'bar' => 1, 'foo' => 2 ) ** DateTime - TypeError Unsupported operand type object for exponentiation
(object) array ( 'bar' => 1, 'foo' => 2 ) ** resource - TypeError Unsupported operand type object for exponentiation
(object) array ( 'bar' => 1, 'foo' => 2 ) ** NULL - TypeError Unsupported operand type object for exponentiation
DateTime ** false - TypeError Unsupported operand type object for exponentiation
DateTime ** true - TypeError Unsupported operand type object for exponentiation
DateTime ** 0 - TypeError Unsupported operand type object for exponentiation
DateTime ** 10 - TypeError Unsupported operand type object for exponentiation
DateTime ** 0.0 - TypeError Unsupported operand type object for exponentiation
DateTime ** 10.0 - TypeError Unsupported operand type object for exponentiation
DateTime ** 3.14 - TypeError Unsupported operand type object for exponentiation
DateTime ** '0' - TypeError Unsupported operand type object for exponentiation
DateTime ** '10' - TypeError Unsupported operand type object for exponentiation
DateTime ** '010' - TypeError Unsupported operand type object for exponentiation
DateTime ** '10 elephants' - TypeError Unsupported operand type object for exponentiation
DateTime ** 'foo' - TypeError Unsupported operand type object for exponentiation
DateTime ** array ( ) - TypeError Unsupported operand type object for exponentiation
DateTime ** array ( 0 => 1 ) - TypeError Unsupported operand type object for exponentiation
DateTime ** array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for exponentiation
DateTime ** array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for exponentiation
DateTime ** array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for exponentiation
DateTime ** (object) array ( ) - TypeError Unsupported operand type object for exponentiation
DateTime ** (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for exponentiation
DateTime ** (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for exponentiation
DateTime ** DateTime - TypeError Unsupported operand type object for exponentiation
DateTime ** resource - TypeError Unsupported operand type object for exponentiation
DateTime ** NULL - TypeError Unsupported operand type object for exponentiation
resource ** false - TypeError Unsupported operand type resource for exponentiation
resource ** true - TypeError Unsupported operand type resource for exponentiation
resource ** 0 - TypeError Unsupported operand type resource for exponentiation
resource ** 10 - TypeError Unsupported operand type resource for exponentiation
resource ** 0.0 - TypeError Unsupported operand type resource for exponentiation
resource ** 10.0 - TypeError Unsupported operand type resource for exponentiation
resource ** 3.14 - TypeError Unsupported operand type resource for exponentiation
resource ** '0' - TypeError Unsupported operand type resource for exponentiation
resource ** '10' - TypeError Unsupported operand type resource for exponentiation
resource ** '010' - TypeError Unsupported operand type resource for exponentiation
resource ** '10 elephants' - TypeError Unsupported operand type resource for exponentiation
resource ** 'foo' - TypeError Unsupported operand type resource for exponentiation
resource ** array ( ) - TypeError Unsupported operand type resource for exponentiation
resource ** array ( 0 => 1 ) - TypeError Unsupported operand type resource for exponentiation
resource ** array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type resource for exponentiation
resource ** array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for exponentiation
resource ** array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for exponentiation
resource ** (object) array ( ) - TypeError Unsupported operand type resource for exponentiation
resource ** (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for exponentiation
resource ** (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for exponentiation
resource ** DateTime - TypeError Unsupported operand type resource for exponentiation
resource ** resource - TypeError Unsupported operand type resource for exponentiation
resource ** NULL - TypeError Unsupported operand type resource for exponentiation
NULL ** false - TypeError Unsupported operand type null for exponentiation
NULL ** true - TypeError Unsupported operand type null for exponentiation
NULL ** 0 - TypeError Unsupported operand type null for exponentiation
NULL ** 10 - TypeError Unsupported operand type null for exponentiation
NULL ** 0.0 - TypeError Unsupported operand type null for exponentiation
NULL ** 10.0 - TypeError Unsupported operand type null for exponentiation
NULL ** 3.14 - TypeError Unsupported operand type null for exponentiation
NULL ** '0' - TypeError Unsupported operand type null for exponentiation
NULL ** '10' - TypeError Unsupported operand type null for exponentiation
NULL ** '010' - TypeError Unsupported operand type null for exponentiation
NULL ** '10 elephants' - TypeError Unsupported operand type null for exponentiation
NULL ** 'foo' - TypeError Unsupported operand type null for exponentiation
NULL ** array ( ) - TypeError Unsupported operand type null for exponentiation
NULL ** array ( 0 => 1 ) - TypeError Unsupported operand type null for exponentiation
NULL ** array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type null for exponentiation
NULL ** array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for exponentiation
NULL ** array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for exponentiation
NULL ** (object) array ( ) - TypeError Unsupported operand type null for exponentiation
NULL ** (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for exponentiation
NULL ** (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for exponentiation
NULL ** DateTime - TypeError Unsupported operand type null for exponentiation
NULL ** resource - TypeError Unsupported operand type null for exponentiation
NULL ** NULL - TypeError Unsupported operand type null for exponentiation