--TEST--
greater than '>' operator
--FILE--
<?php
require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a > $b', function($a, $b) { return $a > $b; });

--EXPECT--
false > false = false
false > true = false
false > 0 = false
false > 10 = false
false > 0.0 = false
false > 10.0 = false
false > 3.14 = false
false > '0' = false
false > '10' = false
false > '010' = false
false > '10 elephants' = false
false > 'foo' = false
false > array ( ) = false
false > array ( 0 => 1 ) = false
false > array ( 0 => 1, 1 => 100 ) = false
false > array ( 'foo' => 1, 'bar' => 2 ) = false
false > array ( 'bar' => 1, 'foo' => 2 ) = false
false > (object) array ( ) = false
false > (object) array ( 'foo' => 1, 'bar' => 2 ) = false
false > (object) array ( 'bar' => 1, 'foo' => 2 ) = false
false > DateTime = false
false > resource = false
false > NULL = false
true > false = true
true > true = false
true > 0 = true
true > 10 = false
true > 0.0 = true
true > 10.0 = false
true > 3.14 = false
true > '0' = true
true > '10' = false
true > '010' = false
true > '10 elephants' = false
true > 'foo' = false
true > array ( ) = true
true > array ( 0 => 1 ) = false
true > array ( 0 => 1, 1 => 100 ) = false
true > array ( 'foo' => 1, 'bar' => 2 ) = false
true > array ( 'bar' => 1, 'foo' => 2 ) = false
true > (object) array ( ) = false
true > (object) array ( 'foo' => 1, 'bar' => 2 ) = false
true > (object) array ( 'bar' => 1, 'foo' => 2 ) = false
true > DateTime = false
true > resource = false
true > NULL = true
0 > false = false
0 > true = false
0 > 0 = false
0 > 10 = false
0 > 0.0 = false
0 > 10.0 = false
0 > 3.14 = false
0 > '0' = false
0 > '10' = false
0 > '010' = false
0 > '10 elephants' = false
0 > 'foo' = false
0 > array ( ) = false
0 > array ( 0 => 1 ) = false
0 > array ( 0 => 1, 1 => 100 ) = false
0 > array ( 'foo' => 1, 'bar' => 2 ) = false
0 > array ( 'bar' => 1, 'foo' => 2 ) = false
0 > (object) array ( ) = false - Notice Object of class stdClass could not be converted to int
0 > (object) array ( 'foo' => 1, 'bar' => 2 ) = false - Notice Object of class stdClass could not be converted to int
0 > (object) array ( 'bar' => 1, 'foo' => 2 ) = false - Notice Object of class stdClass could not be converted to int
0 > DateTime = false - Notice Object of class DateTime could not be converted to int
0 > resource = false
0 > NULL = false
10 > false = true
10 > true = false
10 > 0 = true
10 > 10 = false
10 > 0.0 = true
10 > 10.0 = false
10 > 3.14 = true
10 > '0' = true
10 > '10' = false
10 > '010' = false
10 > '10 elephants' = false
10 > 'foo' = true
10 > array ( ) = false
10 > array ( 0 => 1 ) = false
10 > array ( 0 => 1, 1 => 100 ) = false
10 > array ( 'foo' => 1, 'bar' => 2 ) = false
10 > array ( 'bar' => 1, 'foo' => 2 ) = false
10 > (object) array ( ) = true - Notice Object of class stdClass could not be converted to int
10 > (object) array ( 'foo' => 1, 'bar' => 2 ) = true - Notice Object of class stdClass could not be converted to int
10 > (object) array ( 'bar' => 1, 'foo' => 2 ) = true - Notice Object of class stdClass could not be converted to int
10 > DateTime = true - Notice Object of class DateTime could not be converted to int
10 > resource = true
10 > NULL = true
0.0 > false = false
0.0 > true = false
0.0 > 0 = false
0.0 > 10 = false
0.0 > 0.0 = false
0.0 > 10.0 = false
0.0 > 3.14 = false
0.0 > '0' = false
0.0 > '10' = false
0.0 > '010' = false
0.0 > '10 elephants' = false
0.0 > 'foo' = false
0.0 > array ( ) = false
0.0 > array ( 0 => 1 ) = false
0.0 > array ( 0 => 1, 1 => 100 ) = false
0.0 > array ( 'foo' => 1, 'bar' => 2 ) = false
0.0 > array ( 'bar' => 1, 'foo' => 2 ) = false
0.0 > (object) array ( ) = false - Notice Object of class stdClass could not be converted to float
0.0 > (object) array ( 'foo' => 1, 'bar' => 2 ) = false - Notice Object of class stdClass could not be converted to float
0.0 > (object) array ( 'bar' => 1, 'foo' => 2 ) = false - Notice Object of class stdClass could not be converted to float
0.0 > DateTime = false - Notice Object of class DateTime could not be converted to float
0.0 > resource = false
0.0 > NULL = false
10.0 > false = true
10.0 > true = false
10.0 > 0 = true
10.0 > 10 = false
10.0 > 0.0 = true
10.0 > 10.0 = false
10.0 > 3.14 = true
10.0 > '0' = true
10.0 > '10' = false
10.0 > '010' = false
10.0 > '10 elephants' = false
10.0 > 'foo' = true
10.0 > array ( ) = false
10.0 > array ( 0 => 1 ) = false
10.0 > array ( 0 => 1, 1 => 100 ) = false
10.0 > array ( 'foo' => 1, 'bar' => 2 ) = false
10.0 > array ( 'bar' => 1, 'foo' => 2 ) = false
10.0 > (object) array ( ) = true - Notice Object of class stdClass could not be converted to float
10.0 > (object) array ( 'foo' => 1, 'bar' => 2 ) = true - Notice Object of class stdClass could not be converted to float
10.0 > (object) array ( 'bar' => 1, 'foo' => 2 ) = true - Notice Object of class stdClass could not be converted to float
10.0 > DateTime = true - Notice Object of class DateTime could not be converted to float
10.0 > resource = true
10.0 > NULL = true
3.14 > false = true
3.14 > true = false
3.14 > 0 = true
3.14 > 10 = false
3.14 > 0.0 = true
3.14 > 10.0 = false
3.14 > 3.14 = false
3.14 > '0' = true
3.14 > '10' = false
3.14 > '010' = false
3.14 > '10 elephants' = false
3.14 > 'foo' = true
3.14 > array ( ) = false
3.14 > array ( 0 => 1 ) = false
3.14 > array ( 0 => 1, 1 => 100 ) = false
3.14 > array ( 'foo' => 1, 'bar' => 2 ) = false
3.14 > array ( 'bar' => 1, 'foo' => 2 ) = false
3.14 > (object) array ( ) = true - Notice Object of class stdClass could not be converted to float
3.14 > (object) array ( 'foo' => 1, 'bar' => 2 ) = true - Notice Object of class stdClass could not be converted to float
3.14 > (object) array ( 'bar' => 1, 'foo' => 2 ) = true - Notice Object of class stdClass could not be converted to float
3.14 > DateTime = true - Notice Object of class DateTime could not be converted to float
3.14 > resource = false
3.14 > NULL = true
'0' > false = false
'0' > true = false
'0' > 0 = false
'0' > 10 = false
'0' > 0.0 = false
'0' > 10.0 = false
'0' > 3.14 = false
'0' > '0' = false
'0' > '10' = false
'0' > '010' = false
'0' > '10 elephants' = false
'0' > 'foo' = false
'0' > array ( ) = false
'0' > array ( 0 => 1 ) = false
'0' > array ( 0 => 1, 1 => 100 ) = false
'0' > array ( 'foo' => 1, 'bar' => 2 ) = false
'0' > array ( 'bar' => 1, 'foo' => 2 ) = false
'0' > (object) array ( ) = false
'0' > (object) array ( 'foo' => 1, 'bar' => 2 ) = false
'0' > (object) array ( 'bar' => 1, 'foo' => 2 ) = false
'0' > DateTime = false
'0' > resource = false
'0' > NULL = true
'10' > false = true
'10' > true = false
'10' > 0 = true
'10' > 10 = false
'10' > 0.0 = true
'10' > 10.0 = false
'10' > 3.14 = true
'10' > '0' = true
'10' > '10' = false
'10' > '010' = false
'10' > '10 elephants' = false
'10' > 'foo' = false
'10' > array ( ) = false
'10' > array ( 0 => 1 ) = false
'10' > array ( 0 => 1, 1 => 100 ) = false
'10' > array ( 'foo' => 1, 'bar' => 2 ) = false
'10' > array ( 'bar' => 1, 'foo' => 2 ) = false
'10' > (object) array ( ) = false
'10' > (object) array ( 'foo' => 1, 'bar' => 2 ) = false
'10' > (object) array ( 'bar' => 1, 'foo' => 2 ) = false
'10' > DateTime = false
'10' > resource = true
'10' > NULL = true
'010' > false = true
'010' > true = false
'010' > 0 = true
'010' > 10 = false
'010' > 0.0 = true
'010' > 10.0 = false
'010' > 3.14 = true
'010' > '0' = true
'010' > '10' = false
'010' > '010' = false
'010' > '10 elephants' = false
'010' > 'foo' = false
'010' > array ( ) = false
'010' > array ( 0 => 1 ) = false
'010' > array ( 0 => 1, 1 => 100 ) = false
'010' > array ( 'foo' => 1, 'bar' => 2 ) = false
'010' > array ( 'bar' => 1, 'foo' => 2 ) = false
'010' > (object) array ( ) = false
'010' > (object) array ( 'foo' => 1, 'bar' => 2 ) = false
'010' > (object) array ( 'bar' => 1, 'foo' => 2 ) = false
'010' > DateTime = false
'010' > resource = true
'010' > NULL = true
'10 elephants' > false = true
'10 elephants' > true = false
'10 elephants' > 0 = true
'10 elephants' > 10 = false
'10 elephants' > 0.0 = true
'10 elephants' > 10.0 = false
'10 elephants' > 3.14 = true
'10 elephants' > '0' = true
'10 elephants' > '10' = true
'10 elephants' > '010' = true
'10 elephants' > '10 elephants' = false
'10 elephants' > 'foo' = false
'10 elephants' > array ( ) = false
'10 elephants' > array ( 0 => 1 ) = false
'10 elephants' > array ( 0 => 1, 1 => 100 ) = false
'10 elephants' > array ( 'foo' => 1, 'bar' => 2 ) = false
'10 elephants' > array ( 'bar' => 1, 'foo' => 2 ) = false
'10 elephants' > (object) array ( ) = false
'10 elephants' > (object) array ( 'foo' => 1, 'bar' => 2 ) = false
'10 elephants' > (object) array ( 'bar' => 1, 'foo' => 2 ) = false
'10 elephants' > DateTime = false
'10 elephants' > resource = true
'10 elephants' > NULL = true
'foo' > false = true
'foo' > true = false
'foo' > 0 = false
'foo' > 10 = false
'foo' > 0.0 = false
'foo' > 10.0 = false
'foo' > 3.14 = false
'foo' > '0' = true
'foo' > '10' = true
'foo' > '010' = true
'foo' > '10 elephants' = true
'foo' > 'foo' = false
'foo' > array ( ) = false
'foo' > array ( 0 => 1 ) = false
'foo' > array ( 0 => 1, 1 => 100 ) = false
'foo' > array ( 'foo' => 1, 'bar' => 2 ) = false
'foo' > array ( 'bar' => 1, 'foo' => 2 ) = false
'foo' > (object) array ( ) = false
'foo' > (object) array ( 'foo' => 1, 'bar' => 2 ) = false
'foo' > (object) array ( 'bar' => 1, 'foo' => 2 ) = false
'foo' > DateTime = false
'foo' > resource = false
'foo' > NULL = true
array ( ) > false = false
array ( ) > true = false
array ( ) > 0 = true
array ( ) > 10 = true
array ( ) > 0.0 = true
array ( ) > 10.0 = true
array ( ) > 3.14 = true
array ( ) > '0' = true
array ( ) > '10' = true
array ( ) > '010' = true
array ( ) > '10 elephants' = true
array ( ) > 'foo' = true
array ( ) > array ( ) = false
array ( ) > array ( 0 => 1 ) = false
array ( ) > array ( 0 => 1, 1 => 100 ) = false
array ( ) > array ( 'foo' => 1, 'bar' => 2 ) = false
array ( ) > array ( 'bar' => 1, 'foo' => 2 ) = false
array ( ) > (object) array ( ) = false
array ( ) > (object) array ( 'foo' => 1, 'bar' => 2 ) = false
array ( ) > (object) array ( 'bar' => 1, 'foo' => 2 ) = false
array ( ) > DateTime = false
array ( ) > resource = true
array ( ) > NULL = false
array ( 0 => 1 ) > false = true
array ( 0 => 1 ) > true = false
array ( 0 => 1 ) > 0 = true
array ( 0 => 1 ) > 10 = true
array ( 0 => 1 ) > 0.0 = true
array ( 0 => 1 ) > 10.0 = true
array ( 0 => 1 ) > 3.14 = true
array ( 0 => 1 ) > '0' = true
array ( 0 => 1 ) > '10' = true
array ( 0 => 1 ) > '010' = true
array ( 0 => 1 ) > '10 elephants' = true
array ( 0 => 1 ) > 'foo' = true
array ( 0 => 1 ) > array ( ) = true
array ( 0 => 1 ) > array ( 0 => 1 ) = false
array ( 0 => 1 ) > array ( 0 => 1, 1 => 100 ) = false
array ( 0 => 1 ) > array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 0 => 1 ) > array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 0 => 1 ) > (object) array ( ) = false
array ( 0 => 1 ) > (object) array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 0 => 1 ) > (object) array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 0 => 1 ) > DateTime = false
array ( 0 => 1 ) > resource = true
array ( 0 => 1 ) > NULL = true
array ( 0 => 1, 1 => 100 ) > false = true
array ( 0 => 1, 1 => 100 ) > true = false
array ( 0 => 1, 1 => 100 ) > 0 = true
array ( 0 => 1, 1 => 100 ) > 10 = true
array ( 0 => 1, 1 => 100 ) > 0.0 = true
array ( 0 => 1, 1 => 100 ) > 10.0 = true
array ( 0 => 1, 1 => 100 ) > 3.14 = true
array ( 0 => 1, 1 => 100 ) > '0' = true
array ( 0 => 1, 1 => 100 ) > '10' = true
array ( 0 => 1, 1 => 100 ) > '010' = true
array ( 0 => 1, 1 => 100 ) > '10 elephants' = true
array ( 0 => 1, 1 => 100 ) > 'foo' = true
array ( 0 => 1, 1 => 100 ) > array ( ) = true
array ( 0 => 1, 1 => 100 ) > array ( 0 => 1 ) = true
array ( 0 => 1, 1 => 100 ) > array ( 0 => 1, 1 => 100 ) = false
array ( 0 => 1, 1 => 100 ) > array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 0 => 1, 1 => 100 ) > array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 0 => 1, 1 => 100 ) > (object) array ( ) = false
array ( 0 => 1, 1 => 100 ) > (object) array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 0 => 1, 1 => 100 ) > (object) array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 0 => 1, 1 => 100 ) > DateTime = false
array ( 0 => 1, 1 => 100 ) > resource = true
array ( 0 => 1, 1 => 100 ) > NULL = true
array ( 'foo' => 1, 'bar' => 2 ) > false = true
array ( 'foo' => 1, 'bar' => 2 ) > true = false
array ( 'foo' => 1, 'bar' => 2 ) > 0 = true
array ( 'foo' => 1, 'bar' => 2 ) > 10 = true
array ( 'foo' => 1, 'bar' => 2 ) > 0.0 = true
array ( 'foo' => 1, 'bar' => 2 ) > 10.0 = true
array ( 'foo' => 1, 'bar' => 2 ) > 3.14 = true
array ( 'foo' => 1, 'bar' => 2 ) > '0' = true
array ( 'foo' => 1, 'bar' => 2 ) > '10' = true
array ( 'foo' => 1, 'bar' => 2 ) > '010' = true
array ( 'foo' => 1, 'bar' => 2 ) > '10 elephants' = true
array ( 'foo' => 1, 'bar' => 2 ) > 'foo' = true
array ( 'foo' => 1, 'bar' => 2 ) > array ( ) = true
array ( 'foo' => 1, 'bar' => 2 ) > array ( 0 => 1 ) = true
array ( 'foo' => 1, 'bar' => 2 ) > array ( 0 => 1, 1 => 100 ) = false
array ( 'foo' => 1, 'bar' => 2 ) > array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 'foo' => 1, 'bar' => 2 ) > array ( 'bar' => 1, 'foo' => 2 ) = true
array ( 'foo' => 1, 'bar' => 2 ) > (object) array ( ) = false
array ( 'foo' => 1, 'bar' => 2 ) > (object) array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 'foo' => 1, 'bar' => 2 ) > (object) array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 'foo' => 1, 'bar' => 2 ) > DateTime = false
array ( 'foo' => 1, 'bar' => 2 ) > resource = true
array ( 'foo' => 1, 'bar' => 2 ) > NULL = true
array ( 'bar' => 1, 'foo' => 2 ) > false = true
array ( 'bar' => 1, 'foo' => 2 ) > true = false
array ( 'bar' => 1, 'foo' => 2 ) > 0 = true
array ( 'bar' => 1, 'foo' => 2 ) > 10 = true
array ( 'bar' => 1, 'foo' => 2 ) > 0.0 = true
array ( 'bar' => 1, 'foo' => 2 ) > 10.0 = true
array ( 'bar' => 1, 'foo' => 2 ) > 3.14 = true
array ( 'bar' => 1, 'foo' => 2 ) > '0' = true
array ( 'bar' => 1, 'foo' => 2 ) > '10' = true
array ( 'bar' => 1, 'foo' => 2 ) > '010' = true
array ( 'bar' => 1, 'foo' => 2 ) > '10 elephants' = true
array ( 'bar' => 1, 'foo' => 2 ) > 'foo' = true
array ( 'bar' => 1, 'foo' => 2 ) > array ( ) = true
array ( 'bar' => 1, 'foo' => 2 ) > array ( 0 => 1 ) = true
array ( 'bar' => 1, 'foo' => 2 ) > array ( 0 => 1, 1 => 100 ) = false
array ( 'bar' => 1, 'foo' => 2 ) > array ( 'foo' => 1, 'bar' => 2 ) = true
array ( 'bar' => 1, 'foo' => 2 ) > array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 'bar' => 1, 'foo' => 2 ) > (object) array ( ) = false
array ( 'bar' => 1, 'foo' => 2 ) > (object) array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 'bar' => 1, 'foo' => 2 ) > (object) array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 'bar' => 1, 'foo' => 2 ) > DateTime = false
array ( 'bar' => 1, 'foo' => 2 ) > resource = true
array ( 'bar' => 1, 'foo' => 2 ) > NULL = true
(object) array ( ) > false = true
(object) array ( ) > true = false
(object) array ( ) > 0 = true - Notice Object of class stdClass could not be converted to int
(object) array ( ) > 10 = false - Notice Object of class stdClass could not be converted to int
(object) array ( ) > 0.0 = true - Notice Object of class stdClass could not be converted to float
(object) array ( ) > 10.0 = false - Notice Object of class stdClass could not be converted to float
(object) array ( ) > 3.14 = false - Notice Object of class stdClass could not be converted to float
(object) array ( ) > '0' = true
(object) array ( ) > '10' = true
(object) array ( ) > '010' = true
(object) array ( ) > '10 elephants' = true
(object) array ( ) > 'foo' = true
(object) array ( ) > array ( ) = true
(object) array ( ) > array ( 0 => 1 ) = true
(object) array ( ) > array ( 0 => 1, 1 => 100 ) = true
(object) array ( ) > array ( 'foo' => 1, 'bar' => 2 ) = true
(object) array ( ) > array ( 'bar' => 1, 'foo' => 2 ) = true
(object) array ( ) > (object) array ( ) = false
(object) array ( ) > (object) array ( 'foo' => 1, 'bar' => 2 ) = false
(object) array ( ) > (object) array ( 'bar' => 1, 'foo' => 2 ) = false
(object) array ( ) > DateTime = false
(object) array ( ) > resource = true
(object) array ( ) > NULL = true
(object) array ( 'foo' => 1, 'bar' => 2 ) > false = true
(object) array ( 'foo' => 1, 'bar' => 2 ) > true = false
(object) array ( 'foo' => 1, 'bar' => 2 ) > 0 = true - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) > 10 = false - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) > 0.0 = true - Notice Object of class stdClass could not be converted to float
(object) array ( 'foo' => 1, 'bar' => 2 ) > 10.0 = false - Notice Object of class stdClass could not be converted to float
(object) array ( 'foo' => 1, 'bar' => 2 ) > 3.14 = false - Notice Object of class stdClass could not be converted to float
(object) array ( 'foo' => 1, 'bar' => 2 ) > '0' = true
(object) array ( 'foo' => 1, 'bar' => 2 ) > '10' = true
(object) array ( 'foo' => 1, 'bar' => 2 ) > '010' = true
(object) array ( 'foo' => 1, 'bar' => 2 ) > '10 elephants' = true
(object) array ( 'foo' => 1, 'bar' => 2 ) > 'foo' = true
(object) array ( 'foo' => 1, 'bar' => 2 ) > array ( ) = true
(object) array ( 'foo' => 1, 'bar' => 2 ) > array ( 0 => 1 ) = true
(object) array ( 'foo' => 1, 'bar' => 2 ) > array ( 0 => 1, 1 => 100 ) = true
(object) array ( 'foo' => 1, 'bar' => 2 ) > array ( 'foo' => 1, 'bar' => 2 ) = true
(object) array ( 'foo' => 1, 'bar' => 2 ) > array ( 'bar' => 1, 'foo' => 2 ) = true
(object) array ( 'foo' => 1, 'bar' => 2 ) > (object) array ( ) = true
(object) array ( 'foo' => 1, 'bar' => 2 ) > (object) array ( 'foo' => 1, 'bar' => 2 ) = false
(object) array ( 'foo' => 1, 'bar' => 2 ) > (object) array ( 'bar' => 1, 'foo' => 2 ) = true
(object) array ( 'foo' => 1, 'bar' => 2 ) > DateTime = false
(object) array ( 'foo' => 1, 'bar' => 2 ) > resource = true
(object) array ( 'foo' => 1, 'bar' => 2 ) > NULL = true
(object) array ( 'bar' => 1, 'foo' => 2 ) > false = true
(object) array ( 'bar' => 1, 'foo' => 2 ) > true = false
(object) array ( 'bar' => 1, 'foo' => 2 ) > 0 = true - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) > 10 = false - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) > 0.0 = true - Notice Object of class stdClass could not be converted to float
(object) array ( 'bar' => 1, 'foo' => 2 ) > 10.0 = false - Notice Object of class stdClass could not be converted to float
(object) array ( 'bar' => 1, 'foo' => 2 ) > 3.14 = false - Notice Object of class stdClass could not be converted to float
(object) array ( 'bar' => 1, 'foo' => 2 ) > '0' = true
(object) array ( 'bar' => 1, 'foo' => 2 ) > '10' = true
(object) array ( 'bar' => 1, 'foo' => 2 ) > '010' = true
(object) array ( 'bar' => 1, 'foo' => 2 ) > '10 elephants' = true
(object) array ( 'bar' => 1, 'foo' => 2 ) > 'foo' = true
(object) array ( 'bar' => 1, 'foo' => 2 ) > array ( ) = true
(object) array ( 'bar' => 1, 'foo' => 2 ) > array ( 0 => 1 ) = true
(object) array ( 'bar' => 1, 'foo' => 2 ) > array ( 0 => 1, 1 => 100 ) = true
(object) array ( 'bar' => 1, 'foo' => 2 ) > array ( 'foo' => 1, 'bar' => 2 ) = true
(object) array ( 'bar' => 1, 'foo' => 2 ) > array ( 'bar' => 1, 'foo' => 2 ) = true
(object) array ( 'bar' => 1, 'foo' => 2 ) > (object) array ( ) = true
(object) array ( 'bar' => 1, 'foo' => 2 ) > (object) array ( 'foo' => 1, 'bar' => 2 ) = true
(object) array ( 'bar' => 1, 'foo' => 2 ) > (object) array ( 'bar' => 1, 'foo' => 2 ) = false
(object) array ( 'bar' => 1, 'foo' => 2 ) > DateTime = false
(object) array ( 'bar' => 1, 'foo' => 2 ) > resource = true
(object) array ( 'bar' => 1, 'foo' => 2 ) > NULL = true
DateTime > false = true
DateTime > true = false
DateTime > 0 = true - Notice Object of class DateTime could not be converted to int
DateTime > 10 = false - Notice Object of class DateTime could not be converted to int
DateTime > 0.0 = true - Notice Object of class DateTime could not be converted to float
DateTime > 10.0 = false - Notice Object of class DateTime could not be converted to float
DateTime > 3.14 = false - Notice Object of class DateTime could not be converted to float
DateTime > '0' = true
DateTime > '10' = true
DateTime > '010' = true
DateTime > '10 elephants' = true
DateTime > 'foo' = true
DateTime > array ( ) = true
DateTime > array ( 0 => 1 ) = true
DateTime > array ( 0 => 1, 1 => 100 ) = true
DateTime > array ( 'foo' => 1, 'bar' => 2 ) = true
DateTime > array ( 'bar' => 1, 'foo' => 2 ) = true
DateTime > (object) array ( ) = false
DateTime > (object) array ( 'foo' => 1, 'bar' => 2 ) = false
DateTime > (object) array ( 'bar' => 1, 'foo' => 2 ) = false
DateTime > DateTime = false
DateTime > resource = true
DateTime > NULL = true
resource > false = true
resource > true = false
resource > 0 = true
resource > 10 = false
resource > 0.0 = true
resource > 10.0 = false
resource > 3.14 = true
resource > '0' = true
resource > '10' = false
resource > '010' = false
resource > '10 elephants' = false
resource > 'foo' = true
resource > array ( ) = false
resource > array ( 0 => 1 ) = false
resource > array ( 0 => 1, 1 => 100 ) = false
resource > array ( 'foo' => 1, 'bar' => 2 ) = false
resource > array ( 'bar' => 1, 'foo' => 2 ) = false
resource > (object) array ( ) = false
resource > (object) array ( 'foo' => 1, 'bar' => 2 ) = false
resource > (object) array ( 'bar' => 1, 'foo' => 2 ) = false
resource > DateTime = false
resource > resource = false
resource > NULL = true
NULL > false = false
NULL > true = false
NULL > 0 = false
NULL > 10 = false
NULL > 0.0 = false
NULL > 10.0 = false
NULL > 3.14 = false
NULL > '0' = false
NULL > '10' = false
NULL > '010' = false
NULL > '10 elephants' = false
NULL > 'foo' = false
NULL > array ( ) = false
NULL > array ( 0 => 1 ) = false
NULL > array ( 0 => 1, 1 => 100 ) = false
NULL > array ( 'foo' => 1, 'bar' => 2 ) = false
NULL > array ( 'bar' => 1, 'foo' => 2 ) = false
NULL > (object) array ( ) = false
NULL > (object) array ( 'foo' => 1, 'bar' => 2 ) = false
NULL > (object) array ( 'bar' => 1, 'foo' => 2 ) = false
NULL > DateTime = false
NULL > resource = false
NULL > NULL = false