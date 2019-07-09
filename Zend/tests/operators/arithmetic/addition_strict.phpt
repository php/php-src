--TEST--
addition '+' operator with strict_operators
--INI--
precision=14
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a + $b', function($a, $b) { return $a + $b; });

--EXPECT--
false + false - TypeError Unsupported operand type bool for addition
false + true - TypeError Unsupported operand type bool for addition
false + 0 - TypeError Unsupported operand type bool for addition
false + 10 - TypeError Unsupported operand type bool for addition
false + 0.0 - TypeError Unsupported operand type bool for addition
false + 10.0 - TypeError Unsupported operand type bool for addition
false + 3.14 - TypeError Unsupported operand type bool for addition
false + '0' - TypeError Unsupported operand type bool for addition
false + '10' - TypeError Unsupported operand type bool for addition
false + '010' - TypeError Unsupported operand type bool for addition
false + '10 elephants' - TypeError Unsupported operand type bool for addition
false + 'foo' - TypeError Unsupported operand type bool for addition
false + array ( ) - TypeError Unsupported operand type bool for addition
false + array ( 0 => 1 ) - TypeError Unsupported operand type bool for addition
false + array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for addition
false + array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for addition
false + array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for addition
false + (object) array ( ) - TypeError Unsupported operand type bool for addition
false + (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for addition
false + (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for addition
false + DateTime - TypeError Unsupported operand type bool for addition
false + resource - TypeError Unsupported operand type bool for addition
false + NULL - TypeError Unsupported operand type bool for addition
true + false - TypeError Unsupported operand type bool for addition
true + true - TypeError Unsupported operand type bool for addition
true + 0 - TypeError Unsupported operand type bool for addition
true + 10 - TypeError Unsupported operand type bool for addition
true + 0.0 - TypeError Unsupported operand type bool for addition
true + 10.0 - TypeError Unsupported operand type bool for addition
true + 3.14 - TypeError Unsupported operand type bool for addition
true + '0' - TypeError Unsupported operand type bool for addition
true + '10' - TypeError Unsupported operand type bool for addition
true + '010' - TypeError Unsupported operand type bool for addition
true + '10 elephants' - TypeError Unsupported operand type bool for addition
true + 'foo' - TypeError Unsupported operand type bool for addition
true + array ( ) - TypeError Unsupported operand type bool for addition
true + array ( 0 => 1 ) - TypeError Unsupported operand type bool for addition
true + array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for addition
true + array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for addition
true + array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for addition
true + (object) array ( ) - TypeError Unsupported operand type bool for addition
true + (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for addition
true + (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for addition
true + DateTime - TypeError Unsupported operand type bool for addition
true + resource - TypeError Unsupported operand type bool for addition
true + NULL - TypeError Unsupported operand type bool for addition
0 + false - TypeError Unsupported operand type bool for addition
0 + true - TypeError Unsupported operand type bool for addition
0 + 0 = 0
0 + 10 = 10
0 + 0.0 = 0.0
0 + 10.0 = 10.0
0 + 3.14 = 3.14
0 + '0' - TypeError Unsupported operand type string for addition
0 + '10' - TypeError Unsupported operand type string for addition
0 + '010' - TypeError Unsupported operand type string for addition
0 + '10 elephants' - TypeError Unsupported operand type string for addition
0 + 'foo' - TypeError Unsupported operand type string for addition
0 + array ( ) - TypeError Unsupported operand type array for addition
0 + array ( 0 => 1 ) - TypeError Unsupported operand type array for addition
0 + array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for addition
0 + array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for addition
0 + array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for addition
0 + (object) array ( ) - TypeError Unsupported operand type object for addition
0 + (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for addition
0 + (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for addition
0 + DateTime - TypeError Unsupported operand type object for addition
0 + resource - TypeError Unsupported operand type resource for addition
0 + NULL - TypeError Unsupported operand type null for addition
10 + false - TypeError Unsupported operand type bool for addition
10 + true - TypeError Unsupported operand type bool for addition
10 + 0 = 10
10 + 10 = 20
10 + 0.0 = 10.0
10 + 10.0 = 20.0
10 + 3.14 = 13.14
10 + '0' - TypeError Unsupported operand type string for addition
10 + '10' - TypeError Unsupported operand type string for addition
10 + '010' - TypeError Unsupported operand type string for addition
10 + '10 elephants' - TypeError Unsupported operand type string for addition
10 + 'foo' - TypeError Unsupported operand type string for addition
10 + array ( ) - TypeError Unsupported operand type array for addition
10 + array ( 0 => 1 ) - TypeError Unsupported operand type array for addition
10 + array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for addition
10 + array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for addition
10 + array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for addition
10 + (object) array ( ) - TypeError Unsupported operand type object for addition
10 + (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for addition
10 + (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for addition
10 + DateTime - TypeError Unsupported operand type object for addition
10 + resource - TypeError Unsupported operand type resource for addition
10 + NULL - TypeError Unsupported operand type null for addition
0.0 + false - TypeError Unsupported operand type bool for addition
0.0 + true - TypeError Unsupported operand type bool for addition
0.0 + 0 = 0.0
0.0 + 10 = 10.0
0.0 + 0.0 = 0.0
0.0 + 10.0 = 10.0
0.0 + 3.14 = 3.14
0.0 + '0' - TypeError Unsupported operand type string for addition
0.0 + '10' - TypeError Unsupported operand type string for addition
0.0 + '010' - TypeError Unsupported operand type string for addition
0.0 + '10 elephants' - TypeError Unsupported operand type string for addition
0.0 + 'foo' - TypeError Unsupported operand type string for addition
0.0 + array ( ) - TypeError Unsupported operand type array for addition
0.0 + array ( 0 => 1 ) - TypeError Unsupported operand type array for addition
0.0 + array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for addition
0.0 + array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for addition
0.0 + array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for addition
0.0 + (object) array ( ) - TypeError Unsupported operand type object for addition
0.0 + (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for addition
0.0 + (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for addition
0.0 + DateTime - TypeError Unsupported operand type object for addition
0.0 + resource - TypeError Unsupported operand type resource for addition
0.0 + NULL - TypeError Unsupported operand type null for addition
10.0 + false - TypeError Unsupported operand type bool for addition
10.0 + true - TypeError Unsupported operand type bool for addition
10.0 + 0 = 10.0
10.0 + 10 = 20.0
10.0 + 0.0 = 10.0
10.0 + 10.0 = 20.0
10.0 + 3.14 = 13.14
10.0 + '0' - TypeError Unsupported operand type string for addition
10.0 + '10' - TypeError Unsupported operand type string for addition
10.0 + '010' - TypeError Unsupported operand type string for addition
10.0 + '10 elephants' - TypeError Unsupported operand type string for addition
10.0 + 'foo' - TypeError Unsupported operand type string for addition
10.0 + array ( ) - TypeError Unsupported operand type array for addition
10.0 + array ( 0 => 1 ) - TypeError Unsupported operand type array for addition
10.0 + array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for addition
10.0 + array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for addition
10.0 + array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for addition
10.0 + (object) array ( ) - TypeError Unsupported operand type object for addition
10.0 + (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for addition
10.0 + (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for addition
10.0 + DateTime - TypeError Unsupported operand type object for addition
10.0 + resource - TypeError Unsupported operand type resource for addition
10.0 + NULL - TypeError Unsupported operand type null for addition
3.14 + false - TypeError Unsupported operand type bool for addition
3.14 + true - TypeError Unsupported operand type bool for addition
3.14 + 0 = 3.14
3.14 + 10 = 13.14
3.14 + 0.0 = 3.14
3.14 + 10.0 = 13.14
3.14 + 3.14 = 6.28
3.14 + '0' - TypeError Unsupported operand type string for addition
3.14 + '10' - TypeError Unsupported operand type string for addition
3.14 + '010' - TypeError Unsupported operand type string for addition
3.14 + '10 elephants' - TypeError Unsupported operand type string for addition
3.14 + 'foo' - TypeError Unsupported operand type string for addition
3.14 + array ( ) - TypeError Unsupported operand type array for addition
3.14 + array ( 0 => 1 ) - TypeError Unsupported operand type array for addition
3.14 + array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for addition
3.14 + array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for addition
3.14 + array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for addition
3.14 + (object) array ( ) - TypeError Unsupported operand type object for addition
3.14 + (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for addition
3.14 + (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for addition
3.14 + DateTime - TypeError Unsupported operand type object for addition
3.14 + resource - TypeError Unsupported operand type resource for addition
3.14 + NULL - TypeError Unsupported operand type null for addition
'0' + false - TypeError Unsupported operand type string for addition
'0' + true - TypeError Unsupported operand type string for addition
'0' + 0 - TypeError Unsupported operand type string for addition
'0' + 10 - TypeError Unsupported operand type string for addition
'0' + 0.0 - TypeError Unsupported operand type string for addition
'0' + 10.0 - TypeError Unsupported operand type string for addition
'0' + 3.14 - TypeError Unsupported operand type string for addition
'0' + '0' - TypeError Unsupported operand type string for addition
'0' + '10' - TypeError Unsupported operand type string for addition
'0' + '010' - TypeError Unsupported operand type string for addition
'0' + '10 elephants' - TypeError Unsupported operand type string for addition
'0' + 'foo' - TypeError Unsupported operand type string for addition
'0' + array ( ) - TypeError Unsupported operand type string for addition
'0' + array ( 0 => 1 ) - TypeError Unsupported operand type string for addition
'0' + array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for addition
'0' + array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for addition
'0' + array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for addition
'0' + (object) array ( ) - TypeError Unsupported operand type string for addition
'0' + (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for addition
'0' + (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for addition
'0' + DateTime - TypeError Unsupported operand type string for addition
'0' + resource - TypeError Unsupported operand type string for addition
'0' + NULL - TypeError Unsupported operand type string for addition
'10' + false - TypeError Unsupported operand type string for addition
'10' + true - TypeError Unsupported operand type string for addition
'10' + 0 - TypeError Unsupported operand type string for addition
'10' + 10 - TypeError Unsupported operand type string for addition
'10' + 0.0 - TypeError Unsupported operand type string for addition
'10' + 10.0 - TypeError Unsupported operand type string for addition
'10' + 3.14 - TypeError Unsupported operand type string for addition
'10' + '0' - TypeError Unsupported operand type string for addition
'10' + '10' - TypeError Unsupported operand type string for addition
'10' + '010' - TypeError Unsupported operand type string for addition
'10' + '10 elephants' - TypeError Unsupported operand type string for addition
'10' + 'foo' - TypeError Unsupported operand type string for addition
'10' + array ( ) - TypeError Unsupported operand type string for addition
'10' + array ( 0 => 1 ) - TypeError Unsupported operand type string for addition
'10' + array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for addition
'10' + array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for addition
'10' + array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for addition
'10' + (object) array ( ) - TypeError Unsupported operand type string for addition
'10' + (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for addition
'10' + (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for addition
'10' + DateTime - TypeError Unsupported operand type string for addition
'10' + resource - TypeError Unsupported operand type string for addition
'10' + NULL - TypeError Unsupported operand type string for addition
'010' + false - TypeError Unsupported operand type string for addition
'010' + true - TypeError Unsupported operand type string for addition
'010' + 0 - TypeError Unsupported operand type string for addition
'010' + 10 - TypeError Unsupported operand type string for addition
'010' + 0.0 - TypeError Unsupported operand type string for addition
'010' + 10.0 - TypeError Unsupported operand type string for addition
'010' + 3.14 - TypeError Unsupported operand type string for addition
'010' + '0' - TypeError Unsupported operand type string for addition
'010' + '10' - TypeError Unsupported operand type string for addition
'010' + '010' - TypeError Unsupported operand type string for addition
'010' + '10 elephants' - TypeError Unsupported operand type string for addition
'010' + 'foo' - TypeError Unsupported operand type string for addition
'010' + array ( ) - TypeError Unsupported operand type string for addition
'010' + array ( 0 => 1 ) - TypeError Unsupported operand type string for addition
'010' + array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for addition
'010' + array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for addition
'010' + array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for addition
'010' + (object) array ( ) - TypeError Unsupported operand type string for addition
'010' + (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for addition
'010' + (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for addition
'010' + DateTime - TypeError Unsupported operand type string for addition
'010' + resource - TypeError Unsupported operand type string for addition
'010' + NULL - TypeError Unsupported operand type string for addition
'10 elephants' + false - TypeError Unsupported operand type string for addition
'10 elephants' + true - TypeError Unsupported operand type string for addition
'10 elephants' + 0 - TypeError Unsupported operand type string for addition
'10 elephants' + 10 - TypeError Unsupported operand type string for addition
'10 elephants' + 0.0 - TypeError Unsupported operand type string for addition
'10 elephants' + 10.0 - TypeError Unsupported operand type string for addition
'10 elephants' + 3.14 - TypeError Unsupported operand type string for addition
'10 elephants' + '0' - TypeError Unsupported operand type string for addition
'10 elephants' + '10' - TypeError Unsupported operand type string for addition
'10 elephants' + '010' - TypeError Unsupported operand type string for addition
'10 elephants' + '10 elephants' - TypeError Unsupported operand type string for addition
'10 elephants' + 'foo' - TypeError Unsupported operand type string for addition
'10 elephants' + array ( ) - TypeError Unsupported operand type string for addition
'10 elephants' + array ( 0 => 1 ) - TypeError Unsupported operand type string for addition
'10 elephants' + array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for addition
'10 elephants' + array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for addition
'10 elephants' + array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for addition
'10 elephants' + (object) array ( ) - TypeError Unsupported operand type string for addition
'10 elephants' + (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for addition
'10 elephants' + (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for addition
'10 elephants' + DateTime - TypeError Unsupported operand type string for addition
'10 elephants' + resource - TypeError Unsupported operand type string for addition
'10 elephants' + NULL - TypeError Unsupported operand type string for addition
'foo' + false - TypeError Unsupported operand type string for addition
'foo' + true - TypeError Unsupported operand type string for addition
'foo' + 0 - TypeError Unsupported operand type string for addition
'foo' + 10 - TypeError Unsupported operand type string for addition
'foo' + 0.0 - TypeError Unsupported operand type string for addition
'foo' + 10.0 - TypeError Unsupported operand type string for addition
'foo' + 3.14 - TypeError Unsupported operand type string for addition
'foo' + '0' - TypeError Unsupported operand type string for addition
'foo' + '10' - TypeError Unsupported operand type string for addition
'foo' + '010' - TypeError Unsupported operand type string for addition
'foo' + '10 elephants' - TypeError Unsupported operand type string for addition
'foo' + 'foo' - TypeError Unsupported operand type string for addition
'foo' + array ( ) - TypeError Unsupported operand type string for addition
'foo' + array ( 0 => 1 ) - TypeError Unsupported operand type string for addition
'foo' + array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for addition
'foo' + array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for addition
'foo' + array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for addition
'foo' + (object) array ( ) - TypeError Unsupported operand type string for addition
'foo' + (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for addition
'foo' + (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for addition
'foo' + DateTime - TypeError Unsupported operand type string for addition
'foo' + resource - TypeError Unsupported operand type string for addition
'foo' + NULL - TypeError Unsupported operand type string for addition
array ( ) + false - TypeError Unsupported operand type array for addition
array ( ) + true - TypeError Unsupported operand type array for addition
array ( ) + 0 - TypeError Unsupported operand type array for addition
array ( ) + 10 - TypeError Unsupported operand type array for addition
array ( ) + 0.0 - TypeError Unsupported operand type array for addition
array ( ) + 10.0 - TypeError Unsupported operand type array for addition
array ( ) + 3.14 - TypeError Unsupported operand type array for addition
array ( ) + '0' - TypeError Unsupported operand type array for addition
array ( ) + '10' - TypeError Unsupported operand type array for addition
array ( ) + '010' - TypeError Unsupported operand type array for addition
array ( ) + '10 elephants' - TypeError Unsupported operand type array for addition
array ( ) + 'foo' - TypeError Unsupported operand type array for addition
array ( ) + array ( ) = array ( )
array ( ) + array ( 0 => 1 ) = array ( 0 => 1 )
array ( ) + array ( 0 => 1, 1 => 100 ) = array ( 0 => 1, 1 => 100 )
array ( ) + array ( 'foo' => 1, 'bar' => 2 ) = array ( 'foo' => 1, 'bar' => 2 )
array ( ) + array ( 'bar' => 1, 'foo' => 2 ) = array ( 'bar' => 1, 'foo' => 2 )
array ( ) + (object) array ( ) - TypeError Unsupported operand type array for addition
array ( ) + (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for addition
array ( ) + (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for addition
array ( ) + DateTime - TypeError Unsupported operand type array for addition
array ( ) + resource - TypeError Unsupported operand type array for addition
array ( ) + NULL - TypeError Unsupported operand type array for addition
array ( 0 => 1 ) + false - TypeError Unsupported operand type array for addition
array ( 0 => 1 ) + true - TypeError Unsupported operand type array for addition
array ( 0 => 1 ) + 0 - TypeError Unsupported operand type array for addition
array ( 0 => 1 ) + 10 - TypeError Unsupported operand type array for addition
array ( 0 => 1 ) + 0.0 - TypeError Unsupported operand type array for addition
array ( 0 => 1 ) + 10.0 - TypeError Unsupported operand type array for addition
array ( 0 => 1 ) + 3.14 - TypeError Unsupported operand type array for addition
array ( 0 => 1 ) + '0' - TypeError Unsupported operand type array for addition
array ( 0 => 1 ) + '10' - TypeError Unsupported operand type array for addition
array ( 0 => 1 ) + '010' - TypeError Unsupported operand type array for addition
array ( 0 => 1 ) + '10 elephants' - TypeError Unsupported operand type array for addition
array ( 0 => 1 ) + 'foo' - TypeError Unsupported operand type array for addition
array ( 0 => 1 ) + array ( ) = array ( 0 => 1 )
array ( 0 => 1 ) + array ( 0 => 1 ) = array ( 0 => 1 )
array ( 0 => 1 ) + array ( 0 => 1, 1 => 100 ) = array ( 0 => 1, 1 => 100 )
array ( 0 => 1 ) + array ( 'foo' => 1, 'bar' => 2 ) = array ( 0 => 1, 'foo' => 1, 'bar' => 2 )
array ( 0 => 1 ) + array ( 'bar' => 1, 'foo' => 2 ) = array ( 0 => 1, 'bar' => 1, 'foo' => 2 )
array ( 0 => 1 ) + (object) array ( ) - TypeError Unsupported operand type array for addition
array ( 0 => 1 ) + (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for addition
array ( 0 => 1 ) + (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for addition
array ( 0 => 1 ) + DateTime - TypeError Unsupported operand type array for addition
array ( 0 => 1 ) + resource - TypeError Unsupported operand type array for addition
array ( 0 => 1 ) + NULL - TypeError Unsupported operand type array for addition
array ( 0 => 1, 1 => 100 ) + false - TypeError Unsupported operand type array for addition
array ( 0 => 1, 1 => 100 ) + true - TypeError Unsupported operand type array for addition
array ( 0 => 1, 1 => 100 ) + 0 - TypeError Unsupported operand type array for addition
array ( 0 => 1, 1 => 100 ) + 10 - TypeError Unsupported operand type array for addition
array ( 0 => 1, 1 => 100 ) + 0.0 - TypeError Unsupported operand type array for addition
array ( 0 => 1, 1 => 100 ) + 10.0 - TypeError Unsupported operand type array for addition
array ( 0 => 1, 1 => 100 ) + 3.14 - TypeError Unsupported operand type array for addition
array ( 0 => 1, 1 => 100 ) + '0' - TypeError Unsupported operand type array for addition
array ( 0 => 1, 1 => 100 ) + '10' - TypeError Unsupported operand type array for addition
array ( 0 => 1, 1 => 100 ) + '010' - TypeError Unsupported operand type array for addition
array ( 0 => 1, 1 => 100 ) + '10 elephants' - TypeError Unsupported operand type array for addition
array ( 0 => 1, 1 => 100 ) + 'foo' - TypeError Unsupported operand type array for addition
array ( 0 => 1, 1 => 100 ) + array ( ) = array ( 0 => 1, 1 => 100 )
array ( 0 => 1, 1 => 100 ) + array ( 0 => 1 ) = array ( 0 => 1, 1 => 100 )
array ( 0 => 1, 1 => 100 ) + array ( 0 => 1, 1 => 100 ) = array ( 0 => 1, 1 => 100 )
array ( 0 => 1, 1 => 100 ) + array ( 'foo' => 1, 'bar' => 2 ) = array ( 0 => 1, 1 => 100, 'foo' => 1, 'bar' => 2 )
array ( 0 => 1, 1 => 100 ) + array ( 'bar' => 1, 'foo' => 2 ) = array ( 0 => 1, 1 => 100, 'bar' => 1, 'foo' => 2 )
array ( 0 => 1, 1 => 100 ) + (object) array ( ) - TypeError Unsupported operand type array for addition
array ( 0 => 1, 1 => 100 ) + (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for addition
array ( 0 => 1, 1 => 100 ) + (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for addition
array ( 0 => 1, 1 => 100 ) + DateTime - TypeError Unsupported operand type array for addition
array ( 0 => 1, 1 => 100 ) + resource - TypeError Unsupported operand type array for addition
array ( 0 => 1, 1 => 100 ) + NULL - TypeError Unsupported operand type array for addition
array ( 'foo' => 1, 'bar' => 2 ) + false - TypeError Unsupported operand type array for addition
array ( 'foo' => 1, 'bar' => 2 ) + true - TypeError Unsupported operand type array for addition
array ( 'foo' => 1, 'bar' => 2 ) + 0 - TypeError Unsupported operand type array for addition
array ( 'foo' => 1, 'bar' => 2 ) + 10 - TypeError Unsupported operand type array for addition
array ( 'foo' => 1, 'bar' => 2 ) + 0.0 - TypeError Unsupported operand type array for addition
array ( 'foo' => 1, 'bar' => 2 ) + 10.0 - TypeError Unsupported operand type array for addition
array ( 'foo' => 1, 'bar' => 2 ) + 3.14 - TypeError Unsupported operand type array for addition
array ( 'foo' => 1, 'bar' => 2 ) + '0' - TypeError Unsupported operand type array for addition
array ( 'foo' => 1, 'bar' => 2 ) + '10' - TypeError Unsupported operand type array for addition
array ( 'foo' => 1, 'bar' => 2 ) + '010' - TypeError Unsupported operand type array for addition
array ( 'foo' => 1, 'bar' => 2 ) + '10 elephants' - TypeError Unsupported operand type array for addition
array ( 'foo' => 1, 'bar' => 2 ) + 'foo' - TypeError Unsupported operand type array for addition
array ( 'foo' => 1, 'bar' => 2 ) + array ( ) = array ( 'foo' => 1, 'bar' => 2 )
array ( 'foo' => 1, 'bar' => 2 ) + array ( 0 => 1 ) = array ( 'foo' => 1, 'bar' => 2, 0 => 1 )
array ( 'foo' => 1, 'bar' => 2 ) + array ( 0 => 1, 1 => 100 ) = array ( 'foo' => 1, 'bar' => 2, 0 => 1, 1 => 100 )
array ( 'foo' => 1, 'bar' => 2 ) + array ( 'foo' => 1, 'bar' => 2 ) = array ( 'foo' => 1, 'bar' => 2 )
array ( 'foo' => 1, 'bar' => 2 ) + array ( 'bar' => 1, 'foo' => 2 ) = array ( 'foo' => 1, 'bar' => 2 )
array ( 'foo' => 1, 'bar' => 2 ) + (object) array ( ) - TypeError Unsupported operand type array for addition
array ( 'foo' => 1, 'bar' => 2 ) + (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for addition
array ( 'foo' => 1, 'bar' => 2 ) + (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for addition
array ( 'foo' => 1, 'bar' => 2 ) + DateTime - TypeError Unsupported operand type array for addition
array ( 'foo' => 1, 'bar' => 2 ) + resource - TypeError Unsupported operand type array for addition
array ( 'foo' => 1, 'bar' => 2 ) + NULL - TypeError Unsupported operand type array for addition
array ( 'bar' => 1, 'foo' => 2 ) + false - TypeError Unsupported operand type array for addition
array ( 'bar' => 1, 'foo' => 2 ) + true - TypeError Unsupported operand type array for addition
array ( 'bar' => 1, 'foo' => 2 ) + 0 - TypeError Unsupported operand type array for addition
array ( 'bar' => 1, 'foo' => 2 ) + 10 - TypeError Unsupported operand type array for addition
array ( 'bar' => 1, 'foo' => 2 ) + 0.0 - TypeError Unsupported operand type array for addition
array ( 'bar' => 1, 'foo' => 2 ) + 10.0 - TypeError Unsupported operand type array for addition
array ( 'bar' => 1, 'foo' => 2 ) + 3.14 - TypeError Unsupported operand type array for addition
array ( 'bar' => 1, 'foo' => 2 ) + '0' - TypeError Unsupported operand type array for addition
array ( 'bar' => 1, 'foo' => 2 ) + '10' - TypeError Unsupported operand type array for addition
array ( 'bar' => 1, 'foo' => 2 ) + '010' - TypeError Unsupported operand type array for addition
array ( 'bar' => 1, 'foo' => 2 ) + '10 elephants' - TypeError Unsupported operand type array for addition
array ( 'bar' => 1, 'foo' => 2 ) + 'foo' - TypeError Unsupported operand type array for addition
array ( 'bar' => 1, 'foo' => 2 ) + array ( ) = array ( 'bar' => 1, 'foo' => 2 )
array ( 'bar' => 1, 'foo' => 2 ) + array ( 0 => 1 ) = array ( 'bar' => 1, 'foo' => 2, 0 => 1 )
array ( 'bar' => 1, 'foo' => 2 ) + array ( 0 => 1, 1 => 100 ) = array ( 'bar' => 1, 'foo' => 2, 0 => 1, 1 => 100 )
array ( 'bar' => 1, 'foo' => 2 ) + array ( 'foo' => 1, 'bar' => 2 ) = array ( 'bar' => 1, 'foo' => 2 )
array ( 'bar' => 1, 'foo' => 2 ) + array ( 'bar' => 1, 'foo' => 2 ) = array ( 'bar' => 1, 'foo' => 2 )
array ( 'bar' => 1, 'foo' => 2 ) + (object) array ( ) - TypeError Unsupported operand type array for addition
array ( 'bar' => 1, 'foo' => 2 ) + (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for addition
array ( 'bar' => 1, 'foo' => 2 ) + (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for addition
array ( 'bar' => 1, 'foo' => 2 ) + DateTime - TypeError Unsupported operand type array for addition
array ( 'bar' => 1, 'foo' => 2 ) + resource - TypeError Unsupported operand type array for addition
array ( 'bar' => 1, 'foo' => 2 ) + NULL - TypeError Unsupported operand type array for addition
(object) array ( ) + false - TypeError Unsupported operand type object for addition
(object) array ( ) + true - TypeError Unsupported operand type object for addition
(object) array ( ) + 0 - TypeError Unsupported operand type object for addition
(object) array ( ) + 10 - TypeError Unsupported operand type object for addition
(object) array ( ) + 0.0 - TypeError Unsupported operand type object for addition
(object) array ( ) + 10.0 - TypeError Unsupported operand type object for addition
(object) array ( ) + 3.14 - TypeError Unsupported operand type object for addition
(object) array ( ) + '0' - TypeError Unsupported operand type object for addition
(object) array ( ) + '10' - TypeError Unsupported operand type object for addition
(object) array ( ) + '010' - TypeError Unsupported operand type object for addition
(object) array ( ) + '10 elephants' - TypeError Unsupported operand type object for addition
(object) array ( ) + 'foo' - TypeError Unsupported operand type object for addition
(object) array ( ) + array ( ) - TypeError Unsupported operand type object for addition
(object) array ( ) + array ( 0 => 1 ) - TypeError Unsupported operand type object for addition
(object) array ( ) + array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for addition
(object) array ( ) + array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for addition
(object) array ( ) + array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for addition
(object) array ( ) + (object) array ( ) - TypeError Unsupported operand type object for addition
(object) array ( ) + (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for addition
(object) array ( ) + (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for addition
(object) array ( ) + DateTime - TypeError Unsupported operand type object for addition
(object) array ( ) + resource - TypeError Unsupported operand type object for addition
(object) array ( ) + NULL - TypeError Unsupported operand type object for addition
(object) array ( 'foo' => 1, 'bar' => 2 ) + false - TypeError Unsupported operand type object for addition
(object) array ( 'foo' => 1, 'bar' => 2 ) + true - TypeError Unsupported operand type object for addition
(object) array ( 'foo' => 1, 'bar' => 2 ) + 0 - TypeError Unsupported operand type object for addition
(object) array ( 'foo' => 1, 'bar' => 2 ) + 10 - TypeError Unsupported operand type object for addition
(object) array ( 'foo' => 1, 'bar' => 2 ) + 0.0 - TypeError Unsupported operand type object for addition
(object) array ( 'foo' => 1, 'bar' => 2 ) + 10.0 - TypeError Unsupported operand type object for addition
(object) array ( 'foo' => 1, 'bar' => 2 ) + 3.14 - TypeError Unsupported operand type object for addition
(object) array ( 'foo' => 1, 'bar' => 2 ) + '0' - TypeError Unsupported operand type object for addition
(object) array ( 'foo' => 1, 'bar' => 2 ) + '10' - TypeError Unsupported operand type object for addition
(object) array ( 'foo' => 1, 'bar' => 2 ) + '010' - TypeError Unsupported operand type object for addition
(object) array ( 'foo' => 1, 'bar' => 2 ) + '10 elephants' - TypeError Unsupported operand type object for addition
(object) array ( 'foo' => 1, 'bar' => 2 ) + 'foo' - TypeError Unsupported operand type object for addition
(object) array ( 'foo' => 1, 'bar' => 2 ) + array ( ) - TypeError Unsupported operand type object for addition
(object) array ( 'foo' => 1, 'bar' => 2 ) + array ( 0 => 1 ) - TypeError Unsupported operand type object for addition
(object) array ( 'foo' => 1, 'bar' => 2 ) + array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for addition
(object) array ( 'foo' => 1, 'bar' => 2 ) + array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for addition
(object) array ( 'foo' => 1, 'bar' => 2 ) + array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for addition
(object) array ( 'foo' => 1, 'bar' => 2 ) + (object) array ( ) - TypeError Unsupported operand type object for addition
(object) array ( 'foo' => 1, 'bar' => 2 ) + (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for addition
(object) array ( 'foo' => 1, 'bar' => 2 ) + (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for addition
(object) array ( 'foo' => 1, 'bar' => 2 ) + DateTime - TypeError Unsupported operand type object for addition
(object) array ( 'foo' => 1, 'bar' => 2 ) + resource - TypeError Unsupported operand type object for addition
(object) array ( 'foo' => 1, 'bar' => 2 ) + NULL - TypeError Unsupported operand type object for addition
(object) array ( 'bar' => 1, 'foo' => 2 ) + false - TypeError Unsupported operand type object for addition
(object) array ( 'bar' => 1, 'foo' => 2 ) + true - TypeError Unsupported operand type object for addition
(object) array ( 'bar' => 1, 'foo' => 2 ) + 0 - TypeError Unsupported operand type object for addition
(object) array ( 'bar' => 1, 'foo' => 2 ) + 10 - TypeError Unsupported operand type object for addition
(object) array ( 'bar' => 1, 'foo' => 2 ) + 0.0 - TypeError Unsupported operand type object for addition
(object) array ( 'bar' => 1, 'foo' => 2 ) + 10.0 - TypeError Unsupported operand type object for addition
(object) array ( 'bar' => 1, 'foo' => 2 ) + 3.14 - TypeError Unsupported operand type object for addition
(object) array ( 'bar' => 1, 'foo' => 2 ) + '0' - TypeError Unsupported operand type object for addition
(object) array ( 'bar' => 1, 'foo' => 2 ) + '10' - TypeError Unsupported operand type object for addition
(object) array ( 'bar' => 1, 'foo' => 2 ) + '010' - TypeError Unsupported operand type object for addition
(object) array ( 'bar' => 1, 'foo' => 2 ) + '10 elephants' - TypeError Unsupported operand type object for addition
(object) array ( 'bar' => 1, 'foo' => 2 ) + 'foo' - TypeError Unsupported operand type object for addition
(object) array ( 'bar' => 1, 'foo' => 2 ) + array ( ) - TypeError Unsupported operand type object for addition
(object) array ( 'bar' => 1, 'foo' => 2 ) + array ( 0 => 1 ) - TypeError Unsupported operand type object for addition
(object) array ( 'bar' => 1, 'foo' => 2 ) + array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for addition
(object) array ( 'bar' => 1, 'foo' => 2 ) + array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for addition
(object) array ( 'bar' => 1, 'foo' => 2 ) + array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for addition
(object) array ( 'bar' => 1, 'foo' => 2 ) + (object) array ( ) - TypeError Unsupported operand type object for addition
(object) array ( 'bar' => 1, 'foo' => 2 ) + (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for addition
(object) array ( 'bar' => 1, 'foo' => 2 ) + (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for addition
(object) array ( 'bar' => 1, 'foo' => 2 ) + DateTime - TypeError Unsupported operand type object for addition
(object) array ( 'bar' => 1, 'foo' => 2 ) + resource - TypeError Unsupported operand type object for addition
(object) array ( 'bar' => 1, 'foo' => 2 ) + NULL - TypeError Unsupported operand type object for addition
DateTime + false - TypeError Unsupported operand type object for addition
DateTime + true - TypeError Unsupported operand type object for addition
DateTime + 0 - TypeError Unsupported operand type object for addition
DateTime + 10 - TypeError Unsupported operand type object for addition
DateTime + 0.0 - TypeError Unsupported operand type object for addition
DateTime + 10.0 - TypeError Unsupported operand type object for addition
DateTime + 3.14 - TypeError Unsupported operand type object for addition
DateTime + '0' - TypeError Unsupported operand type object for addition
DateTime + '10' - TypeError Unsupported operand type object for addition
DateTime + '010' - TypeError Unsupported operand type object for addition
DateTime + '10 elephants' - TypeError Unsupported operand type object for addition
DateTime + 'foo' - TypeError Unsupported operand type object for addition
DateTime + array ( ) - TypeError Unsupported operand type object for addition
DateTime + array ( 0 => 1 ) - TypeError Unsupported operand type object for addition
DateTime + array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for addition
DateTime + array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for addition
DateTime + array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for addition
DateTime + (object) array ( ) - TypeError Unsupported operand type object for addition
DateTime + (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for addition
DateTime + (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for addition
DateTime + DateTime - TypeError Unsupported operand type object for addition
DateTime + resource - TypeError Unsupported operand type object for addition
DateTime + NULL - TypeError Unsupported operand type object for addition
resource + false - TypeError Unsupported operand type resource for addition
resource + true - TypeError Unsupported operand type resource for addition
resource + 0 - TypeError Unsupported operand type resource for addition
resource + 10 - TypeError Unsupported operand type resource for addition
resource + 0.0 - TypeError Unsupported operand type resource for addition
resource + 10.0 - TypeError Unsupported operand type resource for addition
resource + 3.14 - TypeError Unsupported operand type resource for addition
resource + '0' - TypeError Unsupported operand type resource for addition
resource + '10' - TypeError Unsupported operand type resource for addition
resource + '010' - TypeError Unsupported operand type resource for addition
resource + '10 elephants' - TypeError Unsupported operand type resource for addition
resource + 'foo' - TypeError Unsupported operand type resource for addition
resource + array ( ) - TypeError Unsupported operand type resource for addition
resource + array ( 0 => 1 ) - TypeError Unsupported operand type resource for addition
resource + array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type resource for addition
resource + array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for addition
resource + array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for addition
resource + (object) array ( ) - TypeError Unsupported operand type resource for addition
resource + (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for addition
resource + (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for addition
resource + DateTime - TypeError Unsupported operand type resource for addition
resource + resource - TypeError Unsupported operand type resource for addition
resource + NULL - TypeError Unsupported operand type resource for addition
NULL + false - TypeError Unsupported operand type null for addition
NULL + true - TypeError Unsupported operand type null for addition
NULL + 0 - TypeError Unsupported operand type null for addition
NULL + 10 - TypeError Unsupported operand type null for addition
NULL + 0.0 - TypeError Unsupported operand type null for addition
NULL + 10.0 - TypeError Unsupported operand type null for addition
NULL + 3.14 - TypeError Unsupported operand type null for addition
NULL + '0' - TypeError Unsupported operand type null for addition
NULL + '10' - TypeError Unsupported operand type null for addition
NULL + '010' - TypeError Unsupported operand type null for addition
NULL + '10 elephants' - TypeError Unsupported operand type null for addition
NULL + 'foo' - TypeError Unsupported operand type null for addition
NULL + array ( ) - TypeError Unsupported operand type null for addition
NULL + array ( 0 => 1 ) - TypeError Unsupported operand type null for addition
NULL + array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type null for addition
NULL + array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for addition
NULL + array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for addition
NULL + (object) array ( ) - TypeError Unsupported operand type null for addition
NULL + (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for addition
NULL + (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for addition
NULL + DateTime - TypeError Unsupported operand type null for addition
NULL + resource - TypeError Unsupported operand type null for addition
NULL + NULL - TypeError Unsupported operand type null for addition