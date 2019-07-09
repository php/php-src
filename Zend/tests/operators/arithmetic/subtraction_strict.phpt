--TEST--
substraction '-' operator with strict_operators
--INI--
precision=14
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a - $b', function($a, $b) { return $a - $b; });

--EXPECT--
false - false - TypeError Unsupported operand type bool for subtraction
false - true - TypeError Unsupported operand type bool for subtraction
false - 0 - TypeError Unsupported operand type bool for subtraction
false - 10 - TypeError Unsupported operand type bool for subtraction
false - 0.0 - TypeError Unsupported operand type bool for subtraction
false - 10.0 - TypeError Unsupported operand type bool for subtraction
false - 3.14 - TypeError Unsupported operand type bool for subtraction
false - '0' - TypeError Unsupported operand type bool for subtraction
false - '10' - TypeError Unsupported operand type bool for subtraction
false - '010' - TypeError Unsupported operand type bool for subtraction
false - '10 elephants' - TypeError Unsupported operand type bool for subtraction
false - 'foo' - TypeError Unsupported operand type bool for subtraction
false - array ( ) - TypeError Unsupported operand type bool for subtraction
false - array ( 0 => 1 ) - TypeError Unsupported operand type bool for subtraction
false - array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for subtraction
false - array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for subtraction
false - array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for subtraction
false - (object) array ( ) - TypeError Unsupported operand type bool for subtraction
false - (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for subtraction
false - (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for subtraction
false - DateTime - TypeError Unsupported operand type bool for subtraction
false - resource - TypeError Unsupported operand type bool for subtraction
false - NULL - TypeError Unsupported operand type bool for subtraction
true - false - TypeError Unsupported operand type bool for subtraction
true - true - TypeError Unsupported operand type bool for subtraction
true - 0 - TypeError Unsupported operand type bool for subtraction
true - 10 - TypeError Unsupported operand type bool for subtraction
true - 0.0 - TypeError Unsupported operand type bool for subtraction
true - 10.0 - TypeError Unsupported operand type bool for subtraction
true - 3.14 - TypeError Unsupported operand type bool for subtraction
true - '0' - TypeError Unsupported operand type bool for subtraction
true - '10' - TypeError Unsupported operand type bool for subtraction
true - '010' - TypeError Unsupported operand type bool for subtraction
true - '10 elephants' - TypeError Unsupported operand type bool for subtraction
true - 'foo' - TypeError Unsupported operand type bool for subtraction
true - array ( ) - TypeError Unsupported operand type bool for subtraction
true - array ( 0 => 1 ) - TypeError Unsupported operand type bool for subtraction
true - array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for subtraction
true - array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for subtraction
true - array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for subtraction
true - (object) array ( ) - TypeError Unsupported operand type bool for subtraction
true - (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for subtraction
true - (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for subtraction
true - DateTime - TypeError Unsupported operand type bool for subtraction
true - resource - TypeError Unsupported operand type bool for subtraction
true - NULL - TypeError Unsupported operand type bool for subtraction
0 - false - TypeError Unsupported operand type bool for subtraction
0 - true - TypeError Unsupported operand type bool for subtraction
0 - 0 = 0
0 - 10 = -10
0 - 0.0 = 0.0
0 - 10.0 = -10.0
0 - 3.14 = -3.14
0 - '0' - TypeError Unsupported operand type string for subtraction
0 - '10' - TypeError Unsupported operand type string for subtraction
0 - '010' - TypeError Unsupported operand type string for subtraction
0 - '10 elephants' - TypeError Unsupported operand type string for subtraction
0 - 'foo' - TypeError Unsupported operand type string for subtraction
0 - array ( ) - TypeError Unsupported operand type array for subtraction
0 - array ( 0 => 1 ) - TypeError Unsupported operand type array for subtraction
0 - array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for subtraction
0 - array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for subtraction
0 - array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for subtraction
0 - (object) array ( ) - TypeError Unsupported operand type object for subtraction
0 - (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for subtraction
0 - (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for subtraction
0 - DateTime - TypeError Unsupported operand type object for subtraction
0 - resource - TypeError Unsupported operand type resource for subtraction
0 - NULL - TypeError Unsupported operand type null for subtraction
10 - false - TypeError Unsupported operand type bool for subtraction
10 - true - TypeError Unsupported operand type bool for subtraction
10 - 0 = 10
10 - 10 = 0
10 - 0.0 = 10.0
10 - 10.0 = 0.0
10 - 3.14 = 6.859999999999999
10 - '0' - TypeError Unsupported operand type string for subtraction
10 - '10' - TypeError Unsupported operand type string for subtraction
10 - '010' - TypeError Unsupported operand type string for subtraction
10 - '10 elephants' - TypeError Unsupported operand type string for subtraction
10 - 'foo' - TypeError Unsupported operand type string for subtraction
10 - array ( ) - TypeError Unsupported operand type array for subtraction
10 - array ( 0 => 1 ) - TypeError Unsupported operand type array for subtraction
10 - array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for subtraction
10 - array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for subtraction
10 - array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for subtraction
10 - (object) array ( ) - TypeError Unsupported operand type object for subtraction
10 - (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for subtraction
10 - (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for subtraction
10 - DateTime - TypeError Unsupported operand type object for subtraction
10 - resource - TypeError Unsupported operand type resource for subtraction
10 - NULL - TypeError Unsupported operand type null for subtraction
0.0 - false - TypeError Unsupported operand type bool for subtraction
0.0 - true - TypeError Unsupported operand type bool for subtraction
0.0 - 0 = 0.0
0.0 - 10 = -10.0
0.0 - 0.0 = 0.0
0.0 - 10.0 = -10.0
0.0 - 3.14 = -3.14
0.0 - '0' - TypeError Unsupported operand type string for subtraction
0.0 - '10' - TypeError Unsupported operand type string for subtraction
0.0 - '010' - TypeError Unsupported operand type string for subtraction
0.0 - '10 elephants' - TypeError Unsupported operand type string for subtraction
0.0 - 'foo' - TypeError Unsupported operand type string for subtraction
0.0 - array ( ) - TypeError Unsupported operand type array for subtraction
0.0 - array ( 0 => 1 ) - TypeError Unsupported operand type array for subtraction
0.0 - array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for subtraction
0.0 - array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for subtraction
0.0 - array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for subtraction
0.0 - (object) array ( ) - TypeError Unsupported operand type object for subtraction
0.0 - (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for subtraction
0.0 - (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for subtraction
0.0 - DateTime - TypeError Unsupported operand type object for subtraction
0.0 - resource - TypeError Unsupported operand type resource for subtraction
0.0 - NULL - TypeError Unsupported operand type null for subtraction
10.0 - false - TypeError Unsupported operand type bool for subtraction
10.0 - true - TypeError Unsupported operand type bool for subtraction
10.0 - 0 = 10.0
10.0 - 10 = 0.0
10.0 - 0.0 = 10.0
10.0 - 10.0 = 0.0
10.0 - 3.14 = 6.859999999999999
10.0 - '0' - TypeError Unsupported operand type string for subtraction
10.0 - '10' - TypeError Unsupported operand type string for subtraction
10.0 - '010' - TypeError Unsupported operand type string for subtraction
10.0 - '10 elephants' - TypeError Unsupported operand type string for subtraction
10.0 - 'foo' - TypeError Unsupported operand type string for subtraction
10.0 - array ( ) - TypeError Unsupported operand type array for subtraction
10.0 - array ( 0 => 1 ) - TypeError Unsupported operand type array for subtraction
10.0 - array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for subtraction
10.0 - array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for subtraction
10.0 - array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for subtraction
10.0 - (object) array ( ) - TypeError Unsupported operand type object for subtraction
10.0 - (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for subtraction
10.0 - (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for subtraction
10.0 - DateTime - TypeError Unsupported operand type object for subtraction
10.0 - resource - TypeError Unsupported operand type resource for subtraction
10.0 - NULL - TypeError Unsupported operand type null for subtraction
3.14 - false - TypeError Unsupported operand type bool for subtraction
3.14 - true - TypeError Unsupported operand type bool for subtraction
3.14 - 0 = 3.14
3.14 - 10 = -6.859999999999999
3.14 - 0.0 = 3.14
3.14 - 10.0 = -6.859999999999999
3.14 - 3.14 = 0.0
3.14 - '0' - TypeError Unsupported operand type string for subtraction
3.14 - '10' - TypeError Unsupported operand type string for subtraction
3.14 - '010' - TypeError Unsupported operand type string for subtraction
3.14 - '10 elephants' - TypeError Unsupported operand type string for subtraction
3.14 - 'foo' - TypeError Unsupported operand type string for subtraction
3.14 - array ( ) - TypeError Unsupported operand type array for subtraction
3.14 - array ( 0 => 1 ) - TypeError Unsupported operand type array for subtraction
3.14 - array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for subtraction
3.14 - array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for subtraction
3.14 - array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for subtraction
3.14 - (object) array ( ) - TypeError Unsupported operand type object for subtraction
3.14 - (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for subtraction
3.14 - (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for subtraction
3.14 - DateTime - TypeError Unsupported operand type object for subtraction
3.14 - resource - TypeError Unsupported operand type resource for subtraction
3.14 - NULL - TypeError Unsupported operand type null for subtraction
'0' - false - TypeError Unsupported operand type string for subtraction
'0' - true - TypeError Unsupported operand type string for subtraction
'0' - 0 - TypeError Unsupported operand type string for subtraction
'0' - 10 - TypeError Unsupported operand type string for subtraction
'0' - 0.0 - TypeError Unsupported operand type string for subtraction
'0' - 10.0 - TypeError Unsupported operand type string for subtraction
'0' - 3.14 - TypeError Unsupported operand type string for subtraction
'0' - '0' - TypeError Unsupported operand type string for subtraction
'0' - '10' - TypeError Unsupported operand type string for subtraction
'0' - '010' - TypeError Unsupported operand type string for subtraction
'0' - '10 elephants' - TypeError Unsupported operand type string for subtraction
'0' - 'foo' - TypeError Unsupported operand type string for subtraction
'0' - array ( ) - TypeError Unsupported operand type string for subtraction
'0' - array ( 0 => 1 ) - TypeError Unsupported operand type string for subtraction
'0' - array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for subtraction
'0' - array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for subtraction
'0' - array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for subtraction
'0' - (object) array ( ) - TypeError Unsupported operand type string for subtraction
'0' - (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for subtraction
'0' - (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for subtraction
'0' - DateTime - TypeError Unsupported operand type string for subtraction
'0' - resource - TypeError Unsupported operand type string for subtraction
'0' - NULL - TypeError Unsupported operand type string for subtraction
'10' - false - TypeError Unsupported operand type string for subtraction
'10' - true - TypeError Unsupported operand type string for subtraction
'10' - 0 - TypeError Unsupported operand type string for subtraction
'10' - 10 - TypeError Unsupported operand type string for subtraction
'10' - 0.0 - TypeError Unsupported operand type string for subtraction
'10' - 10.0 - TypeError Unsupported operand type string for subtraction
'10' - 3.14 - TypeError Unsupported operand type string for subtraction
'10' - '0' - TypeError Unsupported operand type string for subtraction
'10' - '10' - TypeError Unsupported operand type string for subtraction
'10' - '010' - TypeError Unsupported operand type string for subtraction
'10' - '10 elephants' - TypeError Unsupported operand type string for subtraction
'10' - 'foo' - TypeError Unsupported operand type string for subtraction
'10' - array ( ) - TypeError Unsupported operand type string for subtraction
'10' - array ( 0 => 1 ) - TypeError Unsupported operand type string for subtraction
'10' - array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for subtraction
'10' - array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for subtraction
'10' - array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for subtraction
'10' - (object) array ( ) - TypeError Unsupported operand type string for subtraction
'10' - (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for subtraction
'10' - (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for subtraction
'10' - DateTime - TypeError Unsupported operand type string for subtraction
'10' - resource - TypeError Unsupported operand type string for subtraction
'10' - NULL - TypeError Unsupported operand type string for subtraction
'010' - false - TypeError Unsupported operand type string for subtraction
'010' - true - TypeError Unsupported operand type string for subtraction
'010' - 0 - TypeError Unsupported operand type string for subtraction
'010' - 10 - TypeError Unsupported operand type string for subtraction
'010' - 0.0 - TypeError Unsupported operand type string for subtraction
'010' - 10.0 - TypeError Unsupported operand type string for subtraction
'010' - 3.14 - TypeError Unsupported operand type string for subtraction
'010' - '0' - TypeError Unsupported operand type string for subtraction
'010' - '10' - TypeError Unsupported operand type string for subtraction
'010' - '010' - TypeError Unsupported operand type string for subtraction
'010' - '10 elephants' - TypeError Unsupported operand type string for subtraction
'010' - 'foo' - TypeError Unsupported operand type string for subtraction
'010' - array ( ) - TypeError Unsupported operand type string for subtraction
'010' - array ( 0 => 1 ) - TypeError Unsupported operand type string for subtraction
'010' - array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for subtraction
'010' - array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for subtraction
'010' - array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for subtraction
'010' - (object) array ( ) - TypeError Unsupported operand type string for subtraction
'010' - (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for subtraction
'010' - (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for subtraction
'010' - DateTime - TypeError Unsupported operand type string for subtraction
'010' - resource - TypeError Unsupported operand type string for subtraction
'010' - NULL - TypeError Unsupported operand type string for subtraction
'10 elephants' - false - TypeError Unsupported operand type string for subtraction
'10 elephants' - true - TypeError Unsupported operand type string for subtraction
'10 elephants' - 0 - TypeError Unsupported operand type string for subtraction
'10 elephants' - 10 - TypeError Unsupported operand type string for subtraction
'10 elephants' - 0.0 - TypeError Unsupported operand type string for subtraction
'10 elephants' - 10.0 - TypeError Unsupported operand type string for subtraction
'10 elephants' - 3.14 - TypeError Unsupported operand type string for subtraction
'10 elephants' - '0' - TypeError Unsupported operand type string for subtraction
'10 elephants' - '10' - TypeError Unsupported operand type string for subtraction
'10 elephants' - '010' - TypeError Unsupported operand type string for subtraction
'10 elephants' - '10 elephants' - TypeError Unsupported operand type string for subtraction
'10 elephants' - 'foo' - TypeError Unsupported operand type string for subtraction
'10 elephants' - array ( ) - TypeError Unsupported operand type string for subtraction
'10 elephants' - array ( 0 => 1 ) - TypeError Unsupported operand type string for subtraction
'10 elephants' - array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for subtraction
'10 elephants' - array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for subtraction
'10 elephants' - array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for subtraction
'10 elephants' - (object) array ( ) - TypeError Unsupported operand type string for subtraction
'10 elephants' - (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for subtraction
'10 elephants' - (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for subtraction
'10 elephants' - DateTime - TypeError Unsupported operand type string for subtraction
'10 elephants' - resource - TypeError Unsupported operand type string for subtraction
'10 elephants' - NULL - TypeError Unsupported operand type string for subtraction
'foo' - false - TypeError Unsupported operand type string for subtraction
'foo' - true - TypeError Unsupported operand type string for subtraction
'foo' - 0 - TypeError Unsupported operand type string for subtraction
'foo' - 10 - TypeError Unsupported operand type string for subtraction
'foo' - 0.0 - TypeError Unsupported operand type string for subtraction
'foo' - 10.0 - TypeError Unsupported operand type string for subtraction
'foo' - 3.14 - TypeError Unsupported operand type string for subtraction
'foo' - '0' - TypeError Unsupported operand type string for subtraction
'foo' - '10' - TypeError Unsupported operand type string for subtraction
'foo' - '010' - TypeError Unsupported operand type string for subtraction
'foo' - '10 elephants' - TypeError Unsupported operand type string for subtraction
'foo' - 'foo' - TypeError Unsupported operand type string for subtraction
'foo' - array ( ) - TypeError Unsupported operand type string for subtraction
'foo' - array ( 0 => 1 ) - TypeError Unsupported operand type string for subtraction
'foo' - array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for subtraction
'foo' - array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for subtraction
'foo' - array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for subtraction
'foo' - (object) array ( ) - TypeError Unsupported operand type string for subtraction
'foo' - (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for subtraction
'foo' - (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for subtraction
'foo' - DateTime - TypeError Unsupported operand type string for subtraction
'foo' - resource - TypeError Unsupported operand type string for subtraction
'foo' - NULL - TypeError Unsupported operand type string for subtraction
array ( ) - false - TypeError Unsupported operand type array for subtraction
array ( ) - true - TypeError Unsupported operand type array for subtraction
array ( ) - 0 - TypeError Unsupported operand type array for subtraction
array ( ) - 10 - TypeError Unsupported operand type array for subtraction
array ( ) - 0.0 - TypeError Unsupported operand type array for subtraction
array ( ) - 10.0 - TypeError Unsupported operand type array for subtraction
array ( ) - 3.14 - TypeError Unsupported operand type array for subtraction
array ( ) - '0' - TypeError Unsupported operand type array for subtraction
array ( ) - '10' - TypeError Unsupported operand type array for subtraction
array ( ) - '010' - TypeError Unsupported operand type array for subtraction
array ( ) - '10 elephants' - TypeError Unsupported operand type array for subtraction
array ( ) - 'foo' - TypeError Unsupported operand type array for subtraction
array ( ) - array ( ) - TypeError Unsupported operand type array for subtraction
array ( ) - array ( 0 => 1 ) - TypeError Unsupported operand type array for subtraction
array ( ) - array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for subtraction
array ( ) - array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for subtraction
array ( ) - array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for subtraction
array ( ) - (object) array ( ) - TypeError Unsupported operand type array for subtraction
array ( ) - (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for subtraction
array ( ) - (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for subtraction
array ( ) - DateTime - TypeError Unsupported operand type array for subtraction
array ( ) - resource - TypeError Unsupported operand type array for subtraction
array ( ) - NULL - TypeError Unsupported operand type array for subtraction
array ( 0 => 1 ) - false - TypeError Unsupported operand type array for subtraction
array ( 0 => 1 ) - true - TypeError Unsupported operand type array for subtraction
array ( 0 => 1 ) - 0 - TypeError Unsupported operand type array for subtraction
array ( 0 => 1 ) - 10 - TypeError Unsupported operand type array for subtraction
array ( 0 => 1 ) - 0.0 - TypeError Unsupported operand type array for subtraction
array ( 0 => 1 ) - 10.0 - TypeError Unsupported operand type array for subtraction
array ( 0 => 1 ) - 3.14 - TypeError Unsupported operand type array for subtraction
array ( 0 => 1 ) - '0' - TypeError Unsupported operand type array for subtraction
array ( 0 => 1 ) - '10' - TypeError Unsupported operand type array for subtraction
array ( 0 => 1 ) - '010' - TypeError Unsupported operand type array for subtraction
array ( 0 => 1 ) - '10 elephants' - TypeError Unsupported operand type array for subtraction
array ( 0 => 1 ) - 'foo' - TypeError Unsupported operand type array for subtraction
array ( 0 => 1 ) - array ( ) - TypeError Unsupported operand type array for subtraction
array ( 0 => 1 ) - array ( 0 => 1 ) - TypeError Unsupported operand type array for subtraction
array ( 0 => 1 ) - array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for subtraction
array ( 0 => 1 ) - array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for subtraction
array ( 0 => 1 ) - array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for subtraction
array ( 0 => 1 ) - (object) array ( ) - TypeError Unsupported operand type array for subtraction
array ( 0 => 1 ) - (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for subtraction
array ( 0 => 1 ) - (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for subtraction
array ( 0 => 1 ) - DateTime - TypeError Unsupported operand type array for subtraction
array ( 0 => 1 ) - resource - TypeError Unsupported operand type array for subtraction
array ( 0 => 1 ) - NULL - TypeError Unsupported operand type array for subtraction
array ( 0 => 1, 1 => 100 ) - false - TypeError Unsupported operand type array for subtraction
array ( 0 => 1, 1 => 100 ) - true - TypeError Unsupported operand type array for subtraction
array ( 0 => 1, 1 => 100 ) - 0 - TypeError Unsupported operand type array for subtraction
array ( 0 => 1, 1 => 100 ) - 10 - TypeError Unsupported operand type array for subtraction
array ( 0 => 1, 1 => 100 ) - 0.0 - TypeError Unsupported operand type array for subtraction
array ( 0 => 1, 1 => 100 ) - 10.0 - TypeError Unsupported operand type array for subtraction
array ( 0 => 1, 1 => 100 ) - 3.14 - TypeError Unsupported operand type array for subtraction
array ( 0 => 1, 1 => 100 ) - '0' - TypeError Unsupported operand type array for subtraction
array ( 0 => 1, 1 => 100 ) - '10' - TypeError Unsupported operand type array for subtraction
array ( 0 => 1, 1 => 100 ) - '010' - TypeError Unsupported operand type array for subtraction
array ( 0 => 1, 1 => 100 ) - '10 elephants' - TypeError Unsupported operand type array for subtraction
array ( 0 => 1, 1 => 100 ) - 'foo' - TypeError Unsupported operand type array for subtraction
array ( 0 => 1, 1 => 100 ) - array ( ) - TypeError Unsupported operand type array for subtraction
array ( 0 => 1, 1 => 100 ) - array ( 0 => 1 ) - TypeError Unsupported operand type array for subtraction
array ( 0 => 1, 1 => 100 ) - array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for subtraction
array ( 0 => 1, 1 => 100 ) - array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for subtraction
array ( 0 => 1, 1 => 100 ) - array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for subtraction
array ( 0 => 1, 1 => 100 ) - (object) array ( ) - TypeError Unsupported operand type array for subtraction
array ( 0 => 1, 1 => 100 ) - (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for subtraction
array ( 0 => 1, 1 => 100 ) - (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for subtraction
array ( 0 => 1, 1 => 100 ) - DateTime - TypeError Unsupported operand type array for subtraction
array ( 0 => 1, 1 => 100 ) - resource - TypeError Unsupported operand type array for subtraction
array ( 0 => 1, 1 => 100 ) - NULL - TypeError Unsupported operand type array for subtraction
array ( 'foo' => 1, 'bar' => 2 ) - false - TypeError Unsupported operand type array for subtraction
array ( 'foo' => 1, 'bar' => 2 ) - true - TypeError Unsupported operand type array for subtraction
array ( 'foo' => 1, 'bar' => 2 ) - 0 - TypeError Unsupported operand type array for subtraction
array ( 'foo' => 1, 'bar' => 2 ) - 10 - TypeError Unsupported operand type array for subtraction
array ( 'foo' => 1, 'bar' => 2 ) - 0.0 - TypeError Unsupported operand type array for subtraction
array ( 'foo' => 1, 'bar' => 2 ) - 10.0 - TypeError Unsupported operand type array for subtraction
array ( 'foo' => 1, 'bar' => 2 ) - 3.14 - TypeError Unsupported operand type array for subtraction
array ( 'foo' => 1, 'bar' => 2 ) - '0' - TypeError Unsupported operand type array for subtraction
array ( 'foo' => 1, 'bar' => 2 ) - '10' - TypeError Unsupported operand type array for subtraction
array ( 'foo' => 1, 'bar' => 2 ) - '010' - TypeError Unsupported operand type array for subtraction
array ( 'foo' => 1, 'bar' => 2 ) - '10 elephants' - TypeError Unsupported operand type array for subtraction
array ( 'foo' => 1, 'bar' => 2 ) - 'foo' - TypeError Unsupported operand type array for subtraction
array ( 'foo' => 1, 'bar' => 2 ) - array ( ) - TypeError Unsupported operand type array for subtraction
array ( 'foo' => 1, 'bar' => 2 ) - array ( 0 => 1 ) - TypeError Unsupported operand type array for subtraction
array ( 'foo' => 1, 'bar' => 2 ) - array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for subtraction
array ( 'foo' => 1, 'bar' => 2 ) - array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for subtraction
array ( 'foo' => 1, 'bar' => 2 ) - array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for subtraction
array ( 'foo' => 1, 'bar' => 2 ) - (object) array ( ) - TypeError Unsupported operand type array for subtraction
array ( 'foo' => 1, 'bar' => 2 ) - (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for subtraction
array ( 'foo' => 1, 'bar' => 2 ) - (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for subtraction
array ( 'foo' => 1, 'bar' => 2 ) - DateTime - TypeError Unsupported operand type array for subtraction
array ( 'foo' => 1, 'bar' => 2 ) - resource - TypeError Unsupported operand type array for subtraction
array ( 'foo' => 1, 'bar' => 2 ) - NULL - TypeError Unsupported operand type array for subtraction
array ( 'bar' => 1, 'foo' => 2 ) - false - TypeError Unsupported operand type array for subtraction
array ( 'bar' => 1, 'foo' => 2 ) - true - TypeError Unsupported operand type array for subtraction
array ( 'bar' => 1, 'foo' => 2 ) - 0 - TypeError Unsupported operand type array for subtraction
array ( 'bar' => 1, 'foo' => 2 ) - 10 - TypeError Unsupported operand type array for subtraction
array ( 'bar' => 1, 'foo' => 2 ) - 0.0 - TypeError Unsupported operand type array for subtraction
array ( 'bar' => 1, 'foo' => 2 ) - 10.0 - TypeError Unsupported operand type array for subtraction
array ( 'bar' => 1, 'foo' => 2 ) - 3.14 - TypeError Unsupported operand type array for subtraction
array ( 'bar' => 1, 'foo' => 2 ) - '0' - TypeError Unsupported operand type array for subtraction
array ( 'bar' => 1, 'foo' => 2 ) - '10' - TypeError Unsupported operand type array for subtraction
array ( 'bar' => 1, 'foo' => 2 ) - '010' - TypeError Unsupported operand type array for subtraction
array ( 'bar' => 1, 'foo' => 2 ) - '10 elephants' - TypeError Unsupported operand type array for subtraction
array ( 'bar' => 1, 'foo' => 2 ) - 'foo' - TypeError Unsupported operand type array for subtraction
array ( 'bar' => 1, 'foo' => 2 ) - array ( ) - TypeError Unsupported operand type array for subtraction
array ( 'bar' => 1, 'foo' => 2 ) - array ( 0 => 1 ) - TypeError Unsupported operand type array for subtraction
array ( 'bar' => 1, 'foo' => 2 ) - array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for subtraction
array ( 'bar' => 1, 'foo' => 2 ) - array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for subtraction
array ( 'bar' => 1, 'foo' => 2 ) - array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for subtraction
array ( 'bar' => 1, 'foo' => 2 ) - (object) array ( ) - TypeError Unsupported operand type array for subtraction
array ( 'bar' => 1, 'foo' => 2 ) - (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for subtraction
array ( 'bar' => 1, 'foo' => 2 ) - (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for subtraction
array ( 'bar' => 1, 'foo' => 2 ) - DateTime - TypeError Unsupported operand type array for subtraction
array ( 'bar' => 1, 'foo' => 2 ) - resource - TypeError Unsupported operand type array for subtraction
array ( 'bar' => 1, 'foo' => 2 ) - NULL - TypeError Unsupported operand type array for subtraction
(object) array ( ) - false - TypeError Unsupported operand type object for subtraction
(object) array ( ) - true - TypeError Unsupported operand type object for subtraction
(object) array ( ) - 0 - TypeError Unsupported operand type object for subtraction
(object) array ( ) - 10 - TypeError Unsupported operand type object for subtraction
(object) array ( ) - 0.0 - TypeError Unsupported operand type object for subtraction
(object) array ( ) - 10.0 - TypeError Unsupported operand type object for subtraction
(object) array ( ) - 3.14 - TypeError Unsupported operand type object for subtraction
(object) array ( ) - '0' - TypeError Unsupported operand type object for subtraction
(object) array ( ) - '10' - TypeError Unsupported operand type object for subtraction
(object) array ( ) - '010' - TypeError Unsupported operand type object for subtraction
(object) array ( ) - '10 elephants' - TypeError Unsupported operand type object for subtraction
(object) array ( ) - 'foo' - TypeError Unsupported operand type object for subtraction
(object) array ( ) - array ( ) - TypeError Unsupported operand type object for subtraction
(object) array ( ) - array ( 0 => 1 ) - TypeError Unsupported operand type object for subtraction
(object) array ( ) - array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for subtraction
(object) array ( ) - array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for subtraction
(object) array ( ) - array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for subtraction
(object) array ( ) - (object) array ( ) - TypeError Unsupported operand type object for subtraction
(object) array ( ) - (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for subtraction
(object) array ( ) - (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for subtraction
(object) array ( ) - DateTime - TypeError Unsupported operand type object for subtraction
(object) array ( ) - resource - TypeError Unsupported operand type object for subtraction
(object) array ( ) - NULL - TypeError Unsupported operand type object for subtraction
(object) array ( 'foo' => 1, 'bar' => 2 ) - false - TypeError Unsupported operand type object for subtraction
(object) array ( 'foo' => 1, 'bar' => 2 ) - true - TypeError Unsupported operand type object for subtraction
(object) array ( 'foo' => 1, 'bar' => 2 ) - 0 - TypeError Unsupported operand type object for subtraction
(object) array ( 'foo' => 1, 'bar' => 2 ) - 10 - TypeError Unsupported operand type object for subtraction
(object) array ( 'foo' => 1, 'bar' => 2 ) - 0.0 - TypeError Unsupported operand type object for subtraction
(object) array ( 'foo' => 1, 'bar' => 2 ) - 10.0 - TypeError Unsupported operand type object for subtraction
(object) array ( 'foo' => 1, 'bar' => 2 ) - 3.14 - TypeError Unsupported operand type object for subtraction
(object) array ( 'foo' => 1, 'bar' => 2 ) - '0' - TypeError Unsupported operand type object for subtraction
(object) array ( 'foo' => 1, 'bar' => 2 ) - '10' - TypeError Unsupported operand type object for subtraction
(object) array ( 'foo' => 1, 'bar' => 2 ) - '010' - TypeError Unsupported operand type object for subtraction
(object) array ( 'foo' => 1, 'bar' => 2 ) - '10 elephants' - TypeError Unsupported operand type object for subtraction
(object) array ( 'foo' => 1, 'bar' => 2 ) - 'foo' - TypeError Unsupported operand type object for subtraction
(object) array ( 'foo' => 1, 'bar' => 2 ) - array ( ) - TypeError Unsupported operand type object for subtraction
(object) array ( 'foo' => 1, 'bar' => 2 ) - array ( 0 => 1 ) - TypeError Unsupported operand type object for subtraction
(object) array ( 'foo' => 1, 'bar' => 2 ) - array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for subtraction
(object) array ( 'foo' => 1, 'bar' => 2 ) - array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for subtraction
(object) array ( 'foo' => 1, 'bar' => 2 ) - array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for subtraction
(object) array ( 'foo' => 1, 'bar' => 2 ) - (object) array ( ) - TypeError Unsupported operand type object for subtraction
(object) array ( 'foo' => 1, 'bar' => 2 ) - (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for subtraction
(object) array ( 'foo' => 1, 'bar' => 2 ) - (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for subtraction
(object) array ( 'foo' => 1, 'bar' => 2 ) - DateTime - TypeError Unsupported operand type object for subtraction
(object) array ( 'foo' => 1, 'bar' => 2 ) - resource - TypeError Unsupported operand type object for subtraction
(object) array ( 'foo' => 1, 'bar' => 2 ) - NULL - TypeError Unsupported operand type object for subtraction
(object) array ( 'bar' => 1, 'foo' => 2 ) - false - TypeError Unsupported operand type object for subtraction
(object) array ( 'bar' => 1, 'foo' => 2 ) - true - TypeError Unsupported operand type object for subtraction
(object) array ( 'bar' => 1, 'foo' => 2 ) - 0 - TypeError Unsupported operand type object for subtraction
(object) array ( 'bar' => 1, 'foo' => 2 ) - 10 - TypeError Unsupported operand type object for subtraction
(object) array ( 'bar' => 1, 'foo' => 2 ) - 0.0 - TypeError Unsupported operand type object for subtraction
(object) array ( 'bar' => 1, 'foo' => 2 ) - 10.0 - TypeError Unsupported operand type object for subtraction
(object) array ( 'bar' => 1, 'foo' => 2 ) - 3.14 - TypeError Unsupported operand type object for subtraction
(object) array ( 'bar' => 1, 'foo' => 2 ) - '0' - TypeError Unsupported operand type object for subtraction
(object) array ( 'bar' => 1, 'foo' => 2 ) - '10' - TypeError Unsupported operand type object for subtraction
(object) array ( 'bar' => 1, 'foo' => 2 ) - '010' - TypeError Unsupported operand type object for subtraction
(object) array ( 'bar' => 1, 'foo' => 2 ) - '10 elephants' - TypeError Unsupported operand type object for subtraction
(object) array ( 'bar' => 1, 'foo' => 2 ) - 'foo' - TypeError Unsupported operand type object for subtraction
(object) array ( 'bar' => 1, 'foo' => 2 ) - array ( ) - TypeError Unsupported operand type object for subtraction
(object) array ( 'bar' => 1, 'foo' => 2 ) - array ( 0 => 1 ) - TypeError Unsupported operand type object for subtraction
(object) array ( 'bar' => 1, 'foo' => 2 ) - array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for subtraction
(object) array ( 'bar' => 1, 'foo' => 2 ) - array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for subtraction
(object) array ( 'bar' => 1, 'foo' => 2 ) - array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for subtraction
(object) array ( 'bar' => 1, 'foo' => 2 ) - (object) array ( ) - TypeError Unsupported operand type object for subtraction
(object) array ( 'bar' => 1, 'foo' => 2 ) - (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for subtraction
(object) array ( 'bar' => 1, 'foo' => 2 ) - (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for subtraction
(object) array ( 'bar' => 1, 'foo' => 2 ) - DateTime - TypeError Unsupported operand type object for subtraction
(object) array ( 'bar' => 1, 'foo' => 2 ) - resource - TypeError Unsupported operand type object for subtraction
(object) array ( 'bar' => 1, 'foo' => 2 ) - NULL - TypeError Unsupported operand type object for subtraction
DateTime - false - TypeError Unsupported operand type object for subtraction
DateTime - true - TypeError Unsupported operand type object for subtraction
DateTime - 0 - TypeError Unsupported operand type object for subtraction
DateTime - 10 - TypeError Unsupported operand type object for subtraction
DateTime - 0.0 - TypeError Unsupported operand type object for subtraction
DateTime - 10.0 - TypeError Unsupported operand type object for subtraction
DateTime - 3.14 - TypeError Unsupported operand type object for subtraction
DateTime - '0' - TypeError Unsupported operand type object for subtraction
DateTime - '10' - TypeError Unsupported operand type object for subtraction
DateTime - '010' - TypeError Unsupported operand type object for subtraction
DateTime - '10 elephants' - TypeError Unsupported operand type object for subtraction
DateTime - 'foo' - TypeError Unsupported operand type object for subtraction
DateTime - array ( ) - TypeError Unsupported operand type object for subtraction
DateTime - array ( 0 => 1 ) - TypeError Unsupported operand type object for subtraction
DateTime - array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for subtraction
DateTime - array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for subtraction
DateTime - array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for subtraction
DateTime - (object) array ( ) - TypeError Unsupported operand type object for subtraction
DateTime - (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for subtraction
DateTime - (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for subtraction
DateTime - DateTime - TypeError Unsupported operand type object for subtraction
DateTime - resource - TypeError Unsupported operand type object for subtraction
DateTime - NULL - TypeError Unsupported operand type object for subtraction
resource - false - TypeError Unsupported operand type resource for subtraction
resource - true - TypeError Unsupported operand type resource for subtraction
resource - 0 - TypeError Unsupported operand type resource for subtraction
resource - 10 - TypeError Unsupported operand type resource for subtraction
resource - 0.0 - TypeError Unsupported operand type resource for subtraction
resource - 10.0 - TypeError Unsupported operand type resource for subtraction
resource - 3.14 - TypeError Unsupported operand type resource for subtraction
resource - '0' - TypeError Unsupported operand type resource for subtraction
resource - '10' - TypeError Unsupported operand type resource for subtraction
resource - '010' - TypeError Unsupported operand type resource for subtraction
resource - '10 elephants' - TypeError Unsupported operand type resource for subtraction
resource - 'foo' - TypeError Unsupported operand type resource for subtraction
resource - array ( ) - TypeError Unsupported operand type resource for subtraction
resource - array ( 0 => 1 ) - TypeError Unsupported operand type resource for subtraction
resource - array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type resource for subtraction
resource - array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for subtraction
resource - array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for subtraction
resource - (object) array ( ) - TypeError Unsupported operand type resource for subtraction
resource - (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for subtraction
resource - (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for subtraction
resource - DateTime - TypeError Unsupported operand type resource for subtraction
resource - resource - TypeError Unsupported operand type resource for subtraction
resource - NULL - TypeError Unsupported operand type resource for subtraction
NULL - false - TypeError Unsupported operand type null for subtraction
NULL - true - TypeError Unsupported operand type null for subtraction
NULL - 0 - TypeError Unsupported operand type null for subtraction
NULL - 10 - TypeError Unsupported operand type null for subtraction
NULL - 0.0 - TypeError Unsupported operand type null for subtraction
NULL - 10.0 - TypeError Unsupported operand type null for subtraction
NULL - 3.14 - TypeError Unsupported operand type null for subtraction
NULL - '0' - TypeError Unsupported operand type null for subtraction
NULL - '10' - TypeError Unsupported operand type null for subtraction
NULL - '010' - TypeError Unsupported operand type null for subtraction
NULL - '10 elephants' - TypeError Unsupported operand type null for subtraction
NULL - 'foo' - TypeError Unsupported operand type null for subtraction
NULL - array ( ) - TypeError Unsupported operand type null for subtraction
NULL - array ( 0 => 1 ) - TypeError Unsupported operand type null for subtraction
NULL - array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type null for subtraction
NULL - array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for subtraction
NULL - array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for subtraction
NULL - (object) array ( ) - TypeError Unsupported operand type null for subtraction
NULL - (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for subtraction
NULL - (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for subtraction
NULL - DateTime - TypeError Unsupported operand type null for subtraction
NULL - resource - TypeError Unsupported operand type null for subtraction
NULL - NULL - TypeError Unsupported operand type null for subtraction