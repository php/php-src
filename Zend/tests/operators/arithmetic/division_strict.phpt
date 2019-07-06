--TEST--
division '/' operator with strict_operators
--INI--
precision=14
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a / $b', function($a, $b) { return $a / $b; });

--EXPECT--
false / false - TypeError Unsupported operand types
false / true - TypeError Unsupported operand types
false / 0 - TypeError Unsupported operand types
false / 10 - TypeError Unsupported operand types
false / 0.0 - TypeError Unsupported operand types
false / 10.0 - TypeError Unsupported operand types
false / 3.14 - TypeError Unsupported operand types
false / '0' - TypeError Unsupported operand types
false / '10' - TypeError Unsupported operand types
false / '010' - TypeError Unsupported operand types
false / '10 elephants' - TypeError Unsupported operand types
false / 'foo' - TypeError Unsupported operand types
false / array ( ) - TypeError Unsupported operand types
false / array ( 0 => 1 ) - TypeError Unsupported operand types
false / array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
false / array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
false / array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
false / (object) array ( ) - TypeError Unsupported operand types
false / (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
false / (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
false / DateTime - TypeError Unsupported operand types
false / resource - TypeError Unsupported operand types
false / NULL - TypeError Unsupported operand types
true / false - TypeError Unsupported operand types
true / true - TypeError Unsupported operand types
true / 0 - TypeError Unsupported operand types
true / 10 - TypeError Unsupported operand types
true / 0.0 - TypeError Unsupported operand types
true / 10.0 - TypeError Unsupported operand types
true / 3.14 - TypeError Unsupported operand types
true / '0' - TypeError Unsupported operand types
true / '10' - TypeError Unsupported operand types
true / '010' - TypeError Unsupported operand types
true / '10 elephants' - TypeError Unsupported operand types
true / 'foo' - TypeError Unsupported operand types
true / array ( ) - TypeError Unsupported operand types
true / array ( 0 => 1 ) - TypeError Unsupported operand types
true / array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
true / array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
true / array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
true / (object) array ( ) - TypeError Unsupported operand types
true / (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
true / (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
true / DateTime - TypeError Unsupported operand types
true / resource - TypeError Unsupported operand types
true / NULL - TypeError Unsupported operand types
0 / false - TypeError Unsupported operand types
0 / true - TypeError Unsupported operand types
0 / 0 = NAN - Warning Division by zero
0 / 10 = 0
0 / 0.0 = NAN - Warning Division by zero
0 / 10.0 = 0.0
0 / 3.14 = 0.0
0 / '0' - TypeError Unsupported operand types
0 / '10' - TypeError Unsupported operand types
0 / '010' - TypeError Unsupported operand types
0 / '10 elephants' - TypeError Unsupported operand types
0 / 'foo' - TypeError Unsupported operand types
0 / array ( ) - TypeError Unsupported operand types
0 / array ( 0 => 1 ) - TypeError Unsupported operand types
0 / array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
0 / array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
0 / array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
0 / (object) array ( ) - TypeError Unsupported operand types
0 / (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
0 / (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
0 / DateTime - TypeError Unsupported operand types
0 / resource - TypeError Unsupported operand types
0 / NULL - TypeError Unsupported operand types
10 / false - TypeError Unsupported operand types
10 / true - TypeError Unsupported operand types
10 / 0 = INF - Warning Division by zero
10 / 10 = 1
10 / 0.0 = INF - Warning Division by zero
10 / 10.0 = 1.0
10 / 3.14 = 3.184713375796178
10 / '0' - TypeError Unsupported operand types
10 / '10' - TypeError Unsupported operand types
10 / '010' - TypeError Unsupported operand types
10 / '10 elephants' - TypeError Unsupported operand types
10 / 'foo' - TypeError Unsupported operand types
10 / array ( ) - TypeError Unsupported operand types
10 / array ( 0 => 1 ) - TypeError Unsupported operand types
10 / array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
10 / array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
10 / array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
10 / (object) array ( ) - TypeError Unsupported operand types
10 / (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
10 / (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
10 / DateTime - TypeError Unsupported operand types
10 / resource - TypeError Unsupported operand types
10 / NULL - TypeError Unsupported operand types
0.0 / false - TypeError Unsupported operand types
0.0 / true - TypeError Unsupported operand types
0.0 / 0 = NAN - Warning Division by zero
0.0 / 10 = 0.0
0.0 / 0.0 = NAN - Warning Division by zero
0.0 / 10.0 = 0.0
0.0 / 3.14 = 0.0
0.0 / '0' - TypeError Unsupported operand types
0.0 / '10' - TypeError Unsupported operand types
0.0 / '010' - TypeError Unsupported operand types
0.0 / '10 elephants' - TypeError Unsupported operand types
0.0 / 'foo' - TypeError Unsupported operand types
0.0 / array ( ) - TypeError Unsupported operand types
0.0 / array ( 0 => 1 ) - TypeError Unsupported operand types
0.0 / array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
0.0 / array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
0.0 / array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
0.0 / (object) array ( ) - TypeError Unsupported operand types
0.0 / (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
0.0 / (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
0.0 / DateTime - TypeError Unsupported operand types
0.0 / resource - TypeError Unsupported operand types
0.0 / NULL - TypeError Unsupported operand types
10.0 / false - TypeError Unsupported operand types
10.0 / true - TypeError Unsupported operand types
10.0 / 0 = INF - Warning Division by zero
10.0 / 10 = 1.0
10.0 / 0.0 = INF - Warning Division by zero
10.0 / 10.0 = 1.0
10.0 / 3.14 = 3.184713375796178
10.0 / '0' - TypeError Unsupported operand types
10.0 / '10' - TypeError Unsupported operand types
10.0 / '010' - TypeError Unsupported operand types
10.0 / '10 elephants' - TypeError Unsupported operand types
10.0 / 'foo' - TypeError Unsupported operand types
10.0 / array ( ) - TypeError Unsupported operand types
10.0 / array ( 0 => 1 ) - TypeError Unsupported operand types
10.0 / array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
10.0 / array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
10.0 / array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
10.0 / (object) array ( ) - TypeError Unsupported operand types
10.0 / (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
10.0 / (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
10.0 / DateTime - TypeError Unsupported operand types
10.0 / resource - TypeError Unsupported operand types
10.0 / NULL - TypeError Unsupported operand types
3.14 / false - TypeError Unsupported operand types
3.14 / true - TypeError Unsupported operand types
3.14 / 0 = INF - Warning Division by zero
3.14 / 10 = 0.314
3.14 / 0.0 = INF - Warning Division by zero
3.14 / 10.0 = 0.314
3.14 / 3.14 = 1.0
3.14 / '0' - TypeError Unsupported operand types
3.14 / '10' - TypeError Unsupported operand types
3.14 / '010' - TypeError Unsupported operand types
3.14 / '10 elephants' - TypeError Unsupported operand types
3.14 / 'foo' - TypeError Unsupported operand types
3.14 / array ( ) - TypeError Unsupported operand types
3.14 / array ( 0 => 1 ) - TypeError Unsupported operand types
3.14 / array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
3.14 / array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
3.14 / array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
3.14 / (object) array ( ) - TypeError Unsupported operand types
3.14 / (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
3.14 / (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
3.14 / DateTime - TypeError Unsupported operand types
3.14 / resource - TypeError Unsupported operand types
3.14 / NULL - TypeError Unsupported operand types
'0' / false - TypeError Unsupported operand types
'0' / true - TypeError Unsupported operand types
'0' / 0 - TypeError Unsupported operand types
'0' / 10 - TypeError Unsupported operand types
'0' / 0.0 - TypeError Unsupported operand types
'0' / 10.0 - TypeError Unsupported operand types
'0' / 3.14 - TypeError Unsupported operand types
'0' / '0' - TypeError Unsupported operand types
'0' / '10' - TypeError Unsupported operand types
'0' / '010' - TypeError Unsupported operand types
'0' / '10 elephants' - TypeError Unsupported operand types
'0' / 'foo' - TypeError Unsupported operand types
'0' / array ( ) - TypeError Unsupported operand types
'0' / array ( 0 => 1 ) - TypeError Unsupported operand types
'0' / array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
'0' / array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
'0' / array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
'0' / (object) array ( ) - TypeError Unsupported operand types
'0' / (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
'0' / (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
'0' / DateTime - TypeError Unsupported operand types
'0' / resource - TypeError Unsupported operand types
'0' / NULL - TypeError Unsupported operand types
'10' / false - TypeError Unsupported operand types
'10' / true - TypeError Unsupported operand types
'10' / 0 - TypeError Unsupported operand types
'10' / 10 - TypeError Unsupported operand types
'10' / 0.0 - TypeError Unsupported operand types
'10' / 10.0 - TypeError Unsupported operand types
'10' / 3.14 - TypeError Unsupported operand types
'10' / '0' - TypeError Unsupported operand types
'10' / '10' - TypeError Unsupported operand types
'10' / '010' - TypeError Unsupported operand types
'10' / '10 elephants' - TypeError Unsupported operand types
'10' / 'foo' - TypeError Unsupported operand types
'10' / array ( ) - TypeError Unsupported operand types
'10' / array ( 0 => 1 ) - TypeError Unsupported operand types
'10' / array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
'10' / array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
'10' / array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
'10' / (object) array ( ) - TypeError Unsupported operand types
'10' / (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
'10' / (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
'10' / DateTime - TypeError Unsupported operand types
'10' / resource - TypeError Unsupported operand types
'10' / NULL - TypeError Unsupported operand types
'010' / false - TypeError Unsupported operand types
'010' / true - TypeError Unsupported operand types
'010' / 0 - TypeError Unsupported operand types
'010' / 10 - TypeError Unsupported operand types
'010' / 0.0 - TypeError Unsupported operand types
'010' / 10.0 - TypeError Unsupported operand types
'010' / 3.14 - TypeError Unsupported operand types
'010' / '0' - TypeError Unsupported operand types
'010' / '10' - TypeError Unsupported operand types
'010' / '010' - TypeError Unsupported operand types
'010' / '10 elephants' - TypeError Unsupported operand types
'010' / 'foo' - TypeError Unsupported operand types
'010' / array ( ) - TypeError Unsupported operand types
'010' / array ( 0 => 1 ) - TypeError Unsupported operand types
'010' / array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
'010' / array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
'010' / array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
'010' / (object) array ( ) - TypeError Unsupported operand types
'010' / (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
'010' / (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
'010' / DateTime - TypeError Unsupported operand types
'010' / resource - TypeError Unsupported operand types
'010' / NULL - TypeError Unsupported operand types
'10 elephants' / false - TypeError Unsupported operand types
'10 elephants' / true - TypeError Unsupported operand types
'10 elephants' / 0 - TypeError Unsupported operand types
'10 elephants' / 10 - TypeError Unsupported operand types
'10 elephants' / 0.0 - TypeError Unsupported operand types
'10 elephants' / 10.0 - TypeError Unsupported operand types
'10 elephants' / 3.14 - TypeError Unsupported operand types
'10 elephants' / '0' - TypeError Unsupported operand types
'10 elephants' / '10' - TypeError Unsupported operand types
'10 elephants' / '010' - TypeError Unsupported operand types
'10 elephants' / '10 elephants' - TypeError Unsupported operand types
'10 elephants' / 'foo' - TypeError Unsupported operand types
'10 elephants' / array ( ) - TypeError Unsupported operand types
'10 elephants' / array ( 0 => 1 ) - TypeError Unsupported operand types
'10 elephants' / array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
'10 elephants' / array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
'10 elephants' / array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
'10 elephants' / (object) array ( ) - TypeError Unsupported operand types
'10 elephants' / (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
'10 elephants' / (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
'10 elephants' / DateTime - TypeError Unsupported operand types
'10 elephants' / resource - TypeError Unsupported operand types
'10 elephants' / NULL - TypeError Unsupported operand types
'foo' / false - TypeError Unsupported operand types
'foo' / true - TypeError Unsupported operand types
'foo' / 0 - TypeError Unsupported operand types
'foo' / 10 - TypeError Unsupported operand types
'foo' / 0.0 - TypeError Unsupported operand types
'foo' / 10.0 - TypeError Unsupported operand types
'foo' / 3.14 - TypeError Unsupported operand types
'foo' / '0' - TypeError Unsupported operand types
'foo' / '10' - TypeError Unsupported operand types
'foo' / '010' - TypeError Unsupported operand types
'foo' / '10 elephants' - TypeError Unsupported operand types
'foo' / 'foo' - TypeError Unsupported operand types
'foo' / array ( ) - TypeError Unsupported operand types
'foo' / array ( 0 => 1 ) - TypeError Unsupported operand types
'foo' / array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
'foo' / array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
'foo' / array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
'foo' / (object) array ( ) - TypeError Unsupported operand types
'foo' / (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
'foo' / (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
'foo' / DateTime - TypeError Unsupported operand types
'foo' / resource - TypeError Unsupported operand types
'foo' / NULL - TypeError Unsupported operand types
array ( ) / false - TypeError Unsupported operand types
array ( ) / true - TypeError Unsupported operand types
array ( ) / 0 - TypeError Unsupported operand types
array ( ) / 10 - TypeError Unsupported operand types
array ( ) / 0.0 - TypeError Unsupported operand types
array ( ) / 10.0 - TypeError Unsupported operand types
array ( ) / 3.14 - TypeError Unsupported operand types
array ( ) / '0' - TypeError Unsupported operand types
array ( ) / '10' - TypeError Unsupported operand types
array ( ) / '010' - TypeError Unsupported operand types
array ( ) / '10 elephants' - TypeError Unsupported operand types
array ( ) / 'foo' - TypeError Unsupported operand types
array ( ) / array ( ) - TypeError Unsupported operand types
array ( ) / array ( 0 => 1 ) - TypeError Unsupported operand types
array ( ) / array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
array ( ) / array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
array ( ) / array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
array ( ) / (object) array ( ) - TypeError Unsupported operand types
array ( ) / (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
array ( ) / (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
array ( ) / DateTime - TypeError Unsupported operand types
array ( ) / resource - TypeError Unsupported operand types
array ( ) / NULL - TypeError Unsupported operand types
array ( 0 => 1 ) / false - TypeError Unsupported operand types
array ( 0 => 1 ) / true - TypeError Unsupported operand types
array ( 0 => 1 ) / 0 - TypeError Unsupported operand types
array ( 0 => 1 ) / 10 - TypeError Unsupported operand types
array ( 0 => 1 ) / 0.0 - TypeError Unsupported operand types
array ( 0 => 1 ) / 10.0 - TypeError Unsupported operand types
array ( 0 => 1 ) / 3.14 - TypeError Unsupported operand types
array ( 0 => 1 ) / '0' - TypeError Unsupported operand types
array ( 0 => 1 ) / '10' - TypeError Unsupported operand types
array ( 0 => 1 ) / '010' - TypeError Unsupported operand types
array ( 0 => 1 ) / '10 elephants' - TypeError Unsupported operand types
array ( 0 => 1 ) / 'foo' - TypeError Unsupported operand types
array ( 0 => 1 ) / array ( ) - TypeError Unsupported operand types
array ( 0 => 1 ) / array ( 0 => 1 ) - TypeError Unsupported operand types
array ( 0 => 1 ) / array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
array ( 0 => 1 ) / array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
array ( 0 => 1 ) / array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
array ( 0 => 1 ) / (object) array ( ) - TypeError Unsupported operand types
array ( 0 => 1 ) / (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
array ( 0 => 1 ) / (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
array ( 0 => 1 ) / DateTime - TypeError Unsupported operand types
array ( 0 => 1 ) / resource - TypeError Unsupported operand types
array ( 0 => 1 ) / NULL - TypeError Unsupported operand types
array ( 0 => 1, 1 => 100 ) / false - TypeError Unsupported operand types
array ( 0 => 1, 1 => 100 ) / true - TypeError Unsupported operand types
array ( 0 => 1, 1 => 100 ) / 0 - TypeError Unsupported operand types
array ( 0 => 1, 1 => 100 ) / 10 - TypeError Unsupported operand types
array ( 0 => 1, 1 => 100 ) / 0.0 - TypeError Unsupported operand types
array ( 0 => 1, 1 => 100 ) / 10.0 - TypeError Unsupported operand types
array ( 0 => 1, 1 => 100 ) / 3.14 - TypeError Unsupported operand types
array ( 0 => 1, 1 => 100 ) / '0' - TypeError Unsupported operand types
array ( 0 => 1, 1 => 100 ) / '10' - TypeError Unsupported operand types
array ( 0 => 1, 1 => 100 ) / '010' - TypeError Unsupported operand types
array ( 0 => 1, 1 => 100 ) / '10 elephants' - TypeError Unsupported operand types
array ( 0 => 1, 1 => 100 ) / 'foo' - TypeError Unsupported operand types
array ( 0 => 1, 1 => 100 ) / array ( ) - TypeError Unsupported operand types
array ( 0 => 1, 1 => 100 ) / array ( 0 => 1 ) - TypeError Unsupported operand types
array ( 0 => 1, 1 => 100 ) / array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
array ( 0 => 1, 1 => 100 ) / array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
array ( 0 => 1, 1 => 100 ) / array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
array ( 0 => 1, 1 => 100 ) / (object) array ( ) - TypeError Unsupported operand types
array ( 0 => 1, 1 => 100 ) / (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
array ( 0 => 1, 1 => 100 ) / (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
array ( 0 => 1, 1 => 100 ) / DateTime - TypeError Unsupported operand types
array ( 0 => 1, 1 => 100 ) / resource - TypeError Unsupported operand types
array ( 0 => 1, 1 => 100 ) / NULL - TypeError Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) / false - TypeError Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) / true - TypeError Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) / 0 - TypeError Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) / 10 - TypeError Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) / 0.0 - TypeError Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) / 10.0 - TypeError Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) / 3.14 - TypeError Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) / '0' - TypeError Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) / '10' - TypeError Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) / '010' - TypeError Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) / '10 elephants' - TypeError Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) / 'foo' - TypeError Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) / array ( ) - TypeError Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) / array ( 0 => 1 ) - TypeError Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) / array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) / array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) / array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) / (object) array ( ) - TypeError Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) / (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) / (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) / DateTime - TypeError Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) / resource - TypeError Unsupported operand types
array ( 'foo' => 1, 'bar' => 2 ) / NULL - TypeError Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) / false - TypeError Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) / true - TypeError Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) / 0 - TypeError Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) / 10 - TypeError Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) / 0.0 - TypeError Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) / 10.0 - TypeError Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) / 3.14 - TypeError Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) / '0' - TypeError Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) / '10' - TypeError Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) / '010' - TypeError Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) / '10 elephants' - TypeError Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) / 'foo' - TypeError Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) / array ( ) - TypeError Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) / array ( 0 => 1 ) - TypeError Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) / array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) / array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) / array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) / (object) array ( ) - TypeError Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) / (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) / (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) / DateTime - TypeError Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) / resource - TypeError Unsupported operand types
array ( 'bar' => 1, 'foo' => 2 ) / NULL - TypeError Unsupported operand types
(object) array ( ) / false - TypeError Unsupported operand types
(object) array ( ) / true - TypeError Unsupported operand types
(object) array ( ) / 0 - TypeError Unsupported operand types
(object) array ( ) / 10 - TypeError Unsupported operand types
(object) array ( ) / 0.0 - TypeError Unsupported operand types
(object) array ( ) / 10.0 - TypeError Unsupported operand types
(object) array ( ) / 3.14 - TypeError Unsupported operand types
(object) array ( ) / '0' - TypeError Unsupported operand types
(object) array ( ) / '10' - TypeError Unsupported operand types
(object) array ( ) / '010' - TypeError Unsupported operand types
(object) array ( ) / '10 elephants' - TypeError Unsupported operand types
(object) array ( ) / 'foo' - TypeError Unsupported operand types
(object) array ( ) / array ( ) - TypeError Unsupported operand types
(object) array ( ) / array ( 0 => 1 ) - TypeError Unsupported operand types
(object) array ( ) / array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
(object) array ( ) / array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
(object) array ( ) / array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
(object) array ( ) / (object) array ( ) - TypeError Unsupported operand types
(object) array ( ) / (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
(object) array ( ) / (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
(object) array ( ) / DateTime - TypeError Unsupported operand types
(object) array ( ) / resource - TypeError Unsupported operand types
(object) array ( ) / NULL - TypeError Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) / false - TypeError Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) / true - TypeError Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) / 0 - TypeError Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) / 10 - TypeError Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) / 0.0 - TypeError Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) / 10.0 - TypeError Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) / 3.14 - TypeError Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) / '0' - TypeError Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) / '10' - TypeError Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) / '010' - TypeError Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) / '10 elephants' - TypeError Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) / 'foo' - TypeError Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) / array ( ) - TypeError Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) / array ( 0 => 1 ) - TypeError Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) / array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) / array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) / array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) / (object) array ( ) - TypeError Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) / (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) / (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) / DateTime - TypeError Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) / resource - TypeError Unsupported operand types
(object) array ( 'foo' => 1, 'bar' => 2 ) / NULL - TypeError Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) / false - TypeError Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) / true - TypeError Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) / 0 - TypeError Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) / 10 - TypeError Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) / 0.0 - TypeError Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) / 10.0 - TypeError Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) / 3.14 - TypeError Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) / '0' - TypeError Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) / '10' - TypeError Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) / '010' - TypeError Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) / '10 elephants' - TypeError Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) / 'foo' - TypeError Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) / array ( ) - TypeError Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) / array ( 0 => 1 ) - TypeError Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) / array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) / array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) / array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) / (object) array ( ) - TypeError Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) / (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) / (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) / DateTime - TypeError Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) / resource - TypeError Unsupported operand types
(object) array ( 'bar' => 1, 'foo' => 2 ) / NULL - TypeError Unsupported operand types
DateTime / false - TypeError Unsupported operand types
DateTime / true - TypeError Unsupported operand types
DateTime / 0 - TypeError Unsupported operand types
DateTime / 10 - TypeError Unsupported operand types
DateTime / 0.0 - TypeError Unsupported operand types
DateTime / 10.0 - TypeError Unsupported operand types
DateTime / 3.14 - TypeError Unsupported operand types
DateTime / '0' - TypeError Unsupported operand types
DateTime / '10' - TypeError Unsupported operand types
DateTime / '010' - TypeError Unsupported operand types
DateTime / '10 elephants' - TypeError Unsupported operand types
DateTime / 'foo' - TypeError Unsupported operand types
DateTime / array ( ) - TypeError Unsupported operand types
DateTime / array ( 0 => 1 ) - TypeError Unsupported operand types
DateTime / array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
DateTime / array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
DateTime / array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
DateTime / (object) array ( ) - TypeError Unsupported operand types
DateTime / (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
DateTime / (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
DateTime / DateTime - TypeError Unsupported operand types
DateTime / resource - TypeError Unsupported operand types
DateTime / NULL - TypeError Unsupported operand types
resource / false - TypeError Unsupported operand types
resource / true - TypeError Unsupported operand types
resource / 0 - TypeError Unsupported operand types
resource / 10 - TypeError Unsupported operand types
resource / 0.0 - TypeError Unsupported operand types
resource / 10.0 - TypeError Unsupported operand types
resource / 3.14 - TypeError Unsupported operand types
resource / '0' - TypeError Unsupported operand types
resource / '10' - TypeError Unsupported operand types
resource / '010' - TypeError Unsupported operand types
resource / '10 elephants' - TypeError Unsupported operand types
resource / 'foo' - TypeError Unsupported operand types
resource / array ( ) - TypeError Unsupported operand types
resource / array ( 0 => 1 ) - TypeError Unsupported operand types
resource / array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
resource / array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
resource / array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
resource / (object) array ( ) - TypeError Unsupported operand types
resource / (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
resource / (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
resource / DateTime - TypeError Unsupported operand types
resource / resource - TypeError Unsupported operand types
resource / NULL - TypeError Unsupported operand types
NULL / false - TypeError Unsupported operand types
NULL / true - TypeError Unsupported operand types
NULL / 0 - TypeError Unsupported operand types
NULL / 10 - TypeError Unsupported operand types
NULL / 0.0 - TypeError Unsupported operand types
NULL / 10.0 - TypeError Unsupported operand types
NULL / 3.14 - TypeError Unsupported operand types
NULL / '0' - TypeError Unsupported operand types
NULL / '10' - TypeError Unsupported operand types
NULL / '010' - TypeError Unsupported operand types
NULL / '10 elephants' - TypeError Unsupported operand types
NULL / 'foo' - TypeError Unsupported operand types
NULL / array ( ) - TypeError Unsupported operand types
NULL / array ( 0 => 1 ) - TypeError Unsupported operand types
NULL / array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
NULL / array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
NULL / array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
NULL / (object) array ( ) - TypeError Unsupported operand types
NULL / (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
NULL / (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
NULL / DateTime - TypeError Unsupported operand types
NULL / resource - TypeError Unsupported operand types
NULL / NULL - TypeError Unsupported operand types