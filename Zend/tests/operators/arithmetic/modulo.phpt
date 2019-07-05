--TEST--
modulo '%' operator
--FILE--
<?php
require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a % $b', function($a, $b) { return $a % $b; });

--EXPECT--
false % false - DivisionByZeroError Modulo by zero
false % true = 0
false % 0 - DivisionByZeroError Modulo by zero
false % 10 = 0
false % 0.0 - DivisionByZeroError Modulo by zero
false % 10.0 = 0
false % 3.14 = 0
false % '0' - DivisionByZeroError Modulo by zero
false % '10' = 0
false % '010' = 0
false % '10 elephants' = 0 - Notice A non well formed numeric value encountered
false % 'foo' - DivisionByZeroError Modulo by zero
false % array ( ) - DivisionByZeroError Modulo by zero
false % array ( 0 => 1 ) = 0
false % array ( 0 => 1, 1 => 100 ) = 0
false % array ( 'foo' => 1, 'bar' => 2 ) = 0
false % array ( 'bar' => 1, 'foo' => 2 ) = 0
false % (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
false % (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
false % (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
false % DateTime = 0 - Notice Object of class DateTime could not be converted to int
false % resource = 0
false % NULL - DivisionByZeroError Modulo by zero
true % false - DivisionByZeroError Modulo by zero
true % true = 0
true % 0 - DivisionByZeroError Modulo by zero
true % 10 = 1
true % 0.0 - DivisionByZeroError Modulo by zero
true % 10.0 = 1
true % 3.14 = 1
true % '0' - DivisionByZeroError Modulo by zero
true % '10' = 1
true % '010' = 1
true % '10 elephants' = 1 - Notice A non well formed numeric value encountered
true % 'foo' - DivisionByZeroError Modulo by zero
true % array ( ) - DivisionByZeroError Modulo by zero
true % array ( 0 => 1 ) = 0
true % array ( 0 => 1, 1 => 100 ) = 0
true % array ( 'foo' => 1, 'bar' => 2 ) = 0
true % array ( 'bar' => 1, 'foo' => 2 ) = 0
true % (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
true % (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
true % (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
true % DateTime = 0 - Notice Object of class DateTime could not be converted to int
true % resource = 1
true % NULL - DivisionByZeroError Modulo by zero
0 % false - DivisionByZeroError Modulo by zero
0 % true = 0
0 % 0 - DivisionByZeroError Modulo by zero
0 % 10 = 0
0 % 0.0 - DivisionByZeroError Modulo by zero
0 % 10.0 = 0
0 % 3.14 = 0
0 % '0' - DivisionByZeroError Modulo by zero
0 % '10' = 0
0 % '010' = 0
0 % '10 elephants' = 0 - Notice A non well formed numeric value encountered
0 % 'foo' - DivisionByZeroError Modulo by zero
0 % array ( ) - DivisionByZeroError Modulo by zero
0 % array ( 0 => 1 ) = 0
0 % array ( 0 => 1, 1 => 100 ) = 0
0 % array ( 'foo' => 1, 'bar' => 2 ) = 0
0 % array ( 'bar' => 1, 'foo' => 2 ) = 0
0 % (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
0 % (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
0 % (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
0 % DateTime = 0 - Notice Object of class DateTime could not be converted to int
0 % resource = 0
0 % NULL - DivisionByZeroError Modulo by zero
10 % false - DivisionByZeroError Modulo by zero
10 % true = 0
10 % 0 - DivisionByZeroError Modulo by zero
10 % 10 = 0
10 % 0.0 - DivisionByZeroError Modulo by zero
10 % 10.0 = 0
10 % 3.14 = 1
10 % '0' - DivisionByZeroError Modulo by zero
10 % '10' = 0
10 % '010' = 0
10 % '10 elephants' = 0 - Notice A non well formed numeric value encountered
10 % 'foo' - DivisionByZeroError Modulo by zero
10 % array ( ) - DivisionByZeroError Modulo by zero
10 % array ( 0 => 1 ) = 0
10 % array ( 0 => 1, 1 => 100 ) = 0
10 % array ( 'foo' => 1, 'bar' => 2 ) = 0
10 % array ( 'bar' => 1, 'foo' => 2 ) = 0
10 % (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
10 % (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
10 % (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
10 % DateTime = 0 - Notice Object of class DateTime could not be converted to int
10 % resource = 4
10 % NULL - DivisionByZeroError Modulo by zero
0.0 % false - DivisionByZeroError Modulo by zero
0.0 % true = 0
0.0 % 0 - DivisionByZeroError Modulo by zero
0.0 % 10 = 0
0.0 % 0.0 - DivisionByZeroError Modulo by zero
0.0 % 10.0 = 0
0.0 % 3.14 = 0
0.0 % '0' - DivisionByZeroError Modulo by zero
0.0 % '10' = 0
0.0 % '010' = 0
0.0 % '10 elephants' = 0 - Notice A non well formed numeric value encountered
0.0 % 'foo' - DivisionByZeroError Modulo by zero
0.0 % array ( ) - DivisionByZeroError Modulo by zero
0.0 % array ( 0 => 1 ) = 0
0.0 % array ( 0 => 1, 1 => 100 ) = 0
0.0 % array ( 'foo' => 1, 'bar' => 2 ) = 0
0.0 % array ( 'bar' => 1, 'foo' => 2 ) = 0
0.0 % (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
0.0 % (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
0.0 % (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
0.0 % DateTime = 0 - Notice Object of class DateTime could not be converted to int
0.0 % resource = 0
0.0 % NULL - DivisionByZeroError Modulo by zero
10.0 % false - DivisionByZeroError Modulo by zero
10.0 % true = 0
10.0 % 0 - DivisionByZeroError Modulo by zero
10.0 % 10 = 0
10.0 % 0.0 - DivisionByZeroError Modulo by zero
10.0 % 10.0 = 0
10.0 % 3.14 = 1
10.0 % '0' - DivisionByZeroError Modulo by zero
10.0 % '10' = 0
10.0 % '010' = 0
10.0 % '10 elephants' = 0 - Notice A non well formed numeric value encountered
10.0 % 'foo' - DivisionByZeroError Modulo by zero
10.0 % array ( ) - DivisionByZeroError Modulo by zero
10.0 % array ( 0 => 1 ) = 0
10.0 % array ( 0 => 1, 1 => 100 ) = 0
10.0 % array ( 'foo' => 1, 'bar' => 2 ) = 0
10.0 % array ( 'bar' => 1, 'foo' => 2 ) = 0
10.0 % (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
10.0 % (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
10.0 % (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
10.0 % DateTime = 0 - Notice Object of class DateTime could not be converted to int
10.0 % resource = 4
10.0 % NULL - DivisionByZeroError Modulo by zero
3.14 % false - DivisionByZeroError Modulo by zero
3.14 % true = 0
3.14 % 0 - DivisionByZeroError Modulo by zero
3.14 % 10 = 3
3.14 % 0.0 - DivisionByZeroError Modulo by zero
3.14 % 10.0 = 3
3.14 % 3.14 = 0
3.14 % '0' - DivisionByZeroError Modulo by zero
3.14 % '10' = 3
3.14 % '010' = 3
3.14 % '10 elephants' = 3 - Notice A non well formed numeric value encountered
3.14 % 'foo' - DivisionByZeroError Modulo by zero
3.14 % array ( ) - DivisionByZeroError Modulo by zero
3.14 % array ( 0 => 1 ) = 0
3.14 % array ( 0 => 1, 1 => 100 ) = 0
3.14 % array ( 'foo' => 1, 'bar' => 2 ) = 0
3.14 % array ( 'bar' => 1, 'foo' => 2 ) = 0
3.14 % (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
3.14 % (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
3.14 % (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
3.14 % DateTime = 0 - Notice Object of class DateTime could not be converted to int
3.14 % resource = 3
3.14 % NULL - DivisionByZeroError Modulo by zero
'0' % false - DivisionByZeroError Modulo by zero
'0' % true = 0
'0' % 0 - DivisionByZeroError Modulo by zero
'0' % 10 = 0
'0' % 0.0 - DivisionByZeroError Modulo by zero
'0' % 10.0 = 0
'0' % 3.14 = 0
'0' % '0' - DivisionByZeroError Modulo by zero
'0' % '10' = 0
'0' % '010' = 0
'0' % '10 elephants' = 0 - Notice A non well formed numeric value encountered
'0' % 'foo' - DivisionByZeroError Modulo by zero
'0' % array ( ) - DivisionByZeroError Modulo by zero
'0' % array ( 0 => 1 ) = 0
'0' % array ( 0 => 1, 1 => 100 ) = 0
'0' % array ( 'foo' => 1, 'bar' => 2 ) = 0
'0' % array ( 'bar' => 1, 'foo' => 2 ) = 0
'0' % (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
'0' % (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
'0' % (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
'0' % DateTime = 0 - Notice Object of class DateTime could not be converted to int
'0' % resource = 0
'0' % NULL - DivisionByZeroError Modulo by zero
'10' % false - DivisionByZeroError Modulo by zero
'10' % true = 0
'10' % 0 - DivisionByZeroError Modulo by zero
'10' % 10 = 0
'10' % 0.0 - DivisionByZeroError Modulo by zero
'10' % 10.0 = 0
'10' % 3.14 = 1
'10' % '0' - DivisionByZeroError Modulo by zero
'10' % '10' = 0
'10' % '010' = 0
'10' % '10 elephants' = 0 - Notice A non well formed numeric value encountered
'10' % 'foo' - DivisionByZeroError Modulo by zero
'10' % array ( ) - DivisionByZeroError Modulo by zero
'10' % array ( 0 => 1 ) = 0
'10' % array ( 0 => 1, 1 => 100 ) = 0
'10' % array ( 'foo' => 1, 'bar' => 2 ) = 0
'10' % array ( 'bar' => 1, 'foo' => 2 ) = 0
'10' % (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
'10' % (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
'10' % (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
'10' % DateTime = 0 - Notice Object of class DateTime could not be converted to int
'10' % resource = 4
'10' % NULL - DivisionByZeroError Modulo by zero
'010' % false - DivisionByZeroError Modulo by zero
'010' % true = 0
'010' % 0 - DivisionByZeroError Modulo by zero
'010' % 10 = 0
'010' % 0.0 - DivisionByZeroError Modulo by zero
'010' % 10.0 = 0
'010' % 3.14 = 1
'010' % '0' - DivisionByZeroError Modulo by zero
'010' % '10' = 0
'010' % '010' = 0
'010' % '10 elephants' = 0 - Notice A non well formed numeric value encountered
'010' % 'foo' - DivisionByZeroError Modulo by zero
'010' % array ( ) - DivisionByZeroError Modulo by zero
'010' % array ( 0 => 1 ) = 0
'010' % array ( 0 => 1, 1 => 100 ) = 0
'010' % array ( 'foo' => 1, 'bar' => 2 ) = 0
'010' % array ( 'bar' => 1, 'foo' => 2 ) = 0
'010' % (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
'010' % (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
'010' % (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
'010' % DateTime = 0 - Notice Object of class DateTime could not be converted to int
'010' % resource = 4
'010' % NULL - DivisionByZeroError Modulo by zero
'10 elephants' % false - DivisionByZeroError Modulo by zero
'10 elephants' % true = 0 - Notice A non well formed numeric value encountered
'10 elephants' % 0 - DivisionByZeroError Modulo by zero
'10 elephants' % 10 = 0 - Notice A non well formed numeric value encountered
'10 elephants' % 0.0 - DivisionByZeroError Modulo by zero
'10 elephants' % 10.0 = 0 - Notice A non well formed numeric value encountered
'10 elephants' % 3.14 = 1 - Notice A non well formed numeric value encountered
'10 elephants' % '0' - DivisionByZeroError Modulo by zero
'10 elephants' % '10' = 0 - Notice A non well formed numeric value encountered
'10 elephants' % '010' = 0 - Notice A non well formed numeric value encountered
'10 elephants' % '10 elephants' = 0 - Notice A non well formed numeric value encountered
'10 elephants' % 'foo' - DivisionByZeroError Modulo by zero
'10 elephants' % array ( ) - DivisionByZeroError Modulo by zero
'10 elephants' % array ( 0 => 1 ) = 0 - Notice A non well formed numeric value encountered
'10 elephants' % array ( 0 => 1, 1 => 100 ) = 0 - Notice A non well formed numeric value encountered
'10 elephants' % array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice A non well formed numeric value encountered
'10 elephants' % array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice A non well formed numeric value encountered
'10 elephants' % (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
'10 elephants' % (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
'10 elephants' % (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
'10 elephants' % DateTime = 0 - Notice Object of class DateTime could not be converted to int
'10 elephants' % resource = 4 - Notice A non well formed numeric value encountered
'10 elephants' % NULL - DivisionByZeroError Modulo by zero
'foo' % false - DivisionByZeroError Modulo by zero
'foo' % true = 0 - Warning A non-numeric value encountered
'foo' % 0 - DivisionByZeroError Modulo by zero
'foo' % 10 = 0 - Warning A non-numeric value encountered
'foo' % 0.0 - DivisionByZeroError Modulo by zero
'foo' % 10.0 = 0 - Warning A non-numeric value encountered
'foo' % 3.14 = 0 - Warning A non-numeric value encountered
'foo' % '0' - DivisionByZeroError Modulo by zero
'foo' % '10' = 0 - Warning A non-numeric value encountered
'foo' % '010' = 0 - Warning A non-numeric value encountered
'foo' % '10 elephants' = 0 - Notice A non well formed numeric value encountered
'foo' % 'foo' - DivisionByZeroError Modulo by zero
'foo' % array ( ) - DivisionByZeroError Modulo by zero
'foo' % array ( 0 => 1 ) = 0 - Warning A non-numeric value encountered
'foo' % array ( 0 => 1, 1 => 100 ) = 0 - Warning A non-numeric value encountered
'foo' % array ( 'foo' => 1, 'bar' => 2 ) = 0 - Warning A non-numeric value encountered
'foo' % array ( 'bar' => 1, 'foo' => 2 ) = 0 - Warning A non-numeric value encountered
'foo' % (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
'foo' % (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
'foo' % (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
'foo' % DateTime = 0 - Notice Object of class DateTime could not be converted to int
'foo' % resource = 0 - Warning A non-numeric value encountered
'foo' % NULL - DivisionByZeroError Modulo by zero
array ( ) % false - DivisionByZeroError Modulo by zero
array ( ) % true = 0
array ( ) % 0 - DivisionByZeroError Modulo by zero
array ( ) % 10 = 0
array ( ) % 0.0 - DivisionByZeroError Modulo by zero
array ( ) % 10.0 = 0
array ( ) % 3.14 = 0
array ( ) % '0' - DivisionByZeroError Modulo by zero
array ( ) % '10' = 0
array ( ) % '010' = 0
array ( ) % '10 elephants' = 0 - Notice A non well formed numeric value encountered
array ( ) % 'foo' - DivisionByZeroError Modulo by zero
array ( ) % array ( ) - DivisionByZeroError Modulo by zero
array ( ) % array ( 0 => 1 ) = 0
array ( ) % array ( 0 => 1, 1 => 100 ) = 0
array ( ) % array ( 'foo' => 1, 'bar' => 2 ) = 0
array ( ) % array ( 'bar' => 1, 'foo' => 2 ) = 0
array ( ) % (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
array ( ) % (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
array ( ) % (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
array ( ) % DateTime = 0 - Notice Object of class DateTime could not be converted to int
array ( ) % resource = 0
array ( ) % NULL - DivisionByZeroError Modulo by zero
array ( 0 => 1 ) % false - DivisionByZeroError Modulo by zero
array ( 0 => 1 ) % true = 0
array ( 0 => 1 ) % 0 - DivisionByZeroError Modulo by zero
array ( 0 => 1 ) % 10 = 1
array ( 0 => 1 ) % 0.0 - DivisionByZeroError Modulo by zero
array ( 0 => 1 ) % 10.0 = 1
array ( 0 => 1 ) % 3.14 = 1
array ( 0 => 1 ) % '0' - DivisionByZeroError Modulo by zero
array ( 0 => 1 ) % '10' = 1
array ( 0 => 1 ) % '010' = 1
array ( 0 => 1 ) % '10 elephants' = 1 - Notice A non well formed numeric value encountered
array ( 0 => 1 ) % 'foo' - DivisionByZeroError Modulo by zero
array ( 0 => 1 ) % array ( ) - DivisionByZeroError Modulo by zero
array ( 0 => 1 ) % array ( 0 => 1 ) = 0
array ( 0 => 1 ) % array ( 0 => 1, 1 => 100 ) = 0
array ( 0 => 1 ) % array ( 'foo' => 1, 'bar' => 2 ) = 0
array ( 0 => 1 ) % array ( 'bar' => 1, 'foo' => 2 ) = 0
array ( 0 => 1 ) % (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
array ( 0 => 1 ) % (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
array ( 0 => 1 ) % (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
array ( 0 => 1 ) % DateTime = 0 - Notice Object of class DateTime could not be converted to int
array ( 0 => 1 ) % resource = 1
array ( 0 => 1 ) % NULL - DivisionByZeroError Modulo by zero
array ( 0 => 1, 1 => 100 ) % false - DivisionByZeroError Modulo by zero
array ( 0 => 1, 1 => 100 ) % true = 0
array ( 0 => 1, 1 => 100 ) % 0 - DivisionByZeroError Modulo by zero
array ( 0 => 1, 1 => 100 ) % 10 = 1
array ( 0 => 1, 1 => 100 ) % 0.0 - DivisionByZeroError Modulo by zero
array ( 0 => 1, 1 => 100 ) % 10.0 = 1
array ( 0 => 1, 1 => 100 ) % 3.14 = 1
array ( 0 => 1, 1 => 100 ) % '0' - DivisionByZeroError Modulo by zero
array ( 0 => 1, 1 => 100 ) % '10' = 1
array ( 0 => 1, 1 => 100 ) % '010' = 1
array ( 0 => 1, 1 => 100 ) % '10 elephants' = 1 - Notice A non well formed numeric value encountered
array ( 0 => 1, 1 => 100 ) % 'foo' - DivisionByZeroError Modulo by zero
array ( 0 => 1, 1 => 100 ) % array ( ) - DivisionByZeroError Modulo by zero
array ( 0 => 1, 1 => 100 ) % array ( 0 => 1 ) = 0
array ( 0 => 1, 1 => 100 ) % array ( 0 => 1, 1 => 100 ) = 0
array ( 0 => 1, 1 => 100 ) % array ( 'foo' => 1, 'bar' => 2 ) = 0
array ( 0 => 1, 1 => 100 ) % array ( 'bar' => 1, 'foo' => 2 ) = 0
array ( 0 => 1, 1 => 100 ) % (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
array ( 0 => 1, 1 => 100 ) % (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
array ( 0 => 1, 1 => 100 ) % (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
array ( 0 => 1, 1 => 100 ) % DateTime = 0 - Notice Object of class DateTime could not be converted to int
array ( 0 => 1, 1 => 100 ) % resource = 1
array ( 0 => 1, 1 => 100 ) % NULL - DivisionByZeroError Modulo by zero
array ( 'foo' => 1, 'bar' => 2 ) % false - DivisionByZeroError Modulo by zero
array ( 'foo' => 1, 'bar' => 2 ) % true = 0
array ( 'foo' => 1, 'bar' => 2 ) % 0 - DivisionByZeroError Modulo by zero
array ( 'foo' => 1, 'bar' => 2 ) % 10 = 1
array ( 'foo' => 1, 'bar' => 2 ) % 0.0 - DivisionByZeroError Modulo by zero
array ( 'foo' => 1, 'bar' => 2 ) % 10.0 = 1
array ( 'foo' => 1, 'bar' => 2 ) % 3.14 = 1
array ( 'foo' => 1, 'bar' => 2 ) % '0' - DivisionByZeroError Modulo by zero
array ( 'foo' => 1, 'bar' => 2 ) % '10' = 1
array ( 'foo' => 1, 'bar' => 2 ) % '010' = 1
array ( 'foo' => 1, 'bar' => 2 ) % '10 elephants' = 1 - Notice A non well formed numeric value encountered
array ( 'foo' => 1, 'bar' => 2 ) % 'foo' - DivisionByZeroError Modulo by zero
array ( 'foo' => 1, 'bar' => 2 ) % array ( ) - DivisionByZeroError Modulo by zero
array ( 'foo' => 1, 'bar' => 2 ) % array ( 0 => 1 ) = 0
array ( 'foo' => 1, 'bar' => 2 ) % array ( 0 => 1, 1 => 100 ) = 0
array ( 'foo' => 1, 'bar' => 2 ) % array ( 'foo' => 1, 'bar' => 2 ) = 0
array ( 'foo' => 1, 'bar' => 2 ) % array ( 'bar' => 1, 'foo' => 2 ) = 0
array ( 'foo' => 1, 'bar' => 2 ) % (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
array ( 'foo' => 1, 'bar' => 2 ) % (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
array ( 'foo' => 1, 'bar' => 2 ) % (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
array ( 'foo' => 1, 'bar' => 2 ) % DateTime = 0 - Notice Object of class DateTime could not be converted to int
array ( 'foo' => 1, 'bar' => 2 ) % resource = 1
array ( 'foo' => 1, 'bar' => 2 ) % NULL - DivisionByZeroError Modulo by zero
array ( 'bar' => 1, 'foo' => 2 ) % false - DivisionByZeroError Modulo by zero
array ( 'bar' => 1, 'foo' => 2 ) % true = 0
array ( 'bar' => 1, 'foo' => 2 ) % 0 - DivisionByZeroError Modulo by zero
array ( 'bar' => 1, 'foo' => 2 ) % 10 = 1
array ( 'bar' => 1, 'foo' => 2 ) % 0.0 - DivisionByZeroError Modulo by zero
array ( 'bar' => 1, 'foo' => 2 ) % 10.0 = 1
array ( 'bar' => 1, 'foo' => 2 ) % 3.14 = 1
array ( 'bar' => 1, 'foo' => 2 ) % '0' - DivisionByZeroError Modulo by zero
array ( 'bar' => 1, 'foo' => 2 ) % '10' = 1
array ( 'bar' => 1, 'foo' => 2 ) % '010' = 1
array ( 'bar' => 1, 'foo' => 2 ) % '10 elephants' = 1 - Notice A non well formed numeric value encountered
array ( 'bar' => 1, 'foo' => 2 ) % 'foo' - DivisionByZeroError Modulo by zero
array ( 'bar' => 1, 'foo' => 2 ) % array ( ) - DivisionByZeroError Modulo by zero
array ( 'bar' => 1, 'foo' => 2 ) % array ( 0 => 1 ) = 0
array ( 'bar' => 1, 'foo' => 2 ) % array ( 0 => 1, 1 => 100 ) = 0
array ( 'bar' => 1, 'foo' => 2 ) % array ( 'foo' => 1, 'bar' => 2 ) = 0
array ( 'bar' => 1, 'foo' => 2 ) % array ( 'bar' => 1, 'foo' => 2 ) = 0
array ( 'bar' => 1, 'foo' => 2 ) % (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
array ( 'bar' => 1, 'foo' => 2 ) % (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
array ( 'bar' => 1, 'foo' => 2 ) % (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
array ( 'bar' => 1, 'foo' => 2 ) % DateTime = 0 - Notice Object of class DateTime could not be converted to int
array ( 'bar' => 1, 'foo' => 2 ) % resource = 1
array ( 'bar' => 1, 'foo' => 2 ) % NULL - DivisionByZeroError Modulo by zero
(object) array ( ) % false - DivisionByZeroError Modulo by zero
(object) array ( ) % true = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) % 0 - DivisionByZeroError Modulo by zero
(object) array ( ) % 10 = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( ) % 0.0 - DivisionByZeroError Modulo by zero
(object) array ( ) % 10.0 = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( ) % 3.14 = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( ) % '0' - DivisionByZeroError Modulo by zero
(object) array ( ) % '10' = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( ) % '010' = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( ) % '10 elephants' = 1 - Notice A non well formed numeric value encountered
(object) array ( ) % 'foo' - DivisionByZeroError Modulo by zero
(object) array ( ) % array ( ) - DivisionByZeroError Modulo by zero
(object) array ( ) % array ( 0 => 1 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) % array ( 0 => 1, 1 => 100 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) % array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) % array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) % (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) % (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) % (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) % DateTime = 0 - Notice Object of class DateTime could not be converted to int
(object) array ( ) % resource = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( ) % NULL - DivisionByZeroError Modulo by zero
(object) array ( 'foo' => 1, 'bar' => 2 ) % false - DivisionByZeroError Modulo by zero
(object) array ( 'foo' => 1, 'bar' => 2 ) % true = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) % 0 - DivisionByZeroError Modulo by zero
(object) array ( 'foo' => 1, 'bar' => 2 ) % 10 = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) % 0.0 - DivisionByZeroError Modulo by zero
(object) array ( 'foo' => 1, 'bar' => 2 ) % 10.0 = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) % 3.14 = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) % '0' - DivisionByZeroError Modulo by zero
(object) array ( 'foo' => 1, 'bar' => 2 ) % '10' = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) % '010' = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) % '10 elephants' = 1 - Notice A non well formed numeric value encountered
(object) array ( 'foo' => 1, 'bar' => 2 ) % 'foo' - DivisionByZeroError Modulo by zero
(object) array ( 'foo' => 1, 'bar' => 2 ) % array ( ) - DivisionByZeroError Modulo by zero
(object) array ( 'foo' => 1, 'bar' => 2 ) % array ( 0 => 1 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) % array ( 0 => 1, 1 => 100 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) % array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) % array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) % (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) % (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) % (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) % DateTime = 0 - Notice Object of class DateTime could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) % resource = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) % NULL - DivisionByZeroError Modulo by zero
(object) array ( 'bar' => 1, 'foo' => 2 ) % false - DivisionByZeroError Modulo by zero
(object) array ( 'bar' => 1, 'foo' => 2 ) % true = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) % 0 - DivisionByZeroError Modulo by zero
(object) array ( 'bar' => 1, 'foo' => 2 ) % 10 = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) % 0.0 - DivisionByZeroError Modulo by zero
(object) array ( 'bar' => 1, 'foo' => 2 ) % 10.0 = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) % 3.14 = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) % '0' - DivisionByZeroError Modulo by zero
(object) array ( 'bar' => 1, 'foo' => 2 ) % '10' = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) % '010' = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) % '10 elephants' = 1 - Notice A non well formed numeric value encountered
(object) array ( 'bar' => 1, 'foo' => 2 ) % 'foo' - DivisionByZeroError Modulo by zero
(object) array ( 'bar' => 1, 'foo' => 2 ) % array ( ) - DivisionByZeroError Modulo by zero
(object) array ( 'bar' => 1, 'foo' => 2 ) % array ( 0 => 1 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) % array ( 0 => 1, 1 => 100 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) % array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) % array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) % (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) % (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) % (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) % DateTime = 0 - Notice Object of class DateTime could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) % resource = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) % NULL - DivisionByZeroError Modulo by zero
DateTime % false - DivisionByZeroError Modulo by zero
DateTime % true = 0 - Notice Object of class DateTime could not be converted to int
DateTime % 0 - DivisionByZeroError Modulo by zero
DateTime % 10 = 1 - Notice Object of class DateTime could not be converted to int
DateTime % 0.0 - DivisionByZeroError Modulo by zero
DateTime % 10.0 = 1 - Notice Object of class DateTime could not be converted to int
DateTime % 3.14 = 1 - Notice Object of class DateTime could not be converted to int
DateTime % '0' - DivisionByZeroError Modulo by zero
DateTime % '10' = 1 - Notice Object of class DateTime could not be converted to int
DateTime % '010' = 1 - Notice Object of class DateTime could not be converted to int
DateTime % '10 elephants' = 1 - Notice A non well formed numeric value encountered
DateTime % 'foo' - DivisionByZeroError Modulo by zero
DateTime % array ( ) - DivisionByZeroError Modulo by zero
DateTime % array ( 0 => 1 ) = 0 - Notice Object of class DateTime could not be converted to int
DateTime % array ( 0 => 1, 1 => 100 ) = 0 - Notice Object of class DateTime could not be converted to int
DateTime % array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class DateTime could not be converted to int
DateTime % array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class DateTime could not be converted to int
DateTime % (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
DateTime % (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
DateTime % (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
DateTime % DateTime = 0 - Notice Object of class DateTime could not be converted to int
DateTime % resource = 1 - Notice Object of class DateTime could not be converted to int
DateTime % NULL - DivisionByZeroError Modulo by zero
resource % false - DivisionByZeroError Modulo by zero
resource % true = 0
resource % 0 - DivisionByZeroError Modulo by zero
resource % 10 = 6
resource % 0.0 - DivisionByZeroError Modulo by zero
resource % 10.0 = 6
resource % 3.14 = 0
resource % '0' - DivisionByZeroError Modulo by zero
resource % '10' = 6
resource % '010' = 6
resource % '10 elephants' = 6 - Notice A non well formed numeric value encountered
resource % 'foo' - DivisionByZeroError Modulo by zero
resource % array ( ) - DivisionByZeroError Modulo by zero
resource % array ( 0 => 1 ) = 0
resource % array ( 0 => 1, 1 => 100 ) = 0
resource % array ( 'foo' => 1, 'bar' => 2 ) = 0
resource % array ( 'bar' => 1, 'foo' => 2 ) = 0
resource % (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
resource % (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
resource % (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
resource % DateTime = 0 - Notice Object of class DateTime could not be converted to int
resource % resource = 0
resource % NULL - DivisionByZeroError Modulo by zero
NULL % false - DivisionByZeroError Modulo by zero
NULL % true = 0
NULL % 0 - DivisionByZeroError Modulo by zero
NULL % 10 = 0
NULL % 0.0 - DivisionByZeroError Modulo by zero
NULL % 10.0 = 0
NULL % 3.14 = 0
NULL % '0' - DivisionByZeroError Modulo by zero
NULL % '10' = 0
NULL % '010' = 0
NULL % '10 elephants' = 0 - Notice A non well formed numeric value encountered
NULL % 'foo' - DivisionByZeroError Modulo by zero
NULL % array ( ) - DivisionByZeroError Modulo by zero
NULL % array ( 0 => 1 ) = 0
NULL % array ( 0 => 1, 1 => 100 ) = 0
NULL % array ( 'foo' => 1, 'bar' => 2 ) = 0
NULL % array ( 'bar' => 1, 'foo' => 2 ) = 0
NULL % (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
NULL % (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
NULL % (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
NULL % DateTime = 0 - Notice Object of class DateTime could not be converted to int
NULL % resource = 0
NULL % NULL - DivisionByZeroError Modulo by zero