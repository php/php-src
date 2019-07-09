--TEST--
modulo '%' operator with strict_operators
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a % $b', function($a, $b) { return $a % $b; });

--EXPECT--
false % false - TypeError Unsupported operand type bool for modulo
false % true - TypeError Unsupported operand type bool for modulo
false % 0 - TypeError Unsupported operand type bool for modulo
false % 10 - TypeError Unsupported operand type bool for modulo
false % 0.0 - TypeError Unsupported operand type bool for modulo
false % 10.0 - TypeError Unsupported operand type bool for modulo
false % 3.14 - TypeError Unsupported operand type bool for modulo
false % '0' - TypeError Unsupported operand type bool for modulo
false % '10' - TypeError Unsupported operand type bool for modulo
false % '010' - TypeError Unsupported operand type bool for modulo
false % '10 elephants' - TypeError Unsupported operand type bool for modulo
false % 'foo' - TypeError Unsupported operand type bool for modulo
false % array ( ) - TypeError Unsupported operand type bool for modulo
false % array ( 0 => 1 ) - TypeError Unsupported operand type bool for modulo
false % array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for modulo
false % array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for modulo
false % array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for modulo
false % (object) array ( ) - TypeError Unsupported operand type bool for modulo
false % (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for modulo
false % (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for modulo
false % DateTime - TypeError Unsupported operand type bool for modulo
false % resource - TypeError Unsupported operand type bool for modulo
false % NULL - TypeError Unsupported operand type bool for modulo
true % false - TypeError Unsupported operand type bool for modulo
true % true - TypeError Unsupported operand type bool for modulo
true % 0 - TypeError Unsupported operand type bool for modulo
true % 10 - TypeError Unsupported operand type bool for modulo
true % 0.0 - TypeError Unsupported operand type bool for modulo
true % 10.0 - TypeError Unsupported operand type bool for modulo
true % 3.14 - TypeError Unsupported operand type bool for modulo
true % '0' - TypeError Unsupported operand type bool for modulo
true % '10' - TypeError Unsupported operand type bool for modulo
true % '010' - TypeError Unsupported operand type bool for modulo
true % '10 elephants' - TypeError Unsupported operand type bool for modulo
true % 'foo' - TypeError Unsupported operand type bool for modulo
true % array ( ) - TypeError Unsupported operand type bool for modulo
true % array ( 0 => 1 ) - TypeError Unsupported operand type bool for modulo
true % array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for modulo
true % array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for modulo
true % array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for modulo
true % (object) array ( ) - TypeError Unsupported operand type bool for modulo
true % (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for modulo
true % (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for modulo
true % DateTime - TypeError Unsupported operand type bool for modulo
true % resource - TypeError Unsupported operand type bool for modulo
true % NULL - TypeError Unsupported operand type bool for modulo
0 % false - TypeError Unsupported operand type bool for modulo
0 % true - TypeError Unsupported operand type bool for modulo
0 % 0 - DivisionByZeroError Modulo by zero
0 % 10 = 0
0 % 0.0 - TypeError Unsupported operand type float for modulo
0 % 10.0 - TypeError Unsupported operand type float for modulo
0 % 3.14 - TypeError Unsupported operand type float for modulo
0 % '0' - TypeError Unsupported operand type string for modulo
0 % '10' - TypeError Unsupported operand type string for modulo
0 % '010' - TypeError Unsupported operand type string for modulo
0 % '10 elephants' - TypeError Unsupported operand type string for modulo
0 % 'foo' - TypeError Unsupported operand type string for modulo
0 % array ( ) - TypeError Unsupported operand type array for modulo
0 % array ( 0 => 1 ) - TypeError Unsupported operand type array for modulo
0 % array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for modulo
0 % array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for modulo
0 % array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for modulo
0 % (object) array ( ) - TypeError Unsupported operand type object for modulo
0 % (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for modulo
0 % (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for modulo
0 % DateTime - TypeError Unsupported operand type object for modulo
0 % resource - TypeError Unsupported operand type resource for modulo
0 % NULL - TypeError Unsupported operand type null for modulo
10 % false - TypeError Unsupported operand type bool for modulo
10 % true - TypeError Unsupported operand type bool for modulo
10 % 0 - DivisionByZeroError Modulo by zero
10 % 10 = 0
10 % 0.0 - TypeError Unsupported operand type float for modulo
10 % 10.0 - TypeError Unsupported operand type float for modulo
10 % 3.14 - TypeError Unsupported operand type float for modulo
10 % '0' - TypeError Unsupported operand type string for modulo
10 % '10' - TypeError Unsupported operand type string for modulo
10 % '010' - TypeError Unsupported operand type string for modulo
10 % '10 elephants' - TypeError Unsupported operand type string for modulo
10 % 'foo' - TypeError Unsupported operand type string for modulo
10 % array ( ) - TypeError Unsupported operand type array for modulo
10 % array ( 0 => 1 ) - TypeError Unsupported operand type array for modulo
10 % array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for modulo
10 % array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for modulo
10 % array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for modulo
10 % (object) array ( ) - TypeError Unsupported operand type object for modulo
10 % (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for modulo
10 % (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for modulo
10 % DateTime - TypeError Unsupported operand type object for modulo
10 % resource - TypeError Unsupported operand type resource for modulo
10 % NULL - TypeError Unsupported operand type null for modulo
0.0 % false - TypeError Unsupported operand type bool for modulo
0.0 % true - TypeError Unsupported operand type bool for modulo
0.0 % 0 - TypeError Unsupported operand type int for modulo
0.0 % 10 - TypeError Unsupported operand type int for modulo
0.0 % 0.0 - TypeError Unsupported operand type float for modulo
0.0 % 10.0 - TypeError Unsupported operand type float for modulo
0.0 % 3.14 - TypeError Unsupported operand type float for modulo
0.0 % '0' - TypeError Unsupported operand type string for modulo
0.0 % '10' - TypeError Unsupported operand type string for modulo
0.0 % '010' - TypeError Unsupported operand type string for modulo
0.0 % '10 elephants' - TypeError Unsupported operand type string for modulo
0.0 % 'foo' - TypeError Unsupported operand type string for modulo
0.0 % array ( ) - TypeError Unsupported operand type array for modulo
0.0 % array ( 0 => 1 ) - TypeError Unsupported operand type array for modulo
0.0 % array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for modulo
0.0 % array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for modulo
0.0 % array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for modulo
0.0 % (object) array ( ) - TypeError Unsupported operand type object for modulo
0.0 % (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for modulo
0.0 % (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for modulo
0.0 % DateTime - TypeError Unsupported operand type object for modulo
0.0 % resource - TypeError Unsupported operand type resource for modulo
0.0 % NULL - TypeError Unsupported operand type null for modulo
10.0 % false - TypeError Unsupported operand type bool for modulo
10.0 % true - TypeError Unsupported operand type bool for modulo
10.0 % 0 - TypeError Unsupported operand type int for modulo
10.0 % 10 - TypeError Unsupported operand type int for modulo
10.0 % 0.0 - TypeError Unsupported operand type float for modulo
10.0 % 10.0 - TypeError Unsupported operand type float for modulo
10.0 % 3.14 - TypeError Unsupported operand type float for modulo
10.0 % '0' - TypeError Unsupported operand type string for modulo
10.0 % '10' - TypeError Unsupported operand type string for modulo
10.0 % '010' - TypeError Unsupported operand type string for modulo
10.0 % '10 elephants' - TypeError Unsupported operand type string for modulo
10.0 % 'foo' - TypeError Unsupported operand type string for modulo
10.0 % array ( ) - TypeError Unsupported operand type array for modulo
10.0 % array ( 0 => 1 ) - TypeError Unsupported operand type array for modulo
10.0 % array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for modulo
10.0 % array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for modulo
10.0 % array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for modulo
10.0 % (object) array ( ) - TypeError Unsupported operand type object for modulo
10.0 % (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for modulo
10.0 % (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for modulo
10.0 % DateTime - TypeError Unsupported operand type object for modulo
10.0 % resource - TypeError Unsupported operand type resource for modulo
10.0 % NULL - TypeError Unsupported operand type null for modulo
3.14 % false - TypeError Unsupported operand type bool for modulo
3.14 % true - TypeError Unsupported operand type bool for modulo
3.14 % 0 - TypeError Unsupported operand type int for modulo
3.14 % 10 - TypeError Unsupported operand type int for modulo
3.14 % 0.0 - TypeError Unsupported operand type float for modulo
3.14 % 10.0 - TypeError Unsupported operand type float for modulo
3.14 % 3.14 - TypeError Unsupported operand type float for modulo
3.14 % '0' - TypeError Unsupported operand type string for modulo
3.14 % '10' - TypeError Unsupported operand type string for modulo
3.14 % '010' - TypeError Unsupported operand type string for modulo
3.14 % '10 elephants' - TypeError Unsupported operand type string for modulo
3.14 % 'foo' - TypeError Unsupported operand type string for modulo
3.14 % array ( ) - TypeError Unsupported operand type array for modulo
3.14 % array ( 0 => 1 ) - TypeError Unsupported operand type array for modulo
3.14 % array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for modulo
3.14 % array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for modulo
3.14 % array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for modulo
3.14 % (object) array ( ) - TypeError Unsupported operand type object for modulo
3.14 % (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for modulo
3.14 % (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for modulo
3.14 % DateTime - TypeError Unsupported operand type object for modulo
3.14 % resource - TypeError Unsupported operand type resource for modulo
3.14 % NULL - TypeError Unsupported operand type null for modulo
'0' % false - TypeError Unsupported operand type string for modulo
'0' % true - TypeError Unsupported operand type string for modulo
'0' % 0 - TypeError Unsupported operand type string for modulo
'0' % 10 - TypeError Unsupported operand type string for modulo
'0' % 0.0 - TypeError Unsupported operand type string for modulo
'0' % 10.0 - TypeError Unsupported operand type string for modulo
'0' % 3.14 - TypeError Unsupported operand type string for modulo
'0' % '0' - TypeError Unsupported operand type string for modulo
'0' % '10' - TypeError Unsupported operand type string for modulo
'0' % '010' - TypeError Unsupported operand type string for modulo
'0' % '10 elephants' - TypeError Unsupported operand type string for modulo
'0' % 'foo' - TypeError Unsupported operand type string for modulo
'0' % array ( ) - TypeError Unsupported operand type string for modulo
'0' % array ( 0 => 1 ) - TypeError Unsupported operand type string for modulo
'0' % array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for modulo
'0' % array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for modulo
'0' % array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for modulo
'0' % (object) array ( ) - TypeError Unsupported operand type string for modulo
'0' % (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for modulo
'0' % (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for modulo
'0' % DateTime - TypeError Unsupported operand type string for modulo
'0' % resource - TypeError Unsupported operand type string for modulo
'0' % NULL - TypeError Unsupported operand type string for modulo
'10' % false - TypeError Unsupported operand type string for modulo
'10' % true - TypeError Unsupported operand type string for modulo
'10' % 0 - TypeError Unsupported operand type string for modulo
'10' % 10 - TypeError Unsupported operand type string for modulo
'10' % 0.0 - TypeError Unsupported operand type string for modulo
'10' % 10.0 - TypeError Unsupported operand type string for modulo
'10' % 3.14 - TypeError Unsupported operand type string for modulo
'10' % '0' - TypeError Unsupported operand type string for modulo
'10' % '10' - TypeError Unsupported operand type string for modulo
'10' % '010' - TypeError Unsupported operand type string for modulo
'10' % '10 elephants' - TypeError Unsupported operand type string for modulo
'10' % 'foo' - TypeError Unsupported operand type string for modulo
'10' % array ( ) - TypeError Unsupported operand type string for modulo
'10' % array ( 0 => 1 ) - TypeError Unsupported operand type string for modulo
'10' % array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for modulo
'10' % array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for modulo
'10' % array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for modulo
'10' % (object) array ( ) - TypeError Unsupported operand type string for modulo
'10' % (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for modulo
'10' % (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for modulo
'10' % DateTime - TypeError Unsupported operand type string for modulo
'10' % resource - TypeError Unsupported operand type string for modulo
'10' % NULL - TypeError Unsupported operand type string for modulo
'010' % false - TypeError Unsupported operand type string for modulo
'010' % true - TypeError Unsupported operand type string for modulo
'010' % 0 - TypeError Unsupported operand type string for modulo
'010' % 10 - TypeError Unsupported operand type string for modulo
'010' % 0.0 - TypeError Unsupported operand type string for modulo
'010' % 10.0 - TypeError Unsupported operand type string for modulo
'010' % 3.14 - TypeError Unsupported operand type string for modulo
'010' % '0' - TypeError Unsupported operand type string for modulo
'010' % '10' - TypeError Unsupported operand type string for modulo
'010' % '010' - TypeError Unsupported operand type string for modulo
'010' % '10 elephants' - TypeError Unsupported operand type string for modulo
'010' % 'foo' - TypeError Unsupported operand type string for modulo
'010' % array ( ) - TypeError Unsupported operand type string for modulo
'010' % array ( 0 => 1 ) - TypeError Unsupported operand type string for modulo
'010' % array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for modulo
'010' % array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for modulo
'010' % array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for modulo
'010' % (object) array ( ) - TypeError Unsupported operand type string for modulo
'010' % (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for modulo
'010' % (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for modulo
'010' % DateTime - TypeError Unsupported operand type string for modulo
'010' % resource - TypeError Unsupported operand type string for modulo
'010' % NULL - TypeError Unsupported operand type string for modulo
'10 elephants' % false - TypeError Unsupported operand type string for modulo
'10 elephants' % true - TypeError Unsupported operand type string for modulo
'10 elephants' % 0 - TypeError Unsupported operand type string for modulo
'10 elephants' % 10 - TypeError Unsupported operand type string for modulo
'10 elephants' % 0.0 - TypeError Unsupported operand type string for modulo
'10 elephants' % 10.0 - TypeError Unsupported operand type string for modulo
'10 elephants' % 3.14 - TypeError Unsupported operand type string for modulo
'10 elephants' % '0' - TypeError Unsupported operand type string for modulo
'10 elephants' % '10' - TypeError Unsupported operand type string for modulo
'10 elephants' % '010' - TypeError Unsupported operand type string for modulo
'10 elephants' % '10 elephants' - TypeError Unsupported operand type string for modulo
'10 elephants' % 'foo' - TypeError Unsupported operand type string for modulo
'10 elephants' % array ( ) - TypeError Unsupported operand type string for modulo
'10 elephants' % array ( 0 => 1 ) - TypeError Unsupported operand type string for modulo
'10 elephants' % array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for modulo
'10 elephants' % array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for modulo
'10 elephants' % array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for modulo
'10 elephants' % (object) array ( ) - TypeError Unsupported operand type string for modulo
'10 elephants' % (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for modulo
'10 elephants' % (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for modulo
'10 elephants' % DateTime - TypeError Unsupported operand type string for modulo
'10 elephants' % resource - TypeError Unsupported operand type string for modulo
'10 elephants' % NULL - TypeError Unsupported operand type string for modulo
'foo' % false - TypeError Unsupported operand type string for modulo
'foo' % true - TypeError Unsupported operand type string for modulo
'foo' % 0 - TypeError Unsupported operand type string for modulo
'foo' % 10 - TypeError Unsupported operand type string for modulo
'foo' % 0.0 - TypeError Unsupported operand type string for modulo
'foo' % 10.0 - TypeError Unsupported operand type string for modulo
'foo' % 3.14 - TypeError Unsupported operand type string for modulo
'foo' % '0' - TypeError Unsupported operand type string for modulo
'foo' % '10' - TypeError Unsupported operand type string for modulo
'foo' % '010' - TypeError Unsupported operand type string for modulo
'foo' % '10 elephants' - TypeError Unsupported operand type string for modulo
'foo' % 'foo' - TypeError Unsupported operand type string for modulo
'foo' % array ( ) - TypeError Unsupported operand type string for modulo
'foo' % array ( 0 => 1 ) - TypeError Unsupported operand type string for modulo
'foo' % array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type string for modulo
'foo' % array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for modulo
'foo' % array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for modulo
'foo' % (object) array ( ) - TypeError Unsupported operand type string for modulo
'foo' % (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type string for modulo
'foo' % (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type string for modulo
'foo' % DateTime - TypeError Unsupported operand type string for modulo
'foo' % resource - TypeError Unsupported operand type string for modulo
'foo' % NULL - TypeError Unsupported operand type string for modulo
array ( ) % false - TypeError Unsupported operand type array for modulo
array ( ) % true - TypeError Unsupported operand type array for modulo
array ( ) % 0 - TypeError Unsupported operand type array for modulo
array ( ) % 10 - TypeError Unsupported operand type array for modulo
array ( ) % 0.0 - TypeError Unsupported operand type array for modulo
array ( ) % 10.0 - TypeError Unsupported operand type array for modulo
array ( ) % 3.14 - TypeError Unsupported operand type array for modulo
array ( ) % '0' - TypeError Unsupported operand type array for modulo
array ( ) % '10' - TypeError Unsupported operand type array for modulo
array ( ) % '010' - TypeError Unsupported operand type array for modulo
array ( ) % '10 elephants' - TypeError Unsupported operand type array for modulo
array ( ) % 'foo' - TypeError Unsupported operand type array for modulo
array ( ) % array ( ) - TypeError Unsupported operand type array for modulo
array ( ) % array ( 0 => 1 ) - TypeError Unsupported operand type array for modulo
array ( ) % array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for modulo
array ( ) % array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for modulo
array ( ) % array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for modulo
array ( ) % (object) array ( ) - TypeError Unsupported operand type array for modulo
array ( ) % (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for modulo
array ( ) % (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for modulo
array ( ) % DateTime - TypeError Unsupported operand type array for modulo
array ( ) % resource - TypeError Unsupported operand type array for modulo
array ( ) % NULL - TypeError Unsupported operand type array for modulo
array ( 0 => 1 ) % false - TypeError Unsupported operand type array for modulo
array ( 0 => 1 ) % true - TypeError Unsupported operand type array for modulo
array ( 0 => 1 ) % 0 - TypeError Unsupported operand type array for modulo
array ( 0 => 1 ) % 10 - TypeError Unsupported operand type array for modulo
array ( 0 => 1 ) % 0.0 - TypeError Unsupported operand type array for modulo
array ( 0 => 1 ) % 10.0 - TypeError Unsupported operand type array for modulo
array ( 0 => 1 ) % 3.14 - TypeError Unsupported operand type array for modulo
array ( 0 => 1 ) % '0' - TypeError Unsupported operand type array for modulo
array ( 0 => 1 ) % '10' - TypeError Unsupported operand type array for modulo
array ( 0 => 1 ) % '010' - TypeError Unsupported operand type array for modulo
array ( 0 => 1 ) % '10 elephants' - TypeError Unsupported operand type array for modulo
array ( 0 => 1 ) % 'foo' - TypeError Unsupported operand type array for modulo
array ( 0 => 1 ) % array ( ) - TypeError Unsupported operand type array for modulo
array ( 0 => 1 ) % array ( 0 => 1 ) - TypeError Unsupported operand type array for modulo
array ( 0 => 1 ) % array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for modulo
array ( 0 => 1 ) % array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for modulo
array ( 0 => 1 ) % array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for modulo
array ( 0 => 1 ) % (object) array ( ) - TypeError Unsupported operand type array for modulo
array ( 0 => 1 ) % (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for modulo
array ( 0 => 1 ) % (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for modulo
array ( 0 => 1 ) % DateTime - TypeError Unsupported operand type array for modulo
array ( 0 => 1 ) % resource - TypeError Unsupported operand type array for modulo
array ( 0 => 1 ) % NULL - TypeError Unsupported operand type array for modulo
array ( 0 => 1, 1 => 100 ) % false - TypeError Unsupported operand type array for modulo
array ( 0 => 1, 1 => 100 ) % true - TypeError Unsupported operand type array for modulo
array ( 0 => 1, 1 => 100 ) % 0 - TypeError Unsupported operand type array for modulo
array ( 0 => 1, 1 => 100 ) % 10 - TypeError Unsupported operand type array for modulo
array ( 0 => 1, 1 => 100 ) % 0.0 - TypeError Unsupported operand type array for modulo
array ( 0 => 1, 1 => 100 ) % 10.0 - TypeError Unsupported operand type array for modulo
array ( 0 => 1, 1 => 100 ) % 3.14 - TypeError Unsupported operand type array for modulo
array ( 0 => 1, 1 => 100 ) % '0' - TypeError Unsupported operand type array for modulo
array ( 0 => 1, 1 => 100 ) % '10' - TypeError Unsupported operand type array for modulo
array ( 0 => 1, 1 => 100 ) % '010' - TypeError Unsupported operand type array for modulo
array ( 0 => 1, 1 => 100 ) % '10 elephants' - TypeError Unsupported operand type array for modulo
array ( 0 => 1, 1 => 100 ) % 'foo' - TypeError Unsupported operand type array for modulo
array ( 0 => 1, 1 => 100 ) % array ( ) - TypeError Unsupported operand type array for modulo
array ( 0 => 1, 1 => 100 ) % array ( 0 => 1 ) - TypeError Unsupported operand type array for modulo
array ( 0 => 1, 1 => 100 ) % array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for modulo
array ( 0 => 1, 1 => 100 ) % array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for modulo
array ( 0 => 1, 1 => 100 ) % array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for modulo
array ( 0 => 1, 1 => 100 ) % (object) array ( ) - TypeError Unsupported operand type array for modulo
array ( 0 => 1, 1 => 100 ) % (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for modulo
array ( 0 => 1, 1 => 100 ) % (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for modulo
array ( 0 => 1, 1 => 100 ) % DateTime - TypeError Unsupported operand type array for modulo
array ( 0 => 1, 1 => 100 ) % resource - TypeError Unsupported operand type array for modulo
array ( 0 => 1, 1 => 100 ) % NULL - TypeError Unsupported operand type array for modulo
array ( 'foo' => 1, 'bar' => 2 ) % false - TypeError Unsupported operand type array for modulo
array ( 'foo' => 1, 'bar' => 2 ) % true - TypeError Unsupported operand type array for modulo
array ( 'foo' => 1, 'bar' => 2 ) % 0 - TypeError Unsupported operand type array for modulo
array ( 'foo' => 1, 'bar' => 2 ) % 10 - TypeError Unsupported operand type array for modulo
array ( 'foo' => 1, 'bar' => 2 ) % 0.0 - TypeError Unsupported operand type array for modulo
array ( 'foo' => 1, 'bar' => 2 ) % 10.0 - TypeError Unsupported operand type array for modulo
array ( 'foo' => 1, 'bar' => 2 ) % 3.14 - TypeError Unsupported operand type array for modulo
array ( 'foo' => 1, 'bar' => 2 ) % '0' - TypeError Unsupported operand type array for modulo
array ( 'foo' => 1, 'bar' => 2 ) % '10' - TypeError Unsupported operand type array for modulo
array ( 'foo' => 1, 'bar' => 2 ) % '010' - TypeError Unsupported operand type array for modulo
array ( 'foo' => 1, 'bar' => 2 ) % '10 elephants' - TypeError Unsupported operand type array for modulo
array ( 'foo' => 1, 'bar' => 2 ) % 'foo' - TypeError Unsupported operand type array for modulo
array ( 'foo' => 1, 'bar' => 2 ) % array ( ) - TypeError Unsupported operand type array for modulo
array ( 'foo' => 1, 'bar' => 2 ) % array ( 0 => 1 ) - TypeError Unsupported operand type array for modulo
array ( 'foo' => 1, 'bar' => 2 ) % array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for modulo
array ( 'foo' => 1, 'bar' => 2 ) % array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for modulo
array ( 'foo' => 1, 'bar' => 2 ) % array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for modulo
array ( 'foo' => 1, 'bar' => 2 ) % (object) array ( ) - TypeError Unsupported operand type array for modulo
array ( 'foo' => 1, 'bar' => 2 ) % (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for modulo
array ( 'foo' => 1, 'bar' => 2 ) % (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for modulo
array ( 'foo' => 1, 'bar' => 2 ) % DateTime - TypeError Unsupported operand type array for modulo
array ( 'foo' => 1, 'bar' => 2 ) % resource - TypeError Unsupported operand type array for modulo
array ( 'foo' => 1, 'bar' => 2 ) % NULL - TypeError Unsupported operand type array for modulo
array ( 'bar' => 1, 'foo' => 2 ) % false - TypeError Unsupported operand type array for modulo
array ( 'bar' => 1, 'foo' => 2 ) % true - TypeError Unsupported operand type array for modulo
array ( 'bar' => 1, 'foo' => 2 ) % 0 - TypeError Unsupported operand type array for modulo
array ( 'bar' => 1, 'foo' => 2 ) % 10 - TypeError Unsupported operand type array for modulo
array ( 'bar' => 1, 'foo' => 2 ) % 0.0 - TypeError Unsupported operand type array for modulo
array ( 'bar' => 1, 'foo' => 2 ) % 10.0 - TypeError Unsupported operand type array for modulo
array ( 'bar' => 1, 'foo' => 2 ) % 3.14 - TypeError Unsupported operand type array for modulo
array ( 'bar' => 1, 'foo' => 2 ) % '0' - TypeError Unsupported operand type array for modulo
array ( 'bar' => 1, 'foo' => 2 ) % '10' - TypeError Unsupported operand type array for modulo
array ( 'bar' => 1, 'foo' => 2 ) % '010' - TypeError Unsupported operand type array for modulo
array ( 'bar' => 1, 'foo' => 2 ) % '10 elephants' - TypeError Unsupported operand type array for modulo
array ( 'bar' => 1, 'foo' => 2 ) % 'foo' - TypeError Unsupported operand type array for modulo
array ( 'bar' => 1, 'foo' => 2 ) % array ( ) - TypeError Unsupported operand type array for modulo
array ( 'bar' => 1, 'foo' => 2 ) % array ( 0 => 1 ) - TypeError Unsupported operand type array for modulo
array ( 'bar' => 1, 'foo' => 2 ) % array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for modulo
array ( 'bar' => 1, 'foo' => 2 ) % array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for modulo
array ( 'bar' => 1, 'foo' => 2 ) % array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for modulo
array ( 'bar' => 1, 'foo' => 2 ) % (object) array ( ) - TypeError Unsupported operand type array for modulo
array ( 'bar' => 1, 'foo' => 2 ) % (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for modulo
array ( 'bar' => 1, 'foo' => 2 ) % (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for modulo
array ( 'bar' => 1, 'foo' => 2 ) % DateTime - TypeError Unsupported operand type array for modulo
array ( 'bar' => 1, 'foo' => 2 ) % resource - TypeError Unsupported operand type array for modulo
array ( 'bar' => 1, 'foo' => 2 ) % NULL - TypeError Unsupported operand type array for modulo
(object) array ( ) % false - TypeError Unsupported operand type object for modulo
(object) array ( ) % true - TypeError Unsupported operand type object for modulo
(object) array ( ) % 0 - TypeError Unsupported operand type object for modulo
(object) array ( ) % 10 - TypeError Unsupported operand type object for modulo
(object) array ( ) % 0.0 - TypeError Unsupported operand type object for modulo
(object) array ( ) % 10.0 - TypeError Unsupported operand type object for modulo
(object) array ( ) % 3.14 - TypeError Unsupported operand type object for modulo
(object) array ( ) % '0' - TypeError Unsupported operand type object for modulo
(object) array ( ) % '10' - TypeError Unsupported operand type object for modulo
(object) array ( ) % '010' - TypeError Unsupported operand type object for modulo
(object) array ( ) % '10 elephants' - TypeError Unsupported operand type object for modulo
(object) array ( ) % 'foo' - TypeError Unsupported operand type object for modulo
(object) array ( ) % array ( ) - TypeError Unsupported operand type object for modulo
(object) array ( ) % array ( 0 => 1 ) - TypeError Unsupported operand type object for modulo
(object) array ( ) % array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for modulo
(object) array ( ) % array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for modulo
(object) array ( ) % array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for modulo
(object) array ( ) % (object) array ( ) - TypeError Unsupported operand type object for modulo
(object) array ( ) % (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for modulo
(object) array ( ) % (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for modulo
(object) array ( ) % DateTime - TypeError Unsupported operand type object for modulo
(object) array ( ) % resource - TypeError Unsupported operand type object for modulo
(object) array ( ) % NULL - TypeError Unsupported operand type object for modulo
(object) array ( 'foo' => 1, 'bar' => 2 ) % false - TypeError Unsupported operand type object for modulo
(object) array ( 'foo' => 1, 'bar' => 2 ) % true - TypeError Unsupported operand type object for modulo
(object) array ( 'foo' => 1, 'bar' => 2 ) % 0 - TypeError Unsupported operand type object for modulo
(object) array ( 'foo' => 1, 'bar' => 2 ) % 10 - TypeError Unsupported operand type object for modulo
(object) array ( 'foo' => 1, 'bar' => 2 ) % 0.0 - TypeError Unsupported operand type object for modulo
(object) array ( 'foo' => 1, 'bar' => 2 ) % 10.0 - TypeError Unsupported operand type object for modulo
(object) array ( 'foo' => 1, 'bar' => 2 ) % 3.14 - TypeError Unsupported operand type object for modulo
(object) array ( 'foo' => 1, 'bar' => 2 ) % '0' - TypeError Unsupported operand type object for modulo
(object) array ( 'foo' => 1, 'bar' => 2 ) % '10' - TypeError Unsupported operand type object for modulo
(object) array ( 'foo' => 1, 'bar' => 2 ) % '010' - TypeError Unsupported operand type object for modulo
(object) array ( 'foo' => 1, 'bar' => 2 ) % '10 elephants' - TypeError Unsupported operand type object for modulo
(object) array ( 'foo' => 1, 'bar' => 2 ) % 'foo' - TypeError Unsupported operand type object for modulo
(object) array ( 'foo' => 1, 'bar' => 2 ) % array ( ) - TypeError Unsupported operand type object for modulo
(object) array ( 'foo' => 1, 'bar' => 2 ) % array ( 0 => 1 ) - TypeError Unsupported operand type object for modulo
(object) array ( 'foo' => 1, 'bar' => 2 ) % array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for modulo
(object) array ( 'foo' => 1, 'bar' => 2 ) % array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for modulo
(object) array ( 'foo' => 1, 'bar' => 2 ) % array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for modulo
(object) array ( 'foo' => 1, 'bar' => 2 ) % (object) array ( ) - TypeError Unsupported operand type object for modulo
(object) array ( 'foo' => 1, 'bar' => 2 ) % (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for modulo
(object) array ( 'foo' => 1, 'bar' => 2 ) % (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for modulo
(object) array ( 'foo' => 1, 'bar' => 2 ) % DateTime - TypeError Unsupported operand type object for modulo
(object) array ( 'foo' => 1, 'bar' => 2 ) % resource - TypeError Unsupported operand type object for modulo
(object) array ( 'foo' => 1, 'bar' => 2 ) % NULL - TypeError Unsupported operand type object for modulo
(object) array ( 'bar' => 1, 'foo' => 2 ) % false - TypeError Unsupported operand type object for modulo
(object) array ( 'bar' => 1, 'foo' => 2 ) % true - TypeError Unsupported operand type object for modulo
(object) array ( 'bar' => 1, 'foo' => 2 ) % 0 - TypeError Unsupported operand type object for modulo
(object) array ( 'bar' => 1, 'foo' => 2 ) % 10 - TypeError Unsupported operand type object for modulo
(object) array ( 'bar' => 1, 'foo' => 2 ) % 0.0 - TypeError Unsupported operand type object for modulo
(object) array ( 'bar' => 1, 'foo' => 2 ) % 10.0 - TypeError Unsupported operand type object for modulo
(object) array ( 'bar' => 1, 'foo' => 2 ) % 3.14 - TypeError Unsupported operand type object for modulo
(object) array ( 'bar' => 1, 'foo' => 2 ) % '0' - TypeError Unsupported operand type object for modulo
(object) array ( 'bar' => 1, 'foo' => 2 ) % '10' - TypeError Unsupported operand type object for modulo
(object) array ( 'bar' => 1, 'foo' => 2 ) % '010' - TypeError Unsupported operand type object for modulo
(object) array ( 'bar' => 1, 'foo' => 2 ) % '10 elephants' - TypeError Unsupported operand type object for modulo
(object) array ( 'bar' => 1, 'foo' => 2 ) % 'foo' - TypeError Unsupported operand type object for modulo
(object) array ( 'bar' => 1, 'foo' => 2 ) % array ( ) - TypeError Unsupported operand type object for modulo
(object) array ( 'bar' => 1, 'foo' => 2 ) % array ( 0 => 1 ) - TypeError Unsupported operand type object for modulo
(object) array ( 'bar' => 1, 'foo' => 2 ) % array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for modulo
(object) array ( 'bar' => 1, 'foo' => 2 ) % array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for modulo
(object) array ( 'bar' => 1, 'foo' => 2 ) % array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for modulo
(object) array ( 'bar' => 1, 'foo' => 2 ) % (object) array ( ) - TypeError Unsupported operand type object for modulo
(object) array ( 'bar' => 1, 'foo' => 2 ) % (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for modulo
(object) array ( 'bar' => 1, 'foo' => 2 ) % (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for modulo
(object) array ( 'bar' => 1, 'foo' => 2 ) % DateTime - TypeError Unsupported operand type object for modulo
(object) array ( 'bar' => 1, 'foo' => 2 ) % resource - TypeError Unsupported operand type object for modulo
(object) array ( 'bar' => 1, 'foo' => 2 ) % NULL - TypeError Unsupported operand type object for modulo
DateTime % false - TypeError Unsupported operand type object for modulo
DateTime % true - TypeError Unsupported operand type object for modulo
DateTime % 0 - TypeError Unsupported operand type object for modulo
DateTime % 10 - TypeError Unsupported operand type object for modulo
DateTime % 0.0 - TypeError Unsupported operand type object for modulo
DateTime % 10.0 - TypeError Unsupported operand type object for modulo
DateTime % 3.14 - TypeError Unsupported operand type object for modulo
DateTime % '0' - TypeError Unsupported operand type object for modulo
DateTime % '10' - TypeError Unsupported operand type object for modulo
DateTime % '010' - TypeError Unsupported operand type object for modulo
DateTime % '10 elephants' - TypeError Unsupported operand type object for modulo
DateTime % 'foo' - TypeError Unsupported operand type object for modulo
DateTime % array ( ) - TypeError Unsupported operand type object for modulo
DateTime % array ( 0 => 1 ) - TypeError Unsupported operand type object for modulo
DateTime % array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for modulo
DateTime % array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for modulo
DateTime % array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for modulo
DateTime % (object) array ( ) - TypeError Unsupported operand type object for modulo
DateTime % (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for modulo
DateTime % (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for modulo
DateTime % DateTime - TypeError Unsupported operand type object for modulo
DateTime % resource - TypeError Unsupported operand type object for modulo
DateTime % NULL - TypeError Unsupported operand type object for modulo
resource % false - TypeError Unsupported operand type resource for modulo
resource % true - TypeError Unsupported operand type resource for modulo
resource % 0 - TypeError Unsupported operand type resource for modulo
resource % 10 - TypeError Unsupported operand type resource for modulo
resource % 0.0 - TypeError Unsupported operand type resource for modulo
resource % 10.0 - TypeError Unsupported operand type resource for modulo
resource % 3.14 - TypeError Unsupported operand type resource for modulo
resource % '0' - TypeError Unsupported operand type resource for modulo
resource % '10' - TypeError Unsupported operand type resource for modulo
resource % '010' - TypeError Unsupported operand type resource for modulo
resource % '10 elephants' - TypeError Unsupported operand type resource for modulo
resource % 'foo' - TypeError Unsupported operand type resource for modulo
resource % array ( ) - TypeError Unsupported operand type resource for modulo
resource % array ( 0 => 1 ) - TypeError Unsupported operand type resource for modulo
resource % array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type resource for modulo
resource % array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for modulo
resource % array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for modulo
resource % (object) array ( ) - TypeError Unsupported operand type resource for modulo
resource % (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for modulo
resource % (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for modulo
resource % DateTime - TypeError Unsupported operand type resource for modulo
resource % resource - TypeError Unsupported operand type resource for modulo
resource % NULL - TypeError Unsupported operand type resource for modulo
NULL % false - TypeError Unsupported operand type null for modulo
NULL % true - TypeError Unsupported operand type null for modulo
NULL % 0 - TypeError Unsupported operand type null for modulo
NULL % 10 - TypeError Unsupported operand type null for modulo
NULL % 0.0 - TypeError Unsupported operand type null for modulo
NULL % 10.0 - TypeError Unsupported operand type null for modulo
NULL % 3.14 - TypeError Unsupported operand type null for modulo
NULL % '0' - TypeError Unsupported operand type null for modulo
NULL % '10' - TypeError Unsupported operand type null for modulo
NULL % '010' - TypeError Unsupported operand type null for modulo
NULL % '10 elephants' - TypeError Unsupported operand type null for modulo
NULL % 'foo' - TypeError Unsupported operand type null for modulo
NULL % array ( ) - TypeError Unsupported operand type null for modulo
NULL % array ( 0 => 1 ) - TypeError Unsupported operand type null for modulo
NULL % array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type null for modulo
NULL % array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for modulo
NULL % array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for modulo
NULL % (object) array ( ) - TypeError Unsupported operand type null for modulo
NULL % (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for modulo
NULL % (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for modulo
NULL % DateTime - TypeError Unsupported operand type null for modulo
NULL % resource - TypeError Unsupported operand type null for modulo
NULL % NULL - TypeError Unsupported operand type null for modulo