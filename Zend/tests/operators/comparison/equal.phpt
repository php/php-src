--TEST--
equal '==' operator
--FILE--
<?php
require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

$fn = function($a, $b) { return $a == $b; };

two_operands('$a == $b', $fn, ['foo' => 0, 'bar' => 2], ['bar' => 2, 'foo' => 0]);
two_operands('$a == $b', $fn, ['foo' => 0, 'bar' => 2], ['bar' => 2, 'foo' => null]);
two_operands('$a == $b', $fn, ['foo' => 0, 'bar' => 2], ['bar' => '2', 'foo' => 0]);
echo "\n";

test_two_operands('$a == $b', $fn);

--EXPECT--
array ( 'foo' => 0, 'bar' => 2 ) == array ( 'bar' => 2, 'foo' => 0 ) = true
array ( 'foo' => 0, 'bar' => 2 ) == array ( 'bar' => 2, 'foo' => NULL ) = true
array ( 'foo' => 0, 'bar' => 2 ) == array ( 'bar' => '2', 'foo' => 0 ) = true

false == false = true
false == true = false
false == 0 = true
false == 10 = false
false == 0.0 = true
false == 10.0 = false
false == 3.14 = false
false == '0' = true
false == '10' = false
false == '010' = false
false == '10 elephants' = false
false == 'foo' = false
false == array ( ) = true
false == array ( 0 => 1 ) = false
false == array ( 0 => 1, 1 => 100 ) = false
false == array ( 'foo' => 1, 'bar' => 2 ) = false
false == array ( 'bar' => 1, 'foo' => 2 ) = false
false == (object) array ( ) = false
false == (object) array ( 'foo' => 1, 'bar' => 2 ) = false
false == (object) array ( 'bar' => 1, 'foo' => 2 ) = false
false == DateTime = false
false == resource = false
false == NULL = true
true == false = false
true == true = true
true == 0 = false
true == 10 = true
true == 0.0 = false
true == 10.0 = true
true == 3.14 = true
true == '0' = false
true == '10' = true
true == '010' = true
true == '10 elephants' = true
true == 'foo' = true
true == array ( ) = false
true == array ( 0 => 1 ) = true
true == array ( 0 => 1, 1 => 100 ) = true
true == array ( 'foo' => 1, 'bar' => 2 ) = true
true == array ( 'bar' => 1, 'foo' => 2 ) = true
true == (object) array ( ) = true
true == (object) array ( 'foo' => 1, 'bar' => 2 ) = true
true == (object) array ( 'bar' => 1, 'foo' => 2 ) = true
true == DateTime = true
true == resource = true
true == NULL = false
0 == false = true
0 == true = false
0 == 0 = true
0 == 10 = false
0 == 0.0 = true
0 == 10.0 = false
0 == 3.14 = false
0 == '0' = true
0 == '10' = false
0 == '010' = false
0 == '10 elephants' = false
0 == 'foo' = true
0 == array ( ) = false
0 == array ( 0 => 1 ) = false
0 == array ( 0 => 1, 1 => 100 ) = false
0 == array ( 'foo' => 1, 'bar' => 2 ) = false
0 == array ( 'bar' => 1, 'foo' => 2 ) = false
0 == (object) array ( ) = false - Notice Object of class stdClass could not be converted to int
0 == (object) array ( 'foo' => 1, 'bar' => 2 ) = false - Notice Object of class stdClass could not be converted to int
0 == (object) array ( 'bar' => 1, 'foo' => 2 ) = false - Notice Object of class stdClass could not be converted to int
0 == DateTime = false - Notice Object of class DateTime could not be converted to int
0 == resource = false
0 == NULL = true
10 == false = false
10 == true = true
10 == 0 = false
10 == 10 = true
10 == 0.0 = false
10 == 10.0 = true
10 == 3.14 = false
10 == '0' = false
10 == '10' = true
10 == '010' = true
10 == '10 elephants' = true
10 == 'foo' = false
10 == array ( ) = false
10 == array ( 0 => 1 ) = false
10 == array ( 0 => 1, 1 => 100 ) = false
10 == array ( 'foo' => 1, 'bar' => 2 ) = false
10 == array ( 'bar' => 1, 'foo' => 2 ) = false
10 == (object) array ( ) = false - Notice Object of class stdClass could not be converted to int
10 == (object) array ( 'foo' => 1, 'bar' => 2 ) = false - Notice Object of class stdClass could not be converted to int
10 == (object) array ( 'bar' => 1, 'foo' => 2 ) = false - Notice Object of class stdClass could not be converted to int
10 == DateTime = false - Notice Object of class DateTime could not be converted to int
10 == resource = false
10 == NULL = false
0.0 == false = true
0.0 == true = false
0.0 == 0 = true
0.0 == 10 = false
0.0 == 0.0 = true
0.0 == 10.0 = false
0.0 == 3.14 = false
0.0 == '0' = true
0.0 == '10' = false
0.0 == '010' = false
0.0 == '10 elephants' = false
0.0 == 'foo' = true
0.0 == array ( ) = false
0.0 == array ( 0 => 1 ) = false
0.0 == array ( 0 => 1, 1 => 100 ) = false
0.0 == array ( 'foo' => 1, 'bar' => 2 ) = false
0.0 == array ( 'bar' => 1, 'foo' => 2 ) = false
0.0 == (object) array ( ) = false - Notice Object of class stdClass could not be converted to float
0.0 == (object) array ( 'foo' => 1, 'bar' => 2 ) = false - Notice Object of class stdClass could not be converted to float
0.0 == (object) array ( 'bar' => 1, 'foo' => 2 ) = false - Notice Object of class stdClass could not be converted to float
0.0 == DateTime = false - Notice Object of class DateTime could not be converted to float
0.0 == resource = false
0.0 == NULL = true
10.0 == false = false
10.0 == true = true
10.0 == 0 = false
10.0 == 10 = true
10.0 == 0.0 = false
10.0 == 10.0 = true
10.0 == 3.14 = false
10.0 == '0' = false
10.0 == '10' = true
10.0 == '010' = true
10.0 == '10 elephants' = true
10.0 == 'foo' = false
10.0 == array ( ) = false
10.0 == array ( 0 => 1 ) = false
10.0 == array ( 0 => 1, 1 => 100 ) = false
10.0 == array ( 'foo' => 1, 'bar' => 2 ) = false
10.0 == array ( 'bar' => 1, 'foo' => 2 ) = false
10.0 == (object) array ( ) = false - Notice Object of class stdClass could not be converted to float
10.0 == (object) array ( 'foo' => 1, 'bar' => 2 ) = false - Notice Object of class stdClass could not be converted to float
10.0 == (object) array ( 'bar' => 1, 'foo' => 2 ) = false - Notice Object of class stdClass could not be converted to float
10.0 == DateTime = false - Notice Object of class DateTime could not be converted to float
10.0 == resource = false
10.0 == NULL = false
3.14 == false = false
3.14 == true = true
3.14 == 0 = false
3.14 == 10 = false
3.14 == 0.0 = false
3.14 == 10.0 = false
3.14 == 3.14 = true
3.14 == '0' = false
3.14 == '10' = false
3.14 == '010' = false
3.14 == '10 elephants' = false
3.14 == 'foo' = false
3.14 == array ( ) = false
3.14 == array ( 0 => 1 ) = false
3.14 == array ( 0 => 1, 1 => 100 ) = false
3.14 == array ( 'foo' => 1, 'bar' => 2 ) = false
3.14 == array ( 'bar' => 1, 'foo' => 2 ) = false
3.14 == (object) array ( ) = false - Notice Object of class stdClass could not be converted to float
3.14 == (object) array ( 'foo' => 1, 'bar' => 2 ) = false - Notice Object of class stdClass could not be converted to float
3.14 == (object) array ( 'bar' => 1, 'foo' => 2 ) = false - Notice Object of class stdClass could not be converted to float
3.14 == DateTime = false - Notice Object of class DateTime could not be converted to float
3.14 == resource = false
3.14 == NULL = false
'0' == false = true
'0' == true = false
'0' == 0 = true
'0' == 10 = false
'0' == 0.0 = true
'0' == 10.0 = false
'0' == 3.14 = false
'0' == '0' = true
'0' == '10' = false
'0' == '010' = false
'0' == '10 elephants' = false
'0' == 'foo' = false
'0' == array ( ) = false
'0' == array ( 0 => 1 ) = false
'0' == array ( 0 => 1, 1 => 100 ) = false
'0' == array ( 'foo' => 1, 'bar' => 2 ) = false
'0' == array ( 'bar' => 1, 'foo' => 2 ) = false
'0' == (object) array ( ) = false
'0' == (object) array ( 'foo' => 1, 'bar' => 2 ) = false
'0' == (object) array ( 'bar' => 1, 'foo' => 2 ) = false
'0' == DateTime = false
'0' == resource = false
'0' == NULL = false
'10' == false = false
'10' == true = true
'10' == 0 = false
'10' == 10 = true
'10' == 0.0 = false
'10' == 10.0 = true
'10' == 3.14 = false
'10' == '0' = false
'10' == '10' = true
'10' == '010' = true
'10' == '10 elephants' = false
'10' == 'foo' = false
'10' == array ( ) = false
'10' == array ( 0 => 1 ) = false
'10' == array ( 0 => 1, 1 => 100 ) = false
'10' == array ( 'foo' => 1, 'bar' => 2 ) = false
'10' == array ( 'bar' => 1, 'foo' => 2 ) = false
'10' == (object) array ( ) = false
'10' == (object) array ( 'foo' => 1, 'bar' => 2 ) = false
'10' == (object) array ( 'bar' => 1, 'foo' => 2 ) = false
'10' == DateTime = false
'10' == resource = false
'10' == NULL = false
'010' == false = false
'010' == true = true
'010' == 0 = false
'010' == 10 = true
'010' == 0.0 = false
'010' == 10.0 = true
'010' == 3.14 = false
'010' == '0' = false
'010' == '10' = true
'010' == '010' = true
'010' == '10 elephants' = false
'010' == 'foo' = false
'010' == array ( ) = false
'010' == array ( 0 => 1 ) = false
'010' == array ( 0 => 1, 1 => 100 ) = false
'010' == array ( 'foo' => 1, 'bar' => 2 ) = false
'010' == array ( 'bar' => 1, 'foo' => 2 ) = false
'010' == (object) array ( ) = false
'010' == (object) array ( 'foo' => 1, 'bar' => 2 ) = false
'010' == (object) array ( 'bar' => 1, 'foo' => 2 ) = false
'010' == DateTime = false
'010' == resource = false
'010' == NULL = false
'10 elephants' == false = false
'10 elephants' == true = true
'10 elephants' == 0 = false
'10 elephants' == 10 = true
'10 elephants' == 0.0 = false
'10 elephants' == 10.0 = true
'10 elephants' == 3.14 = false
'10 elephants' == '0' = false
'10 elephants' == '10' = false
'10 elephants' == '010' = false
'10 elephants' == '10 elephants' = true
'10 elephants' == 'foo' = false
'10 elephants' == array ( ) = false
'10 elephants' == array ( 0 => 1 ) = false
'10 elephants' == array ( 0 => 1, 1 => 100 ) = false
'10 elephants' == array ( 'foo' => 1, 'bar' => 2 ) = false
'10 elephants' == array ( 'bar' => 1, 'foo' => 2 ) = false
'10 elephants' == (object) array ( ) = false
'10 elephants' == (object) array ( 'foo' => 1, 'bar' => 2 ) = false
'10 elephants' == (object) array ( 'bar' => 1, 'foo' => 2 ) = false
'10 elephants' == DateTime = false
'10 elephants' == resource = false
'10 elephants' == NULL = false
'foo' == false = false
'foo' == true = true
'foo' == 0 = true
'foo' == 10 = false
'foo' == 0.0 = true
'foo' == 10.0 = false
'foo' == 3.14 = false
'foo' == '0' = false
'foo' == '10' = false
'foo' == '010' = false
'foo' == '10 elephants' = false
'foo' == 'foo' = true
'foo' == array ( ) = false
'foo' == array ( 0 => 1 ) = false
'foo' == array ( 0 => 1, 1 => 100 ) = false
'foo' == array ( 'foo' => 1, 'bar' => 2 ) = false
'foo' == array ( 'bar' => 1, 'foo' => 2 ) = false
'foo' == (object) array ( ) = false
'foo' == (object) array ( 'foo' => 1, 'bar' => 2 ) = false
'foo' == (object) array ( 'bar' => 1, 'foo' => 2 ) = false
'foo' == DateTime = false
'foo' == resource = false
'foo' == NULL = false
array ( ) == false = true
array ( ) == true = false
array ( ) == 0 = false
array ( ) == 10 = false
array ( ) == 0.0 = false
array ( ) == 10.0 = false
array ( ) == 3.14 = false
array ( ) == '0' = false
array ( ) == '10' = false
array ( ) == '010' = false
array ( ) == '10 elephants' = false
array ( ) == 'foo' = false
array ( ) == array ( ) = true
array ( ) == array ( 0 => 1 ) = false
array ( ) == array ( 0 => 1, 1 => 100 ) = false
array ( ) == array ( 'foo' => 1, 'bar' => 2 ) = false
array ( ) == array ( 'bar' => 1, 'foo' => 2 ) = false
array ( ) == (object) array ( ) = false
array ( ) == (object) array ( 'foo' => 1, 'bar' => 2 ) = false
array ( ) == (object) array ( 'bar' => 1, 'foo' => 2 ) = false
array ( ) == DateTime = false
array ( ) == resource = false
array ( ) == NULL = true
array ( 0 => 1 ) == false = false
array ( 0 => 1 ) == true = true
array ( 0 => 1 ) == 0 = false
array ( 0 => 1 ) == 10 = false
array ( 0 => 1 ) == 0.0 = false
array ( 0 => 1 ) == 10.0 = false
array ( 0 => 1 ) == 3.14 = false
array ( 0 => 1 ) == '0' = false
array ( 0 => 1 ) == '10' = false
array ( 0 => 1 ) == '010' = false
array ( 0 => 1 ) == '10 elephants' = false
array ( 0 => 1 ) == 'foo' = false
array ( 0 => 1 ) == array ( ) = false
array ( 0 => 1 ) == array ( 0 => 1 ) = true
array ( 0 => 1 ) == array ( 0 => 1, 1 => 100 ) = false
array ( 0 => 1 ) == array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 0 => 1 ) == array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 0 => 1 ) == (object) array ( ) = false
array ( 0 => 1 ) == (object) array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 0 => 1 ) == (object) array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 0 => 1 ) == DateTime = false
array ( 0 => 1 ) == resource = false
array ( 0 => 1 ) == NULL = false
array ( 0 => 1, 1 => 100 ) == false = false
array ( 0 => 1, 1 => 100 ) == true = true
array ( 0 => 1, 1 => 100 ) == 0 = false
array ( 0 => 1, 1 => 100 ) == 10 = false
array ( 0 => 1, 1 => 100 ) == 0.0 = false
array ( 0 => 1, 1 => 100 ) == 10.0 = false
array ( 0 => 1, 1 => 100 ) == 3.14 = false
array ( 0 => 1, 1 => 100 ) == '0' = false
array ( 0 => 1, 1 => 100 ) == '10' = false
array ( 0 => 1, 1 => 100 ) == '010' = false
array ( 0 => 1, 1 => 100 ) == '10 elephants' = false
array ( 0 => 1, 1 => 100 ) == 'foo' = false
array ( 0 => 1, 1 => 100 ) == array ( ) = false
array ( 0 => 1, 1 => 100 ) == array ( 0 => 1 ) = false
array ( 0 => 1, 1 => 100 ) == array ( 0 => 1, 1 => 100 ) = true
array ( 0 => 1, 1 => 100 ) == array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 0 => 1, 1 => 100 ) == array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 0 => 1, 1 => 100 ) == (object) array ( ) = false
array ( 0 => 1, 1 => 100 ) == (object) array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 0 => 1, 1 => 100 ) == (object) array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 0 => 1, 1 => 100 ) == DateTime = false
array ( 0 => 1, 1 => 100 ) == resource = false
array ( 0 => 1, 1 => 100 ) == NULL = false
array ( 'foo' => 1, 'bar' => 2 ) == false = false
array ( 'foo' => 1, 'bar' => 2 ) == true = true
array ( 'foo' => 1, 'bar' => 2 ) == 0 = false
array ( 'foo' => 1, 'bar' => 2 ) == 10 = false
array ( 'foo' => 1, 'bar' => 2 ) == 0.0 = false
array ( 'foo' => 1, 'bar' => 2 ) == 10.0 = false
array ( 'foo' => 1, 'bar' => 2 ) == 3.14 = false
array ( 'foo' => 1, 'bar' => 2 ) == '0' = false
array ( 'foo' => 1, 'bar' => 2 ) == '10' = false
array ( 'foo' => 1, 'bar' => 2 ) == '010' = false
array ( 'foo' => 1, 'bar' => 2 ) == '10 elephants' = false
array ( 'foo' => 1, 'bar' => 2 ) == 'foo' = false
array ( 'foo' => 1, 'bar' => 2 ) == array ( ) = false
array ( 'foo' => 1, 'bar' => 2 ) == array ( 0 => 1 ) = false
array ( 'foo' => 1, 'bar' => 2 ) == array ( 0 => 1, 1 => 100 ) = false
array ( 'foo' => 1, 'bar' => 2 ) == array ( 'foo' => 1, 'bar' => 2 ) = true
array ( 'foo' => 1, 'bar' => 2 ) == array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 'foo' => 1, 'bar' => 2 ) == (object) array ( ) = false
array ( 'foo' => 1, 'bar' => 2 ) == (object) array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 'foo' => 1, 'bar' => 2 ) == (object) array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 'foo' => 1, 'bar' => 2 ) == DateTime = false
array ( 'foo' => 1, 'bar' => 2 ) == resource = false
array ( 'foo' => 1, 'bar' => 2 ) == NULL = false
array ( 'bar' => 1, 'foo' => 2 ) == false = false
array ( 'bar' => 1, 'foo' => 2 ) == true = true
array ( 'bar' => 1, 'foo' => 2 ) == 0 = false
array ( 'bar' => 1, 'foo' => 2 ) == 10 = false
array ( 'bar' => 1, 'foo' => 2 ) == 0.0 = false
array ( 'bar' => 1, 'foo' => 2 ) == 10.0 = false
array ( 'bar' => 1, 'foo' => 2 ) == 3.14 = false
array ( 'bar' => 1, 'foo' => 2 ) == '0' = false
array ( 'bar' => 1, 'foo' => 2 ) == '10' = false
array ( 'bar' => 1, 'foo' => 2 ) == '010' = false
array ( 'bar' => 1, 'foo' => 2 ) == '10 elephants' = false
array ( 'bar' => 1, 'foo' => 2 ) == 'foo' = false
array ( 'bar' => 1, 'foo' => 2 ) == array ( ) = false
array ( 'bar' => 1, 'foo' => 2 ) == array ( 0 => 1 ) = false
array ( 'bar' => 1, 'foo' => 2 ) == array ( 0 => 1, 1 => 100 ) = false
array ( 'bar' => 1, 'foo' => 2 ) == array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 'bar' => 1, 'foo' => 2 ) == array ( 'bar' => 1, 'foo' => 2 ) = true
array ( 'bar' => 1, 'foo' => 2 ) == (object) array ( ) = false
array ( 'bar' => 1, 'foo' => 2 ) == (object) array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 'bar' => 1, 'foo' => 2 ) == (object) array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 'bar' => 1, 'foo' => 2 ) == DateTime = false
array ( 'bar' => 1, 'foo' => 2 ) == resource = false
array ( 'bar' => 1, 'foo' => 2 ) == NULL = false
(object) array ( ) == false = false
(object) array ( ) == true = true
(object) array ( ) == 0 = false - Notice Object of class stdClass could not be converted to int
(object) array ( ) == 10 = false - Notice Object of class stdClass could not be converted to int
(object) array ( ) == 0.0 = false - Notice Object of class stdClass could not be converted to float
(object) array ( ) == 10.0 = false - Notice Object of class stdClass could not be converted to float
(object) array ( ) == 3.14 = false - Notice Object of class stdClass could not be converted to float
(object) array ( ) == '0' = false
(object) array ( ) == '10' = false
(object) array ( ) == '010' = false
(object) array ( ) == '10 elephants' = false
(object) array ( ) == 'foo' = false
(object) array ( ) == array ( ) = false
(object) array ( ) == array ( 0 => 1 ) = false
(object) array ( ) == array ( 0 => 1, 1 => 100 ) = false
(object) array ( ) == array ( 'foo' => 1, 'bar' => 2 ) = false
(object) array ( ) == array ( 'bar' => 1, 'foo' => 2 ) = false
(object) array ( ) == (object) array ( ) = true
(object) array ( ) == (object) array ( 'foo' => 1, 'bar' => 2 ) = false
(object) array ( ) == (object) array ( 'bar' => 1, 'foo' => 2 ) = false
(object) array ( ) == DateTime = false
(object) array ( ) == resource = false
(object) array ( ) == NULL = false
(object) array ( 'foo' => 1, 'bar' => 2 ) == false = false
(object) array ( 'foo' => 1, 'bar' => 2 ) == true = true
(object) array ( 'foo' => 1, 'bar' => 2 ) == 0 = false - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) == 10 = false - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) == 0.0 = false - Notice Object of class stdClass could not be converted to float
(object) array ( 'foo' => 1, 'bar' => 2 ) == 10.0 = false - Notice Object of class stdClass could not be converted to float
(object) array ( 'foo' => 1, 'bar' => 2 ) == 3.14 = false - Notice Object of class stdClass could not be converted to float
(object) array ( 'foo' => 1, 'bar' => 2 ) == '0' = false
(object) array ( 'foo' => 1, 'bar' => 2 ) == '10' = false
(object) array ( 'foo' => 1, 'bar' => 2 ) == '010' = false
(object) array ( 'foo' => 1, 'bar' => 2 ) == '10 elephants' = false
(object) array ( 'foo' => 1, 'bar' => 2 ) == 'foo' = false
(object) array ( 'foo' => 1, 'bar' => 2 ) == array ( ) = false
(object) array ( 'foo' => 1, 'bar' => 2 ) == array ( 0 => 1 ) = false
(object) array ( 'foo' => 1, 'bar' => 2 ) == array ( 0 => 1, 1 => 100 ) = false
(object) array ( 'foo' => 1, 'bar' => 2 ) == array ( 'foo' => 1, 'bar' => 2 ) = false
(object) array ( 'foo' => 1, 'bar' => 2 ) == array ( 'bar' => 1, 'foo' => 2 ) = false
(object) array ( 'foo' => 1, 'bar' => 2 ) == (object) array ( ) = false
(object) array ( 'foo' => 1, 'bar' => 2 ) == (object) array ( 'foo' => 1, 'bar' => 2 ) = true
(object) array ( 'foo' => 1, 'bar' => 2 ) == (object) array ( 'bar' => 1, 'foo' => 2 ) = false
(object) array ( 'foo' => 1, 'bar' => 2 ) == DateTime = false
(object) array ( 'foo' => 1, 'bar' => 2 ) == resource = false
(object) array ( 'foo' => 1, 'bar' => 2 ) == NULL = false
(object) array ( 'bar' => 1, 'foo' => 2 ) == false = false
(object) array ( 'bar' => 1, 'foo' => 2 ) == true = true
(object) array ( 'bar' => 1, 'foo' => 2 ) == 0 = false - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) == 10 = false - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) == 0.0 = false - Notice Object of class stdClass could not be converted to float
(object) array ( 'bar' => 1, 'foo' => 2 ) == 10.0 = false - Notice Object of class stdClass could not be converted to float
(object) array ( 'bar' => 1, 'foo' => 2 ) == 3.14 = false - Notice Object of class stdClass could not be converted to float
(object) array ( 'bar' => 1, 'foo' => 2 ) == '0' = false
(object) array ( 'bar' => 1, 'foo' => 2 ) == '10' = false
(object) array ( 'bar' => 1, 'foo' => 2 ) == '010' = false
(object) array ( 'bar' => 1, 'foo' => 2 ) == '10 elephants' = false
(object) array ( 'bar' => 1, 'foo' => 2 ) == 'foo' = false
(object) array ( 'bar' => 1, 'foo' => 2 ) == array ( ) = false
(object) array ( 'bar' => 1, 'foo' => 2 ) == array ( 0 => 1 ) = false
(object) array ( 'bar' => 1, 'foo' => 2 ) == array ( 0 => 1, 1 => 100 ) = false
(object) array ( 'bar' => 1, 'foo' => 2 ) == array ( 'foo' => 1, 'bar' => 2 ) = false
(object) array ( 'bar' => 1, 'foo' => 2 ) == array ( 'bar' => 1, 'foo' => 2 ) = false
(object) array ( 'bar' => 1, 'foo' => 2 ) == (object) array ( ) = false
(object) array ( 'bar' => 1, 'foo' => 2 ) == (object) array ( 'foo' => 1, 'bar' => 2 ) = false
(object) array ( 'bar' => 1, 'foo' => 2 ) == (object) array ( 'bar' => 1, 'foo' => 2 ) = true
(object) array ( 'bar' => 1, 'foo' => 2 ) == DateTime = false
(object) array ( 'bar' => 1, 'foo' => 2 ) == resource = false
(object) array ( 'bar' => 1, 'foo' => 2 ) == NULL = false
DateTime == false = false
DateTime == true = true
DateTime == 0 = false - Notice Object of class DateTime could not be converted to int
DateTime == 10 = false - Notice Object of class DateTime could not be converted to int
DateTime == 0.0 = false - Notice Object of class DateTime could not be converted to float
DateTime == 10.0 = false - Notice Object of class DateTime could not be converted to float
DateTime == 3.14 = false - Notice Object of class DateTime could not be converted to float
DateTime == '0' = false
DateTime == '10' = false
DateTime == '010' = false
DateTime == '10 elephants' = false
DateTime == 'foo' = false
DateTime == array ( ) = false
DateTime == array ( 0 => 1 ) = false
DateTime == array ( 0 => 1, 1 => 100 ) = false
DateTime == array ( 'foo' => 1, 'bar' => 2 ) = false
DateTime == array ( 'bar' => 1, 'foo' => 2 ) = false
DateTime == (object) array ( ) = false
DateTime == (object) array ( 'foo' => 1, 'bar' => 2 ) = false
DateTime == (object) array ( 'bar' => 1, 'foo' => 2 ) = false
DateTime == DateTime = true
DateTime == resource = false
DateTime == NULL = false
resource == false = false
resource == true = true
resource == 0 = false
resource == 10 = false
resource == 0.0 = false
resource == 10.0 = false
resource == 3.14 = false
resource == '0' = false
resource == '10' = false
resource == '010' = false
resource == '10 elephants' = false
resource == 'foo' = false
resource == array ( ) = false
resource == array ( 0 => 1 ) = false
resource == array ( 0 => 1, 1 => 100 ) = false
resource == array ( 'foo' => 1, 'bar' => 2 ) = false
resource == array ( 'bar' => 1, 'foo' => 2 ) = false
resource == (object) array ( ) = false
resource == (object) array ( 'foo' => 1, 'bar' => 2 ) = false
resource == (object) array ( 'bar' => 1, 'foo' => 2 ) = false
resource == DateTime = false
resource == resource = true
resource == NULL = false
NULL == false = true
NULL == true = false
NULL == 0 = true
NULL == 10 = false
NULL == 0.0 = true
NULL == 10.0 = false
NULL == 3.14 = false
NULL == '0' = false
NULL == '10' = false
NULL == '010' = false
NULL == '10 elephants' = false
NULL == 'foo' = false
NULL == array ( ) = true
NULL == array ( 0 => 1 ) = false
NULL == array ( 0 => 1, 1 => 100 ) = false
NULL == array ( 'foo' => 1, 'bar' => 2 ) = false
NULL == array ( 'bar' => 1, 'foo' => 2 ) = false
NULL == (object) array ( ) = false
NULL == (object) array ( 'foo' => 1, 'bar' => 2 ) = false
NULL == (object) array ( 'bar' => 1, 'foo' => 2 ) = false
NULL == DateTime = false
NULL == resource = false
NULL == NULL = true