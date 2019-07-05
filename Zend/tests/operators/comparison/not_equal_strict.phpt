--TEST--
not equal '!=' operator with strict_operators
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a != $b', function($a, $b) { return $a != $b; });

--EXPECT--
false != false = false
false != true = false
false != 0 - TypeError Type mismatch
false != 10 - TypeError Type mismatch
false != 0.0 - TypeError Type mismatch
false != 10.0 - TypeError Type mismatch
false != 3.14 - TypeError Type mismatch
false != '0' - TypeError Type mismatch
false != '10' - TypeError Type mismatch
false != '010' - TypeError Type mismatch
false != '10 elephants' - TypeError Type mismatch
false != 'foo' - TypeError Type mismatch
false != array ( ) - TypeError Type mismatch
false != array ( 0 => 1 ) - TypeError Type mismatch
false != array ( 0 => 1, 1 => 100 ) - TypeError Type mismatch
false != array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
false != array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
false != (object) array ( ) - TypeError Type mismatch
false != (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
false != (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
false != DateTime - TypeError Type mismatch
false != resource - TypeError Type mismatch
false != NULL - TypeError Type mismatch
true != false = false
true != true = false
true != 0 - TypeError Type mismatch
true != 10 - TypeError Type mismatch
true != 0.0 - TypeError Type mismatch
true != 10.0 - TypeError Type mismatch
true != 3.14 - TypeError Type mismatch
true != '0' - TypeError Type mismatch
true != '10' - TypeError Type mismatch
true != '010' - TypeError Type mismatch
true != '10 elephants' - TypeError Type mismatch
true != 'foo' - TypeError Type mismatch
true != array ( ) - TypeError Type mismatch
true != array ( 0 => 1 ) - TypeError Type mismatch
true != array ( 0 => 1, 1 => 100 ) - TypeError Type mismatch
true != array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
true != array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
true != (object) array ( ) - TypeError Type mismatch
true != (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
true != (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
true != DateTime - TypeError Type mismatch
true != resource - TypeError Type mismatch
true != NULL - TypeError Type mismatch
0 != false - TypeError Type mismatch
0 != true - TypeError Type mismatch
0 != 0 = false
0 != 10 = true
0 != 0.0 = false
0 != 10.0 = true
0 != 3.14 = true
0 != '0' - TypeError Type mismatch
0 != '10' - TypeError Type mismatch
0 != '010' - TypeError Type mismatch
0 != '10 elephants' - TypeError Type mismatch
0 != 'foo' - TypeError Type mismatch
0 != array ( ) - TypeError Type mismatch
0 != array ( 0 => 1 ) - TypeError Type mismatch
0 != array ( 0 => 1, 1 => 100 ) - TypeError Type mismatch
0 != array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
0 != array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
0 != (object) array ( ) - TypeError Type mismatch
0 != (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
0 != (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
0 != DateTime - TypeError Type mismatch
0 != resource - TypeError Type mismatch
0 != NULL - TypeError Type mismatch
10 != false - TypeError Type mismatch
10 != true - TypeError Type mismatch
10 != 0 = true
10 != 10 = false
10 != 0.0 = true
10 != 10.0 = false
10 != 3.14 = true
10 != '0' - TypeError Type mismatch
10 != '10' - TypeError Type mismatch
10 != '010' - TypeError Type mismatch
10 != '10 elephants' - TypeError Type mismatch
10 != 'foo' - TypeError Type mismatch
10 != array ( ) - TypeError Type mismatch
10 != array ( 0 => 1 ) - TypeError Type mismatch
10 != array ( 0 => 1, 1 => 100 ) - TypeError Type mismatch
10 != array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
10 != array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
10 != (object) array ( ) - TypeError Type mismatch
10 != (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
10 != (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
10 != DateTime - TypeError Type mismatch
10 != resource - TypeError Type mismatch
10 != NULL - TypeError Type mismatch
0.0 != false - TypeError Type mismatch
0.0 != true - TypeError Type mismatch
0.0 != 0 = false
0.0 != 10 = true
0.0 != 0.0 = false
0.0 != 10.0 = true
0.0 != 3.14 = true
0.0 != '0' - TypeError Type mismatch
0.0 != '10' - TypeError Type mismatch
0.0 != '010' - TypeError Type mismatch
0.0 != '10 elephants' - TypeError Type mismatch
0.0 != 'foo' - TypeError Type mismatch
0.0 != array ( ) - TypeError Type mismatch
0.0 != array ( 0 => 1 ) - TypeError Type mismatch
0.0 != array ( 0 => 1, 1 => 100 ) - TypeError Type mismatch
0.0 != array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
0.0 != array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
0.0 != (object) array ( ) - TypeError Type mismatch
0.0 != (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
0.0 != (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
0.0 != DateTime - TypeError Type mismatch
0.0 != resource - TypeError Type mismatch
0.0 != NULL - TypeError Type mismatch
10.0 != false - TypeError Type mismatch
10.0 != true - TypeError Type mismatch
10.0 != 0 = true
10.0 != 10 = false
10.0 != 0.0 = true
10.0 != 10.0 = false
10.0 != 3.14 = true
10.0 != '0' - TypeError Type mismatch
10.0 != '10' - TypeError Type mismatch
10.0 != '010' - TypeError Type mismatch
10.0 != '10 elephants' - TypeError Type mismatch
10.0 != 'foo' - TypeError Type mismatch
10.0 != array ( ) - TypeError Type mismatch
10.0 != array ( 0 => 1 ) - TypeError Type mismatch
10.0 != array ( 0 => 1, 1 => 100 ) - TypeError Type mismatch
10.0 != array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
10.0 != array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
10.0 != (object) array ( ) - TypeError Type mismatch
10.0 != (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
10.0 != (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
10.0 != DateTime - TypeError Type mismatch
10.0 != resource - TypeError Type mismatch
10.0 != NULL - TypeError Type mismatch
3.14 != false - TypeError Type mismatch
3.14 != true - TypeError Type mismatch
3.14 != 0 = true
3.14 != 10 = true
3.14 != 0.0 = true
3.14 != 10.0 = true
3.14 != 3.14 = false
3.14 != '0' - TypeError Type mismatch
3.14 != '10' - TypeError Type mismatch
3.14 != '010' - TypeError Type mismatch
3.14 != '10 elephants' - TypeError Type mismatch
3.14 != 'foo' - TypeError Type mismatch
3.14 != array ( ) - TypeError Type mismatch
3.14 != array ( 0 => 1 ) - TypeError Type mismatch
3.14 != array ( 0 => 1, 1 => 100 ) - TypeError Type mismatch
3.14 != array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
3.14 != array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
3.14 != (object) array ( ) - TypeError Type mismatch
3.14 != (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
3.14 != (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
3.14 != DateTime - TypeError Type mismatch
3.14 != resource - TypeError Type mismatch
3.14 != NULL - TypeError Type mismatch
'0' != false - TypeError Type mismatch
'0' != true - TypeError Type mismatch
'0' != 0 - TypeError Type mismatch
'0' != 10 - TypeError Type mismatch
'0' != 0.0 - TypeError Type mismatch
'0' != 10.0 - TypeError Type mismatch
'0' != 3.14 - TypeError Type mismatch
'0' != '0' = false
'0' != '10' = true
'0' != '010' = true
'0' != '10 elephants' = true
'0' != 'foo' = true
'0' != array ( ) - TypeError Type mismatch
'0' != array ( 0 => 1 ) - TypeError Type mismatch
'0' != array ( 0 => 1, 1 => 100 ) - TypeError Type mismatch
'0' != array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
'0' != array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
'0' != (object) array ( ) - TypeError Type mismatch
'0' != (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
'0' != (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
'0' != DateTime - TypeError Type mismatch
'0' != resource - TypeError Type mismatch
'0' != NULL - TypeError Type mismatch
'10' != false - TypeError Type mismatch
'10' != true - TypeError Type mismatch
'10' != 0 - TypeError Type mismatch
'10' != 10 - TypeError Type mismatch
'10' != 0.0 - TypeError Type mismatch
'10' != 10.0 - TypeError Type mismatch
'10' != 3.14 - TypeError Type mismatch
'10' != '0' = true
'10' != '10' = false
'10' != '010' = true
'10' != '10 elephants' = true
'10' != 'foo' = true
'10' != array ( ) - TypeError Type mismatch
'10' != array ( 0 => 1 ) - TypeError Type mismatch
'10' != array ( 0 => 1, 1 => 100 ) - TypeError Type mismatch
'10' != array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
'10' != array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
'10' != (object) array ( ) - TypeError Type mismatch
'10' != (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
'10' != (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
'10' != DateTime - TypeError Type mismatch
'10' != resource - TypeError Type mismatch
'10' != NULL - TypeError Type mismatch
'010' != false - TypeError Type mismatch
'010' != true - TypeError Type mismatch
'010' != 0 - TypeError Type mismatch
'010' != 10 - TypeError Type mismatch
'010' != 0.0 - TypeError Type mismatch
'010' != 10.0 - TypeError Type mismatch
'010' != 3.14 - TypeError Type mismatch
'010' != '0' = true
'010' != '10' = true
'010' != '010' = false
'010' != '10 elephants' = true
'010' != 'foo' = true
'010' != array ( ) - TypeError Type mismatch
'010' != array ( 0 => 1 ) - TypeError Type mismatch
'010' != array ( 0 => 1, 1 => 100 ) - TypeError Type mismatch
'010' != array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
'010' != array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
'010' != (object) array ( ) - TypeError Type mismatch
'010' != (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
'010' != (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
'010' != DateTime - TypeError Type mismatch
'010' != resource - TypeError Type mismatch
'010' != NULL - TypeError Type mismatch
'10 elephants' != false - TypeError Type mismatch
'10 elephants' != true - TypeError Type mismatch
'10 elephants' != 0 - TypeError Type mismatch
'10 elephants' != 10 - TypeError Type mismatch
'10 elephants' != 0.0 - TypeError Type mismatch
'10 elephants' != 10.0 - TypeError Type mismatch
'10 elephants' != 3.14 - TypeError Type mismatch
'10 elephants' != '0' = true
'10 elephants' != '10' = true
'10 elephants' != '010' = true
'10 elephants' != '10 elephants' = false
'10 elephants' != 'foo' = true
'10 elephants' != array ( ) - TypeError Type mismatch
'10 elephants' != array ( 0 => 1 ) - TypeError Type mismatch
'10 elephants' != array ( 0 => 1, 1 => 100 ) - TypeError Type mismatch
'10 elephants' != array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
'10 elephants' != array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
'10 elephants' != (object) array ( ) - TypeError Type mismatch
'10 elephants' != (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
'10 elephants' != (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
'10 elephants' != DateTime - TypeError Type mismatch
'10 elephants' != resource - TypeError Type mismatch
'10 elephants' != NULL - TypeError Type mismatch
'foo' != false - TypeError Type mismatch
'foo' != true - TypeError Type mismatch
'foo' != 0 - TypeError Type mismatch
'foo' != 10 - TypeError Type mismatch
'foo' != 0.0 - TypeError Type mismatch
'foo' != 10.0 - TypeError Type mismatch
'foo' != 3.14 - TypeError Type mismatch
'foo' != '0' = true
'foo' != '10' = true
'foo' != '010' = true
'foo' != '10 elephants' = true
'foo' != 'foo' = false
'foo' != array ( ) - TypeError Type mismatch
'foo' != array ( 0 => 1 ) - TypeError Type mismatch
'foo' != array ( 0 => 1, 1 => 100 ) - TypeError Type mismatch
'foo' != array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
'foo' != array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
'foo' != (object) array ( ) - TypeError Type mismatch
'foo' != (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
'foo' != (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
'foo' != DateTime - TypeError Type mismatch
'foo' != resource - TypeError Type mismatch
'foo' != NULL - TypeError Type mismatch
array ( ) != false - TypeError Type mismatch
array ( ) != true - TypeError Type mismatch
array ( ) != 0 - TypeError Type mismatch
array ( ) != 10 - TypeError Type mismatch
array ( ) != 0.0 - TypeError Type mismatch
array ( ) != 10.0 - TypeError Type mismatch
array ( ) != 3.14 - TypeError Type mismatch
array ( ) != '0' - TypeError Type mismatch
array ( ) != '10' - TypeError Type mismatch
array ( ) != '010' - TypeError Type mismatch
array ( ) != '10 elephants' - TypeError Type mismatch
array ( ) != 'foo' - TypeError Type mismatch
array ( ) != array ( ) = false
array ( ) != array ( 0 => 1 ) = false
array ( ) != array ( 0 => 1, 1 => 100 ) = false
array ( ) != array ( 'foo' => 1, 'bar' => 2 ) = false
array ( ) != array ( 'bar' => 1, 'foo' => 2 ) = false
array ( ) != (object) array ( ) - TypeError Type mismatch
array ( ) != (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
array ( ) != (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
array ( ) != DateTime - TypeError Type mismatch
array ( ) != resource - TypeError Type mismatch
array ( ) != NULL - TypeError Type mismatch
array ( 0 => 1 ) != false - TypeError Type mismatch
array ( 0 => 1 ) != true - TypeError Type mismatch
array ( 0 => 1 ) != 0 - TypeError Type mismatch
array ( 0 => 1 ) != 10 - TypeError Type mismatch
array ( 0 => 1 ) != 0.0 - TypeError Type mismatch
array ( 0 => 1 ) != 10.0 - TypeError Type mismatch
array ( 0 => 1 ) != 3.14 - TypeError Type mismatch
array ( 0 => 1 ) != '0' - TypeError Type mismatch
array ( 0 => 1 ) != '10' - TypeError Type mismatch
array ( 0 => 1 ) != '010' - TypeError Type mismatch
array ( 0 => 1 ) != '10 elephants' - TypeError Type mismatch
array ( 0 => 1 ) != 'foo' - TypeError Type mismatch
array ( 0 => 1 ) != array ( ) = false
array ( 0 => 1 ) != array ( 0 => 1 ) = false
array ( 0 => 1 ) != array ( 0 => 1, 1 => 100 ) = false
array ( 0 => 1 ) != array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 0 => 1 ) != array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 0 => 1 ) != (object) array ( ) - TypeError Type mismatch
array ( 0 => 1 ) != (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
array ( 0 => 1 ) != (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
array ( 0 => 1 ) != DateTime - TypeError Type mismatch
array ( 0 => 1 ) != resource - TypeError Type mismatch
array ( 0 => 1 ) != NULL - TypeError Type mismatch
array ( 0 => 1, 1 => 100 ) != false - TypeError Type mismatch
array ( 0 => 1, 1 => 100 ) != true - TypeError Type mismatch
array ( 0 => 1, 1 => 100 ) != 0 - TypeError Type mismatch
array ( 0 => 1, 1 => 100 ) != 10 - TypeError Type mismatch
array ( 0 => 1, 1 => 100 ) != 0.0 - TypeError Type mismatch
array ( 0 => 1, 1 => 100 ) != 10.0 - TypeError Type mismatch
array ( 0 => 1, 1 => 100 ) != 3.14 - TypeError Type mismatch
array ( 0 => 1, 1 => 100 ) != '0' - TypeError Type mismatch
array ( 0 => 1, 1 => 100 ) != '10' - TypeError Type mismatch
array ( 0 => 1, 1 => 100 ) != '010' - TypeError Type mismatch
array ( 0 => 1, 1 => 100 ) != '10 elephants' - TypeError Type mismatch
array ( 0 => 1, 1 => 100 ) != 'foo' - TypeError Type mismatch
array ( 0 => 1, 1 => 100 ) != array ( ) = false
array ( 0 => 1, 1 => 100 ) != array ( 0 => 1 ) = false
array ( 0 => 1, 1 => 100 ) != array ( 0 => 1, 1 => 100 ) = false
array ( 0 => 1, 1 => 100 ) != array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 0 => 1, 1 => 100 ) != array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 0 => 1, 1 => 100 ) != (object) array ( ) - TypeError Type mismatch
array ( 0 => 1, 1 => 100 ) != (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
array ( 0 => 1, 1 => 100 ) != (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
array ( 0 => 1, 1 => 100 ) != DateTime - TypeError Type mismatch
array ( 0 => 1, 1 => 100 ) != resource - TypeError Type mismatch
array ( 0 => 1, 1 => 100 ) != NULL - TypeError Type mismatch
array ( 'foo' => 1, 'bar' => 2 ) != false - TypeError Type mismatch
array ( 'foo' => 1, 'bar' => 2 ) != true - TypeError Type mismatch
array ( 'foo' => 1, 'bar' => 2 ) != 0 - TypeError Type mismatch
array ( 'foo' => 1, 'bar' => 2 ) != 10 - TypeError Type mismatch
array ( 'foo' => 1, 'bar' => 2 ) != 0.0 - TypeError Type mismatch
array ( 'foo' => 1, 'bar' => 2 ) != 10.0 - TypeError Type mismatch
array ( 'foo' => 1, 'bar' => 2 ) != 3.14 - TypeError Type mismatch
array ( 'foo' => 1, 'bar' => 2 ) != '0' - TypeError Type mismatch
array ( 'foo' => 1, 'bar' => 2 ) != '10' - TypeError Type mismatch
array ( 'foo' => 1, 'bar' => 2 ) != '010' - TypeError Type mismatch
array ( 'foo' => 1, 'bar' => 2 ) != '10 elephants' - TypeError Type mismatch
array ( 'foo' => 1, 'bar' => 2 ) != 'foo' - TypeError Type mismatch
array ( 'foo' => 1, 'bar' => 2 ) != array ( ) = false
array ( 'foo' => 1, 'bar' => 2 ) != array ( 0 => 1 ) = false
array ( 'foo' => 1, 'bar' => 2 ) != array ( 0 => 1, 1 => 100 ) = false
array ( 'foo' => 1, 'bar' => 2 ) != array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 'foo' => 1, 'bar' => 2 ) != array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 'foo' => 1, 'bar' => 2 ) != (object) array ( ) - TypeError Type mismatch
array ( 'foo' => 1, 'bar' => 2 ) != (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
array ( 'foo' => 1, 'bar' => 2 ) != (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
array ( 'foo' => 1, 'bar' => 2 ) != DateTime - TypeError Type mismatch
array ( 'foo' => 1, 'bar' => 2 ) != resource - TypeError Type mismatch
array ( 'foo' => 1, 'bar' => 2 ) != NULL - TypeError Type mismatch
array ( 'bar' => 1, 'foo' => 2 ) != false - TypeError Type mismatch
array ( 'bar' => 1, 'foo' => 2 ) != true - TypeError Type mismatch
array ( 'bar' => 1, 'foo' => 2 ) != 0 - TypeError Type mismatch
array ( 'bar' => 1, 'foo' => 2 ) != 10 - TypeError Type mismatch
array ( 'bar' => 1, 'foo' => 2 ) != 0.0 - TypeError Type mismatch
array ( 'bar' => 1, 'foo' => 2 ) != 10.0 - TypeError Type mismatch
array ( 'bar' => 1, 'foo' => 2 ) != 3.14 - TypeError Type mismatch
array ( 'bar' => 1, 'foo' => 2 ) != '0' - TypeError Type mismatch
array ( 'bar' => 1, 'foo' => 2 ) != '10' - TypeError Type mismatch
array ( 'bar' => 1, 'foo' => 2 ) != '010' - TypeError Type mismatch
array ( 'bar' => 1, 'foo' => 2 ) != '10 elephants' - TypeError Type mismatch
array ( 'bar' => 1, 'foo' => 2 ) != 'foo' - TypeError Type mismatch
array ( 'bar' => 1, 'foo' => 2 ) != array ( ) = false
array ( 'bar' => 1, 'foo' => 2 ) != array ( 0 => 1 ) = false
array ( 'bar' => 1, 'foo' => 2 ) != array ( 0 => 1, 1 => 100 ) = false
array ( 'bar' => 1, 'foo' => 2 ) != array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 'bar' => 1, 'foo' => 2 ) != array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 'bar' => 1, 'foo' => 2 ) != (object) array ( ) - TypeError Type mismatch
array ( 'bar' => 1, 'foo' => 2 ) != (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
array ( 'bar' => 1, 'foo' => 2 ) != (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
array ( 'bar' => 1, 'foo' => 2 ) != DateTime - TypeError Type mismatch
array ( 'bar' => 1, 'foo' => 2 ) != resource - TypeError Type mismatch
array ( 'bar' => 1, 'foo' => 2 ) != NULL - TypeError Type mismatch
(object) array ( ) != false - TypeError Type mismatch
(object) array ( ) != true - TypeError Type mismatch
(object) array ( ) != 0 - TypeError Type mismatch
(object) array ( ) != 10 - TypeError Type mismatch
(object) array ( ) != 0.0 - TypeError Type mismatch
(object) array ( ) != 10.0 - TypeError Type mismatch
(object) array ( ) != 3.14 - TypeError Type mismatch
(object) array ( ) != '0' - TypeError Type mismatch
(object) array ( ) != '10' - TypeError Type mismatch
(object) array ( ) != '010' - TypeError Type mismatch
(object) array ( ) != '10 elephants' - TypeError Type mismatch
(object) array ( ) != 'foo' - TypeError Type mismatch
(object) array ( ) != array ( ) - TypeError Type mismatch
(object) array ( ) != array ( 0 => 1 ) - TypeError Type mismatch
(object) array ( ) != array ( 0 => 1, 1 => 100 ) - TypeError Type mismatch
(object) array ( ) != array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
(object) array ( ) != array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
(object) array ( ) != (object) array ( ) = false
(object) array ( ) != (object) array ( 'foo' => 1, 'bar' => 2 ) = false
(object) array ( ) != (object) array ( 'bar' => 1, 'foo' => 2 ) = false
(object) array ( ) != DateTime - TypeError Type mismatch
(object) array ( ) != resource - TypeError Type mismatch
(object) array ( ) != NULL - TypeError Type mismatch
(object) array ( 'foo' => 1, 'bar' => 2 ) != false - TypeError Type mismatch
(object) array ( 'foo' => 1, 'bar' => 2 ) != true - TypeError Type mismatch
(object) array ( 'foo' => 1, 'bar' => 2 ) != 0 - TypeError Type mismatch
(object) array ( 'foo' => 1, 'bar' => 2 ) != 10 - TypeError Type mismatch
(object) array ( 'foo' => 1, 'bar' => 2 ) != 0.0 - TypeError Type mismatch
(object) array ( 'foo' => 1, 'bar' => 2 ) != 10.0 - TypeError Type mismatch
(object) array ( 'foo' => 1, 'bar' => 2 ) != 3.14 - TypeError Type mismatch
(object) array ( 'foo' => 1, 'bar' => 2 ) != '0' - TypeError Type mismatch
(object) array ( 'foo' => 1, 'bar' => 2 ) != '10' - TypeError Type mismatch
(object) array ( 'foo' => 1, 'bar' => 2 ) != '010' - TypeError Type mismatch
(object) array ( 'foo' => 1, 'bar' => 2 ) != '10 elephants' - TypeError Type mismatch
(object) array ( 'foo' => 1, 'bar' => 2 ) != 'foo' - TypeError Type mismatch
(object) array ( 'foo' => 1, 'bar' => 2 ) != array ( ) - TypeError Type mismatch
(object) array ( 'foo' => 1, 'bar' => 2 ) != array ( 0 => 1 ) - TypeError Type mismatch
(object) array ( 'foo' => 1, 'bar' => 2 ) != array ( 0 => 1, 1 => 100 ) - TypeError Type mismatch
(object) array ( 'foo' => 1, 'bar' => 2 ) != array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
(object) array ( 'foo' => 1, 'bar' => 2 ) != array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
(object) array ( 'foo' => 1, 'bar' => 2 ) != (object) array ( ) = false
(object) array ( 'foo' => 1, 'bar' => 2 ) != (object) array ( 'foo' => 1, 'bar' => 2 ) = false
(object) array ( 'foo' => 1, 'bar' => 2 ) != (object) array ( 'bar' => 1, 'foo' => 2 ) = false
(object) array ( 'foo' => 1, 'bar' => 2 ) != DateTime - TypeError Type mismatch
(object) array ( 'foo' => 1, 'bar' => 2 ) != resource - TypeError Type mismatch
(object) array ( 'foo' => 1, 'bar' => 2 ) != NULL - TypeError Type mismatch
(object) array ( 'bar' => 1, 'foo' => 2 ) != false - TypeError Type mismatch
(object) array ( 'bar' => 1, 'foo' => 2 ) != true - TypeError Type mismatch
(object) array ( 'bar' => 1, 'foo' => 2 ) != 0 - TypeError Type mismatch
(object) array ( 'bar' => 1, 'foo' => 2 ) != 10 - TypeError Type mismatch
(object) array ( 'bar' => 1, 'foo' => 2 ) != 0.0 - TypeError Type mismatch
(object) array ( 'bar' => 1, 'foo' => 2 ) != 10.0 - TypeError Type mismatch
(object) array ( 'bar' => 1, 'foo' => 2 ) != 3.14 - TypeError Type mismatch
(object) array ( 'bar' => 1, 'foo' => 2 ) != '0' - TypeError Type mismatch
(object) array ( 'bar' => 1, 'foo' => 2 ) != '10' - TypeError Type mismatch
(object) array ( 'bar' => 1, 'foo' => 2 ) != '010' - TypeError Type mismatch
(object) array ( 'bar' => 1, 'foo' => 2 ) != '10 elephants' - TypeError Type mismatch
(object) array ( 'bar' => 1, 'foo' => 2 ) != 'foo' - TypeError Type mismatch
(object) array ( 'bar' => 1, 'foo' => 2 ) != array ( ) - TypeError Type mismatch
(object) array ( 'bar' => 1, 'foo' => 2 ) != array ( 0 => 1 ) - TypeError Type mismatch
(object) array ( 'bar' => 1, 'foo' => 2 ) != array ( 0 => 1, 1 => 100 ) - TypeError Type mismatch
(object) array ( 'bar' => 1, 'foo' => 2 ) != array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
(object) array ( 'bar' => 1, 'foo' => 2 ) != array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
(object) array ( 'bar' => 1, 'foo' => 2 ) != (object) array ( ) = false
(object) array ( 'bar' => 1, 'foo' => 2 ) != (object) array ( 'foo' => 1, 'bar' => 2 ) = false
(object) array ( 'bar' => 1, 'foo' => 2 ) != (object) array ( 'bar' => 1, 'foo' => 2 ) = false
(object) array ( 'bar' => 1, 'foo' => 2 ) != DateTime - TypeError Type mismatch
(object) array ( 'bar' => 1, 'foo' => 2 ) != resource - TypeError Type mismatch
(object) array ( 'bar' => 1, 'foo' => 2 ) != NULL - TypeError Type mismatch
DateTime != false - TypeError Type mismatch
DateTime != true - TypeError Type mismatch
DateTime != 0 - TypeError Type mismatch
DateTime != 10 - TypeError Type mismatch
DateTime != 0.0 - TypeError Type mismatch
DateTime != 10.0 - TypeError Type mismatch
DateTime != 3.14 - TypeError Type mismatch
DateTime != '0' - TypeError Type mismatch
DateTime != '10' - TypeError Type mismatch
DateTime != '010' - TypeError Type mismatch
DateTime != '10 elephants' - TypeError Type mismatch
DateTime != 'foo' - TypeError Type mismatch
DateTime != array ( ) - TypeError Type mismatch
DateTime != array ( 0 => 1 ) - TypeError Type mismatch
DateTime != array ( 0 => 1, 1 => 100 ) - TypeError Type mismatch
DateTime != array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
DateTime != array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
DateTime != (object) array ( ) - TypeError Type mismatch
DateTime != (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
DateTime != (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
DateTime != DateTime = false
DateTime != resource - TypeError Type mismatch
DateTime != NULL - TypeError Type mismatch
resource != false - TypeError Type mismatch
resource != true - TypeError Type mismatch
resource != 0 - TypeError Type mismatch
resource != 10 - TypeError Type mismatch
resource != 0.0 - TypeError Type mismatch
resource != 10.0 - TypeError Type mismatch
resource != 3.14 - TypeError Type mismatch
resource != '0' - TypeError Type mismatch
resource != '10' - TypeError Type mismatch
resource != '010' - TypeError Type mismatch
resource != '10 elephants' - TypeError Type mismatch
resource != 'foo' - TypeError Type mismatch
resource != array ( ) - TypeError Type mismatch
resource != array ( 0 => 1 ) - TypeError Type mismatch
resource != array ( 0 => 1, 1 => 100 ) - TypeError Type mismatch
resource != array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
resource != array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
resource != (object) array ( ) - TypeError Type mismatch
resource != (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
resource != (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
resource != DateTime - TypeError Type mismatch
resource != resource = false
resource != NULL - TypeError Type mismatch
NULL != false - TypeError Type mismatch
NULL != true - TypeError Type mismatch
NULL != 0 - TypeError Type mismatch
NULL != 10 - TypeError Type mismatch
NULL != 0.0 - TypeError Type mismatch
NULL != 10.0 - TypeError Type mismatch
NULL != 3.14 - TypeError Type mismatch
NULL != '0' - TypeError Type mismatch
NULL != '10' - TypeError Type mismatch
NULL != '010' - TypeError Type mismatch
NULL != '10 elephants' - TypeError Type mismatch
NULL != 'foo' - TypeError Type mismatch
NULL != array ( ) - TypeError Type mismatch
NULL != array ( 0 => 1 ) - TypeError Type mismatch
NULL != array ( 0 => 1, 1 => 100 ) - TypeError Type mismatch
NULL != array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
NULL != array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
NULL != (object) array ( ) - TypeError Type mismatch
NULL != (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Type mismatch
NULL != (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Type mismatch
NULL != DateTime - TypeError Type mismatch
NULL != resource - TypeError Type mismatch
NULL != NULL = false