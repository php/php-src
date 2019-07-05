--TEST--
logical xor operator
--FILE--
<?php
require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a xor $b', function($a, $b) { return $a xor $b; });

--EXPECT--
false xor false = false
false xor true = true
false xor 0 = false
false xor 10 = true
false xor 0.0 = false
false xor 10.0 = true
false xor 3.14 = true
false xor '0' = false
false xor '10' = true
false xor '010' = true
false xor '10 elephants' = true
false xor 'foo' = true
false xor array ( ) = false
false xor array ( 0 => 1 ) = true
false xor array ( 0 => 1, 1 => 100 ) = true
false xor array ( 'foo' => 1, 'bar' => 2 ) = true
false xor array ( 'bar' => 1, 'foo' => 2 ) = true
false xor (object) array ( ) = true
false xor (object) array ( 'foo' => 1, 'bar' => 2 ) = true
false xor (object) array ( 'bar' => 1, 'foo' => 2 ) = true
false xor DateTime = true
false xor resource = true
false xor NULL = false
true xor false = true
true xor true = false
true xor 0 = true
true xor 10 = false
true xor 0.0 = true
true xor 10.0 = false
true xor 3.14 = false
true xor '0' = true
true xor '10' = false
true xor '010' = false
true xor '10 elephants' = false
true xor 'foo' = false
true xor array ( ) = true
true xor array ( 0 => 1 ) = false
true xor array ( 0 => 1, 1 => 100 ) = false
true xor array ( 'foo' => 1, 'bar' => 2 ) = false
true xor array ( 'bar' => 1, 'foo' => 2 ) = false
true xor (object) array ( ) = false
true xor (object) array ( 'foo' => 1, 'bar' => 2 ) = false
true xor (object) array ( 'bar' => 1, 'foo' => 2 ) = false
true xor DateTime = false
true xor resource = false
true xor NULL = true
0 xor false = false
0 xor true = true
0 xor 0 = false
0 xor 10 = true
0 xor 0.0 = false
0 xor 10.0 = true
0 xor 3.14 = true
0 xor '0' = false
0 xor '10' = true
0 xor '010' = true
0 xor '10 elephants' = true
0 xor 'foo' = true
0 xor array ( ) = false
0 xor array ( 0 => 1 ) = true
0 xor array ( 0 => 1, 1 => 100 ) = true
0 xor array ( 'foo' => 1, 'bar' => 2 ) = true
0 xor array ( 'bar' => 1, 'foo' => 2 ) = true
0 xor (object) array ( ) = true
0 xor (object) array ( 'foo' => 1, 'bar' => 2 ) = true
0 xor (object) array ( 'bar' => 1, 'foo' => 2 ) = true
0 xor DateTime = true
0 xor resource = true
0 xor NULL = false
10 xor false = true
10 xor true = false
10 xor 0 = true
10 xor 10 = false
10 xor 0.0 = true
10 xor 10.0 = false
10 xor 3.14 = false
10 xor '0' = true
10 xor '10' = false
10 xor '010' = false
10 xor '10 elephants' = false
10 xor 'foo' = false
10 xor array ( ) = true
10 xor array ( 0 => 1 ) = false
10 xor array ( 0 => 1, 1 => 100 ) = false
10 xor array ( 'foo' => 1, 'bar' => 2 ) = false
10 xor array ( 'bar' => 1, 'foo' => 2 ) = false
10 xor (object) array ( ) = false
10 xor (object) array ( 'foo' => 1, 'bar' => 2 ) = false
10 xor (object) array ( 'bar' => 1, 'foo' => 2 ) = false
10 xor DateTime = false
10 xor resource = false
10 xor NULL = true
0.0 xor false = false
0.0 xor true = true
0.0 xor 0 = false
0.0 xor 10 = true
0.0 xor 0.0 = false
0.0 xor 10.0 = true
0.0 xor 3.14 = true
0.0 xor '0' = false
0.0 xor '10' = true
0.0 xor '010' = true
0.0 xor '10 elephants' = true
0.0 xor 'foo' = true
0.0 xor array ( ) = false
0.0 xor array ( 0 => 1 ) = true
0.0 xor array ( 0 => 1, 1 => 100 ) = true
0.0 xor array ( 'foo' => 1, 'bar' => 2 ) = true
0.0 xor array ( 'bar' => 1, 'foo' => 2 ) = true
0.0 xor (object) array ( ) = true
0.0 xor (object) array ( 'foo' => 1, 'bar' => 2 ) = true
0.0 xor (object) array ( 'bar' => 1, 'foo' => 2 ) = true
0.0 xor DateTime = true
0.0 xor resource = true
0.0 xor NULL = false
10.0 xor false = true
10.0 xor true = false
10.0 xor 0 = true
10.0 xor 10 = false
10.0 xor 0.0 = true
10.0 xor 10.0 = false
10.0 xor 3.14 = false
10.0 xor '0' = true
10.0 xor '10' = false
10.0 xor '010' = false
10.0 xor '10 elephants' = false
10.0 xor 'foo' = false
10.0 xor array ( ) = true
10.0 xor array ( 0 => 1 ) = false
10.0 xor array ( 0 => 1, 1 => 100 ) = false
10.0 xor array ( 'foo' => 1, 'bar' => 2 ) = false
10.0 xor array ( 'bar' => 1, 'foo' => 2 ) = false
10.0 xor (object) array ( ) = false
10.0 xor (object) array ( 'foo' => 1, 'bar' => 2 ) = false
10.0 xor (object) array ( 'bar' => 1, 'foo' => 2 ) = false
10.0 xor DateTime = false
10.0 xor resource = false
10.0 xor NULL = true
3.14 xor false = true
3.14 xor true = false
3.14 xor 0 = true
3.14 xor 10 = false
3.14 xor 0.0 = true
3.14 xor 10.0 = false
3.14 xor 3.14 = false
3.14 xor '0' = true
3.14 xor '10' = false
3.14 xor '010' = false
3.14 xor '10 elephants' = false
3.14 xor 'foo' = false
3.14 xor array ( ) = true
3.14 xor array ( 0 => 1 ) = false
3.14 xor array ( 0 => 1, 1 => 100 ) = false
3.14 xor array ( 'foo' => 1, 'bar' => 2 ) = false
3.14 xor array ( 'bar' => 1, 'foo' => 2 ) = false
3.14 xor (object) array ( ) = false
3.14 xor (object) array ( 'foo' => 1, 'bar' => 2 ) = false
3.14 xor (object) array ( 'bar' => 1, 'foo' => 2 ) = false
3.14 xor DateTime = false
3.14 xor resource = false
3.14 xor NULL = true
'0' xor false = false
'0' xor true = true
'0' xor 0 = false
'0' xor 10 = true
'0' xor 0.0 = false
'0' xor 10.0 = true
'0' xor 3.14 = true
'0' xor '0' = false
'0' xor '10' = true
'0' xor '010' = true
'0' xor '10 elephants' = true
'0' xor 'foo' = true
'0' xor array ( ) = false
'0' xor array ( 0 => 1 ) = true
'0' xor array ( 0 => 1, 1 => 100 ) = true
'0' xor array ( 'foo' => 1, 'bar' => 2 ) = true
'0' xor array ( 'bar' => 1, 'foo' => 2 ) = true
'0' xor (object) array ( ) = true
'0' xor (object) array ( 'foo' => 1, 'bar' => 2 ) = true
'0' xor (object) array ( 'bar' => 1, 'foo' => 2 ) = true
'0' xor DateTime = true
'0' xor resource = true
'0' xor NULL = false
'10' xor false = true
'10' xor true = false
'10' xor 0 = true
'10' xor 10 = false
'10' xor 0.0 = true
'10' xor 10.0 = false
'10' xor 3.14 = false
'10' xor '0' = true
'10' xor '10' = false
'10' xor '010' = false
'10' xor '10 elephants' = false
'10' xor 'foo' = false
'10' xor array ( ) = true
'10' xor array ( 0 => 1 ) = false
'10' xor array ( 0 => 1, 1 => 100 ) = false
'10' xor array ( 'foo' => 1, 'bar' => 2 ) = false
'10' xor array ( 'bar' => 1, 'foo' => 2 ) = false
'10' xor (object) array ( ) = false
'10' xor (object) array ( 'foo' => 1, 'bar' => 2 ) = false
'10' xor (object) array ( 'bar' => 1, 'foo' => 2 ) = false
'10' xor DateTime = false
'10' xor resource = false
'10' xor NULL = true
'010' xor false = true
'010' xor true = false
'010' xor 0 = true
'010' xor 10 = false
'010' xor 0.0 = true
'010' xor 10.0 = false
'010' xor 3.14 = false
'010' xor '0' = true
'010' xor '10' = false
'010' xor '010' = false
'010' xor '10 elephants' = false
'010' xor 'foo' = false
'010' xor array ( ) = true
'010' xor array ( 0 => 1 ) = false
'010' xor array ( 0 => 1, 1 => 100 ) = false
'010' xor array ( 'foo' => 1, 'bar' => 2 ) = false
'010' xor array ( 'bar' => 1, 'foo' => 2 ) = false
'010' xor (object) array ( ) = false
'010' xor (object) array ( 'foo' => 1, 'bar' => 2 ) = false
'010' xor (object) array ( 'bar' => 1, 'foo' => 2 ) = false
'010' xor DateTime = false
'010' xor resource = false
'010' xor NULL = true
'10 elephants' xor false = true
'10 elephants' xor true = false
'10 elephants' xor 0 = true
'10 elephants' xor 10 = false
'10 elephants' xor 0.0 = true
'10 elephants' xor 10.0 = false
'10 elephants' xor 3.14 = false
'10 elephants' xor '0' = true
'10 elephants' xor '10' = false
'10 elephants' xor '010' = false
'10 elephants' xor '10 elephants' = false
'10 elephants' xor 'foo' = false
'10 elephants' xor array ( ) = true
'10 elephants' xor array ( 0 => 1 ) = false
'10 elephants' xor array ( 0 => 1, 1 => 100 ) = false
'10 elephants' xor array ( 'foo' => 1, 'bar' => 2 ) = false
'10 elephants' xor array ( 'bar' => 1, 'foo' => 2 ) = false
'10 elephants' xor (object) array ( ) = false
'10 elephants' xor (object) array ( 'foo' => 1, 'bar' => 2 ) = false
'10 elephants' xor (object) array ( 'bar' => 1, 'foo' => 2 ) = false
'10 elephants' xor DateTime = false
'10 elephants' xor resource = false
'10 elephants' xor NULL = true
'foo' xor false = true
'foo' xor true = false
'foo' xor 0 = true
'foo' xor 10 = false
'foo' xor 0.0 = true
'foo' xor 10.0 = false
'foo' xor 3.14 = false
'foo' xor '0' = true
'foo' xor '10' = false
'foo' xor '010' = false
'foo' xor '10 elephants' = false
'foo' xor 'foo' = false
'foo' xor array ( ) = true
'foo' xor array ( 0 => 1 ) = false
'foo' xor array ( 0 => 1, 1 => 100 ) = false
'foo' xor array ( 'foo' => 1, 'bar' => 2 ) = false
'foo' xor array ( 'bar' => 1, 'foo' => 2 ) = false
'foo' xor (object) array ( ) = false
'foo' xor (object) array ( 'foo' => 1, 'bar' => 2 ) = false
'foo' xor (object) array ( 'bar' => 1, 'foo' => 2 ) = false
'foo' xor DateTime = false
'foo' xor resource = false
'foo' xor NULL = true
array ( ) xor false = false
array ( ) xor true = true
array ( ) xor 0 = false
array ( ) xor 10 = true
array ( ) xor 0.0 = false
array ( ) xor 10.0 = true
array ( ) xor 3.14 = true
array ( ) xor '0' = false
array ( ) xor '10' = true
array ( ) xor '010' = true
array ( ) xor '10 elephants' = true
array ( ) xor 'foo' = true
array ( ) xor array ( ) = false
array ( ) xor array ( 0 => 1 ) = true
array ( ) xor array ( 0 => 1, 1 => 100 ) = true
array ( ) xor array ( 'foo' => 1, 'bar' => 2 ) = true
array ( ) xor array ( 'bar' => 1, 'foo' => 2 ) = true
array ( ) xor (object) array ( ) = true
array ( ) xor (object) array ( 'foo' => 1, 'bar' => 2 ) = true
array ( ) xor (object) array ( 'bar' => 1, 'foo' => 2 ) = true
array ( ) xor DateTime = true
array ( ) xor resource = true
array ( ) xor NULL = false
array ( 0 => 1 ) xor false = true
array ( 0 => 1 ) xor true = false
array ( 0 => 1 ) xor 0 = true
array ( 0 => 1 ) xor 10 = false
array ( 0 => 1 ) xor 0.0 = true
array ( 0 => 1 ) xor 10.0 = false
array ( 0 => 1 ) xor 3.14 = false
array ( 0 => 1 ) xor '0' = true
array ( 0 => 1 ) xor '10' = false
array ( 0 => 1 ) xor '010' = false
array ( 0 => 1 ) xor '10 elephants' = false
array ( 0 => 1 ) xor 'foo' = false
array ( 0 => 1 ) xor array ( ) = true
array ( 0 => 1 ) xor array ( 0 => 1 ) = false
array ( 0 => 1 ) xor array ( 0 => 1, 1 => 100 ) = false
array ( 0 => 1 ) xor array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 0 => 1 ) xor array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 0 => 1 ) xor (object) array ( ) = false
array ( 0 => 1 ) xor (object) array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 0 => 1 ) xor (object) array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 0 => 1 ) xor DateTime = false
array ( 0 => 1 ) xor resource = false
array ( 0 => 1 ) xor NULL = true
array ( 0 => 1, 1 => 100 ) xor false = true
array ( 0 => 1, 1 => 100 ) xor true = false
array ( 0 => 1, 1 => 100 ) xor 0 = true
array ( 0 => 1, 1 => 100 ) xor 10 = false
array ( 0 => 1, 1 => 100 ) xor 0.0 = true
array ( 0 => 1, 1 => 100 ) xor 10.0 = false
array ( 0 => 1, 1 => 100 ) xor 3.14 = false
array ( 0 => 1, 1 => 100 ) xor '0' = true
array ( 0 => 1, 1 => 100 ) xor '10' = false
array ( 0 => 1, 1 => 100 ) xor '010' = false
array ( 0 => 1, 1 => 100 ) xor '10 elephants' = false
array ( 0 => 1, 1 => 100 ) xor 'foo' = false
array ( 0 => 1, 1 => 100 ) xor array ( ) = true
array ( 0 => 1, 1 => 100 ) xor array ( 0 => 1 ) = false
array ( 0 => 1, 1 => 100 ) xor array ( 0 => 1, 1 => 100 ) = false
array ( 0 => 1, 1 => 100 ) xor array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 0 => 1, 1 => 100 ) xor array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 0 => 1, 1 => 100 ) xor (object) array ( ) = false
array ( 0 => 1, 1 => 100 ) xor (object) array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 0 => 1, 1 => 100 ) xor (object) array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 0 => 1, 1 => 100 ) xor DateTime = false
array ( 0 => 1, 1 => 100 ) xor resource = false
array ( 0 => 1, 1 => 100 ) xor NULL = true
array ( 'foo' => 1, 'bar' => 2 ) xor false = true
array ( 'foo' => 1, 'bar' => 2 ) xor true = false
array ( 'foo' => 1, 'bar' => 2 ) xor 0 = true
array ( 'foo' => 1, 'bar' => 2 ) xor 10 = false
array ( 'foo' => 1, 'bar' => 2 ) xor 0.0 = true
array ( 'foo' => 1, 'bar' => 2 ) xor 10.0 = false
array ( 'foo' => 1, 'bar' => 2 ) xor 3.14 = false
array ( 'foo' => 1, 'bar' => 2 ) xor '0' = true
array ( 'foo' => 1, 'bar' => 2 ) xor '10' = false
array ( 'foo' => 1, 'bar' => 2 ) xor '010' = false
array ( 'foo' => 1, 'bar' => 2 ) xor '10 elephants' = false
array ( 'foo' => 1, 'bar' => 2 ) xor 'foo' = false
array ( 'foo' => 1, 'bar' => 2 ) xor array ( ) = true
array ( 'foo' => 1, 'bar' => 2 ) xor array ( 0 => 1 ) = false
array ( 'foo' => 1, 'bar' => 2 ) xor array ( 0 => 1, 1 => 100 ) = false
array ( 'foo' => 1, 'bar' => 2 ) xor array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 'foo' => 1, 'bar' => 2 ) xor array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 'foo' => 1, 'bar' => 2 ) xor (object) array ( ) = false
array ( 'foo' => 1, 'bar' => 2 ) xor (object) array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 'foo' => 1, 'bar' => 2 ) xor (object) array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 'foo' => 1, 'bar' => 2 ) xor DateTime = false
array ( 'foo' => 1, 'bar' => 2 ) xor resource = false
array ( 'foo' => 1, 'bar' => 2 ) xor NULL = true
array ( 'bar' => 1, 'foo' => 2 ) xor false = true
array ( 'bar' => 1, 'foo' => 2 ) xor true = false
array ( 'bar' => 1, 'foo' => 2 ) xor 0 = true
array ( 'bar' => 1, 'foo' => 2 ) xor 10 = false
array ( 'bar' => 1, 'foo' => 2 ) xor 0.0 = true
array ( 'bar' => 1, 'foo' => 2 ) xor 10.0 = false
array ( 'bar' => 1, 'foo' => 2 ) xor 3.14 = false
array ( 'bar' => 1, 'foo' => 2 ) xor '0' = true
array ( 'bar' => 1, 'foo' => 2 ) xor '10' = false
array ( 'bar' => 1, 'foo' => 2 ) xor '010' = false
array ( 'bar' => 1, 'foo' => 2 ) xor '10 elephants' = false
array ( 'bar' => 1, 'foo' => 2 ) xor 'foo' = false
array ( 'bar' => 1, 'foo' => 2 ) xor array ( ) = true
array ( 'bar' => 1, 'foo' => 2 ) xor array ( 0 => 1 ) = false
array ( 'bar' => 1, 'foo' => 2 ) xor array ( 0 => 1, 1 => 100 ) = false
array ( 'bar' => 1, 'foo' => 2 ) xor array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 'bar' => 1, 'foo' => 2 ) xor array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 'bar' => 1, 'foo' => 2 ) xor (object) array ( ) = false
array ( 'bar' => 1, 'foo' => 2 ) xor (object) array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 'bar' => 1, 'foo' => 2 ) xor (object) array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 'bar' => 1, 'foo' => 2 ) xor DateTime = false
array ( 'bar' => 1, 'foo' => 2 ) xor resource = false
array ( 'bar' => 1, 'foo' => 2 ) xor NULL = true
(object) array ( ) xor false = true
(object) array ( ) xor true = false
(object) array ( ) xor 0 = true
(object) array ( ) xor 10 = false
(object) array ( ) xor 0.0 = true
(object) array ( ) xor 10.0 = false
(object) array ( ) xor 3.14 = false
(object) array ( ) xor '0' = true
(object) array ( ) xor '10' = false
(object) array ( ) xor '010' = false
(object) array ( ) xor '10 elephants' = false
(object) array ( ) xor 'foo' = false
(object) array ( ) xor array ( ) = true
(object) array ( ) xor array ( 0 => 1 ) = false
(object) array ( ) xor array ( 0 => 1, 1 => 100 ) = false
(object) array ( ) xor array ( 'foo' => 1, 'bar' => 2 ) = false
(object) array ( ) xor array ( 'bar' => 1, 'foo' => 2 ) = false
(object) array ( ) xor (object) array ( ) = false
(object) array ( ) xor (object) array ( 'foo' => 1, 'bar' => 2 ) = false
(object) array ( ) xor (object) array ( 'bar' => 1, 'foo' => 2 ) = false
(object) array ( ) xor DateTime = false
(object) array ( ) xor resource = false
(object) array ( ) xor NULL = true
(object) array ( 'foo' => 1, 'bar' => 2 ) xor false = true
(object) array ( 'foo' => 1, 'bar' => 2 ) xor true = false
(object) array ( 'foo' => 1, 'bar' => 2 ) xor 0 = true
(object) array ( 'foo' => 1, 'bar' => 2 ) xor 10 = false
(object) array ( 'foo' => 1, 'bar' => 2 ) xor 0.0 = true
(object) array ( 'foo' => 1, 'bar' => 2 ) xor 10.0 = false
(object) array ( 'foo' => 1, 'bar' => 2 ) xor 3.14 = false
(object) array ( 'foo' => 1, 'bar' => 2 ) xor '0' = true
(object) array ( 'foo' => 1, 'bar' => 2 ) xor '10' = false
(object) array ( 'foo' => 1, 'bar' => 2 ) xor '010' = false
(object) array ( 'foo' => 1, 'bar' => 2 ) xor '10 elephants' = false
(object) array ( 'foo' => 1, 'bar' => 2 ) xor 'foo' = false
(object) array ( 'foo' => 1, 'bar' => 2 ) xor array ( ) = true
(object) array ( 'foo' => 1, 'bar' => 2 ) xor array ( 0 => 1 ) = false
(object) array ( 'foo' => 1, 'bar' => 2 ) xor array ( 0 => 1, 1 => 100 ) = false
(object) array ( 'foo' => 1, 'bar' => 2 ) xor array ( 'foo' => 1, 'bar' => 2 ) = false
(object) array ( 'foo' => 1, 'bar' => 2 ) xor array ( 'bar' => 1, 'foo' => 2 ) = false
(object) array ( 'foo' => 1, 'bar' => 2 ) xor (object) array ( ) = false
(object) array ( 'foo' => 1, 'bar' => 2 ) xor (object) array ( 'foo' => 1, 'bar' => 2 ) = false
(object) array ( 'foo' => 1, 'bar' => 2 ) xor (object) array ( 'bar' => 1, 'foo' => 2 ) = false
(object) array ( 'foo' => 1, 'bar' => 2 ) xor DateTime = false
(object) array ( 'foo' => 1, 'bar' => 2 ) xor resource = false
(object) array ( 'foo' => 1, 'bar' => 2 ) xor NULL = true
(object) array ( 'bar' => 1, 'foo' => 2 ) xor false = true
(object) array ( 'bar' => 1, 'foo' => 2 ) xor true = false
(object) array ( 'bar' => 1, 'foo' => 2 ) xor 0 = true
(object) array ( 'bar' => 1, 'foo' => 2 ) xor 10 = false
(object) array ( 'bar' => 1, 'foo' => 2 ) xor 0.0 = true
(object) array ( 'bar' => 1, 'foo' => 2 ) xor 10.0 = false
(object) array ( 'bar' => 1, 'foo' => 2 ) xor 3.14 = false
(object) array ( 'bar' => 1, 'foo' => 2 ) xor '0' = true
(object) array ( 'bar' => 1, 'foo' => 2 ) xor '10' = false
(object) array ( 'bar' => 1, 'foo' => 2 ) xor '010' = false
(object) array ( 'bar' => 1, 'foo' => 2 ) xor '10 elephants' = false
(object) array ( 'bar' => 1, 'foo' => 2 ) xor 'foo' = false
(object) array ( 'bar' => 1, 'foo' => 2 ) xor array ( ) = true
(object) array ( 'bar' => 1, 'foo' => 2 ) xor array ( 0 => 1 ) = false
(object) array ( 'bar' => 1, 'foo' => 2 ) xor array ( 0 => 1, 1 => 100 ) = false
(object) array ( 'bar' => 1, 'foo' => 2 ) xor array ( 'foo' => 1, 'bar' => 2 ) = false
(object) array ( 'bar' => 1, 'foo' => 2 ) xor array ( 'bar' => 1, 'foo' => 2 ) = false
(object) array ( 'bar' => 1, 'foo' => 2 ) xor (object) array ( ) = false
(object) array ( 'bar' => 1, 'foo' => 2 ) xor (object) array ( 'foo' => 1, 'bar' => 2 ) = false
(object) array ( 'bar' => 1, 'foo' => 2 ) xor (object) array ( 'bar' => 1, 'foo' => 2 ) = false
(object) array ( 'bar' => 1, 'foo' => 2 ) xor DateTime = false
(object) array ( 'bar' => 1, 'foo' => 2 ) xor resource = false
(object) array ( 'bar' => 1, 'foo' => 2 ) xor NULL = true
DateTime xor false = true
DateTime xor true = false
DateTime xor 0 = true
DateTime xor 10 = false
DateTime xor 0.0 = true
DateTime xor 10.0 = false
DateTime xor 3.14 = false
DateTime xor '0' = true
DateTime xor '10' = false
DateTime xor '010' = false
DateTime xor '10 elephants' = false
DateTime xor 'foo' = false
DateTime xor array ( ) = true
DateTime xor array ( 0 => 1 ) = false
DateTime xor array ( 0 => 1, 1 => 100 ) = false
DateTime xor array ( 'foo' => 1, 'bar' => 2 ) = false
DateTime xor array ( 'bar' => 1, 'foo' => 2 ) = false
DateTime xor (object) array ( ) = false
DateTime xor (object) array ( 'foo' => 1, 'bar' => 2 ) = false
DateTime xor (object) array ( 'bar' => 1, 'foo' => 2 ) = false
DateTime xor DateTime = false
DateTime xor resource = false
DateTime xor NULL = true
resource xor false = true
resource xor true = false
resource xor 0 = true
resource xor 10 = false
resource xor 0.0 = true
resource xor 10.0 = false
resource xor 3.14 = false
resource xor '0' = true
resource xor '10' = false
resource xor '010' = false
resource xor '10 elephants' = false
resource xor 'foo' = false
resource xor array ( ) = true
resource xor array ( 0 => 1 ) = false
resource xor array ( 0 => 1, 1 => 100 ) = false
resource xor array ( 'foo' => 1, 'bar' => 2 ) = false
resource xor array ( 'bar' => 1, 'foo' => 2 ) = false
resource xor (object) array ( ) = false
resource xor (object) array ( 'foo' => 1, 'bar' => 2 ) = false
resource xor (object) array ( 'bar' => 1, 'foo' => 2 ) = false
resource xor DateTime = false
resource xor resource = false
resource xor NULL = true
NULL xor false = false
NULL xor true = true
NULL xor 0 = false
NULL xor 10 = true
NULL xor 0.0 = false
NULL xor 10.0 = true
NULL xor 3.14 = true
NULL xor '0' = false
NULL xor '10' = true
NULL xor '010' = true
NULL xor '10 elephants' = true
NULL xor 'foo' = true
NULL xor array ( ) = false
NULL xor array ( 0 => 1 ) = true
NULL xor array ( 0 => 1, 1 => 100 ) = true
NULL xor array ( 'foo' => 1, 'bar' => 2 ) = true
NULL xor array ( 'bar' => 1, 'foo' => 2 ) = true
NULL xor (object) array ( ) = true
NULL xor (object) array ( 'foo' => 1, 'bar' => 2 ) = true
NULL xor (object) array ( 'bar' => 1, 'foo' => 2 ) = true
NULL xor DateTime = true
NULL xor resource = true
NULL xor NULL = false