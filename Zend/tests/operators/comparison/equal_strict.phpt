--TEST--
equal '==' operator with strict_operators
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

$fn = function($a, $b) { return $a == $b; };

test_two_operands('$a == $b', $fn);

--EXPECT--
false == false = true
false == true = false
false == 0 - TypeError Operand type mismatch bool and int for comparison operator
false == 10 - TypeError Operand type mismatch bool and int for comparison operator
false == 0.0 - TypeError Operand type mismatch bool and float for comparison operator
false == 10.0 - TypeError Operand type mismatch bool and float for comparison operator
false == 3.14 - TypeError Operand type mismatch bool and float for comparison operator
false == '0' - TypeError Operand type mismatch bool and string for comparison operator
false == '10' - TypeError Operand type mismatch bool and string for comparison operator
false == '010' - TypeError Operand type mismatch bool and string for comparison operator
false == '10 elephants' - TypeError Operand type mismatch bool and string for comparison operator
false == 'foo' - TypeError Operand type mismatch bool and string for comparison operator
false == array ( ) - TypeError Operand type mismatch bool and array for comparison operator
false == array ( 0 => 1 ) - TypeError Operand type mismatch bool and array for comparison operator
false == array ( 0 => 1, 1 => 100 ) - TypeError Operand type mismatch bool and array for comparison operator
false == array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch bool and array for comparison operator
false == array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch bool and array for comparison operator
false == (object) array ( ) - TypeError Operand type mismatch bool and object for comparison operator
false == (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch bool and object for comparison operator
false == (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch bool and object for comparison operator
false == DateTime - TypeError Operand type mismatch bool and object for comparison operator
false == resource - TypeError Operand type mismatch bool and resource for comparison operator
false == NULL - TypeError Operand type mismatch bool and null for comparison operator
true == false = false
true == true = true
true == 0 - TypeError Operand type mismatch bool and int for comparison operator
true == 10 - TypeError Operand type mismatch bool and int for comparison operator
true == 0.0 - TypeError Operand type mismatch bool and float for comparison operator
true == 10.0 - TypeError Operand type mismatch bool and float for comparison operator
true == 3.14 - TypeError Operand type mismatch bool and float for comparison operator
true == '0' - TypeError Operand type mismatch bool and string for comparison operator
true == '10' - TypeError Operand type mismatch bool and string for comparison operator
true == '010' - TypeError Operand type mismatch bool and string for comparison operator
true == '10 elephants' - TypeError Operand type mismatch bool and string for comparison operator
true == 'foo' - TypeError Operand type mismatch bool and string for comparison operator
true == array ( ) - TypeError Operand type mismatch bool and array for comparison operator
true == array ( 0 => 1 ) - TypeError Operand type mismatch bool and array for comparison operator
true == array ( 0 => 1, 1 => 100 ) - TypeError Operand type mismatch bool and array for comparison operator
true == array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch bool and array for comparison operator
true == array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch bool and array for comparison operator
true == (object) array ( ) - TypeError Operand type mismatch bool and object for comparison operator
true == (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch bool and object for comparison operator
true == (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch bool and object for comparison operator
true == DateTime - TypeError Operand type mismatch bool and object for comparison operator
true == resource - TypeError Operand type mismatch bool and resource for comparison operator
true == NULL - TypeError Operand type mismatch bool and null for comparison operator
0 == false - TypeError Operand type mismatch int and bool for comparison operator
0 == true - TypeError Operand type mismatch int and bool for comparison operator
0 == 0 = true
0 == 10 = false
0 == 0.0 = true
0 == 10.0 = false
0 == 3.14 = false
0 == '0' - TypeError Operand type mismatch int and string for comparison operator
0 == '10' - TypeError Operand type mismatch int and string for comparison operator
0 == '010' - TypeError Operand type mismatch int and string for comparison operator
0 == '10 elephants' - TypeError Operand type mismatch int and string for comparison operator
0 == 'foo' - TypeError Operand type mismatch int and string for comparison operator
0 == array ( ) - TypeError Operand type mismatch int and array for comparison operator
0 == array ( 0 => 1 ) - TypeError Operand type mismatch int and array for comparison operator
0 == array ( 0 => 1, 1 => 100 ) - TypeError Operand type mismatch int and array for comparison operator
0 == array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch int and array for comparison operator
0 == array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch int and array for comparison operator
0 == (object) array ( ) - TypeError Operand type mismatch int and object for comparison operator
0 == (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch int and object for comparison operator
0 == (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch int and object for comparison operator
0 == DateTime - TypeError Operand type mismatch int and object for comparison operator
0 == resource - TypeError Operand type mismatch int and resource for comparison operator
0 == NULL - TypeError Operand type mismatch int and null for comparison operator
10 == false - TypeError Operand type mismatch int and bool for comparison operator
10 == true - TypeError Operand type mismatch int and bool for comparison operator
10 == 0 = false
10 == 10 = true
10 == 0.0 = false
10 == 10.0 = true
10 == 3.14 = false
10 == '0' - TypeError Operand type mismatch int and string for comparison operator
10 == '10' - TypeError Operand type mismatch int and string for comparison operator
10 == '010' - TypeError Operand type mismatch int and string for comparison operator
10 == '10 elephants' - TypeError Operand type mismatch int and string for comparison operator
10 == 'foo' - TypeError Operand type mismatch int and string for comparison operator
10 == array ( ) - TypeError Operand type mismatch int and array for comparison operator
10 == array ( 0 => 1 ) - TypeError Operand type mismatch int and array for comparison operator
10 == array ( 0 => 1, 1 => 100 ) - TypeError Operand type mismatch int and array for comparison operator
10 == array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch int and array for comparison operator
10 == array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch int and array for comparison operator
10 == (object) array ( ) - TypeError Operand type mismatch int and object for comparison operator
10 == (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch int and object for comparison operator
10 == (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch int and object for comparison operator
10 == DateTime - TypeError Operand type mismatch int and object for comparison operator
10 == resource - TypeError Operand type mismatch int and resource for comparison operator
10 == NULL - TypeError Operand type mismatch int and null for comparison operator
0.0 == false - TypeError Operand type mismatch float and bool for comparison operator
0.0 == true - TypeError Operand type mismatch float and bool for comparison operator
0.0 == 0 = true
0.0 == 10 = false
0.0 == 0.0 = true
0.0 == 10.0 = false
0.0 == 3.14 = false
0.0 == '0' - TypeError Operand type mismatch float and string for comparison operator
0.0 == '10' - TypeError Operand type mismatch float and string for comparison operator
0.0 == '010' - TypeError Operand type mismatch float and string for comparison operator
0.0 == '10 elephants' - TypeError Operand type mismatch float and string for comparison operator
0.0 == 'foo' - TypeError Operand type mismatch float and string for comparison operator
0.0 == array ( ) - TypeError Operand type mismatch float and array for comparison operator
0.0 == array ( 0 => 1 ) - TypeError Operand type mismatch float and array for comparison operator
0.0 == array ( 0 => 1, 1 => 100 ) - TypeError Operand type mismatch float and array for comparison operator
0.0 == array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch float and array for comparison operator
0.0 == array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch float and array for comparison operator
0.0 == (object) array ( ) - TypeError Operand type mismatch float and object for comparison operator
0.0 == (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch float and object for comparison operator
0.0 == (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch float and object for comparison operator
0.0 == DateTime - TypeError Operand type mismatch float and object for comparison operator
0.0 == resource - TypeError Operand type mismatch float and resource for comparison operator
0.0 == NULL - TypeError Operand type mismatch float and null for comparison operator
10.0 == false - TypeError Operand type mismatch float and bool for comparison operator
10.0 == true - TypeError Operand type mismatch float and bool for comparison operator
10.0 == 0 = false
10.0 == 10 = true
10.0 == 0.0 = false
10.0 == 10.0 = true
10.0 == 3.14 = false
10.0 == '0' - TypeError Operand type mismatch float and string for comparison operator
10.0 == '10' - TypeError Operand type mismatch float and string for comparison operator
10.0 == '010' - TypeError Operand type mismatch float and string for comparison operator
10.0 == '10 elephants' - TypeError Operand type mismatch float and string for comparison operator
10.0 == 'foo' - TypeError Operand type mismatch float and string for comparison operator
10.0 == array ( ) - TypeError Operand type mismatch float and array for comparison operator
10.0 == array ( 0 => 1 ) - TypeError Operand type mismatch float and array for comparison operator
10.0 == array ( 0 => 1, 1 => 100 ) - TypeError Operand type mismatch float and array for comparison operator
10.0 == array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch float and array for comparison operator
10.0 == array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch float and array for comparison operator
10.0 == (object) array ( ) - TypeError Operand type mismatch float and object for comparison operator
10.0 == (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch float and object for comparison operator
10.0 == (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch float and object for comparison operator
10.0 == DateTime - TypeError Operand type mismatch float and object for comparison operator
10.0 == resource - TypeError Operand type mismatch float and resource for comparison operator
10.0 == NULL - TypeError Operand type mismatch float and null for comparison operator
3.14 == false - TypeError Operand type mismatch float and bool for comparison operator
3.14 == true - TypeError Operand type mismatch float and bool for comparison operator
3.14 == 0 = false
3.14 == 10 = false
3.14 == 0.0 = false
3.14 == 10.0 = false
3.14 == 3.14 = true
3.14 == '0' - TypeError Operand type mismatch float and string for comparison operator
3.14 == '10' - TypeError Operand type mismatch float and string for comparison operator
3.14 == '010' - TypeError Operand type mismatch float and string for comparison operator
3.14 == '10 elephants' - TypeError Operand type mismatch float and string for comparison operator
3.14 == 'foo' - TypeError Operand type mismatch float and string for comparison operator
3.14 == array ( ) - TypeError Operand type mismatch float and array for comparison operator
3.14 == array ( 0 => 1 ) - TypeError Operand type mismatch float and array for comparison operator
3.14 == array ( 0 => 1, 1 => 100 ) - TypeError Operand type mismatch float and array for comparison operator
3.14 == array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch float and array for comparison operator
3.14 == array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch float and array for comparison operator
3.14 == (object) array ( ) - TypeError Operand type mismatch float and object for comparison operator
3.14 == (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch float and object for comparison operator
3.14 == (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch float and object for comparison operator
3.14 == DateTime - TypeError Operand type mismatch float and object for comparison operator
3.14 == resource - TypeError Operand type mismatch float and resource for comparison operator
3.14 == NULL - TypeError Operand type mismatch float and null for comparison operator
'0' == false - TypeError Operand type mismatch string and bool for comparison operator
'0' == true - TypeError Operand type mismatch string and bool for comparison operator
'0' == 0 - TypeError Operand type mismatch string and int for comparison operator
'0' == 10 - TypeError Operand type mismatch string and int for comparison operator
'0' == 0.0 - TypeError Operand type mismatch string and float for comparison operator
'0' == 10.0 - TypeError Operand type mismatch string and float for comparison operator
'0' == 3.14 - TypeError Operand type mismatch string and float for comparison operator
'0' == '0' = true
'0' == '10' = false
'0' == '010' = false
'0' == '10 elephants' = false
'0' == 'foo' = false
'0' == array ( ) - TypeError Operand type mismatch string and array for comparison operator
'0' == array ( 0 => 1 ) - TypeError Operand type mismatch string and array for comparison operator
'0' == array ( 0 => 1, 1 => 100 ) - TypeError Operand type mismatch string and array for comparison operator
'0' == array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch string and array for comparison operator
'0' == array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch string and array for comparison operator
'0' == (object) array ( ) - TypeError Operand type mismatch string and object for comparison operator
'0' == (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch string and object for comparison operator
'0' == (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch string and object for comparison operator
'0' == DateTime - TypeError Operand type mismatch string and object for comparison operator
'0' == resource - TypeError Operand type mismatch string and resource for comparison operator
'0' == NULL - TypeError Operand type mismatch string and null for comparison operator
'10' == false - TypeError Operand type mismatch string and bool for comparison operator
'10' == true - TypeError Operand type mismatch string and bool for comparison operator
'10' == 0 - TypeError Operand type mismatch string and int for comparison operator
'10' == 10 - TypeError Operand type mismatch string and int for comparison operator
'10' == 0.0 - TypeError Operand type mismatch string and float for comparison operator
'10' == 10.0 - TypeError Operand type mismatch string and float for comparison operator
'10' == 3.14 - TypeError Operand type mismatch string and float for comparison operator
'10' == '0' = false
'10' == '10' = true
'10' == '010' = false
'10' == '10 elephants' = false
'10' == 'foo' = false
'10' == array ( ) - TypeError Operand type mismatch string and array for comparison operator
'10' == array ( 0 => 1 ) - TypeError Operand type mismatch string and array for comparison operator
'10' == array ( 0 => 1, 1 => 100 ) - TypeError Operand type mismatch string and array for comparison operator
'10' == array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch string and array for comparison operator
'10' == array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch string and array for comparison operator
'10' == (object) array ( ) - TypeError Operand type mismatch string and object for comparison operator
'10' == (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch string and object for comparison operator
'10' == (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch string and object for comparison operator
'10' == DateTime - TypeError Operand type mismatch string and object for comparison operator
'10' == resource - TypeError Operand type mismatch string and resource for comparison operator
'10' == NULL - TypeError Operand type mismatch string and null for comparison operator
'010' == false - TypeError Operand type mismatch string and bool for comparison operator
'010' == true - TypeError Operand type mismatch string and bool for comparison operator
'010' == 0 - TypeError Operand type mismatch string and int for comparison operator
'010' == 10 - TypeError Operand type mismatch string and int for comparison operator
'010' == 0.0 - TypeError Operand type mismatch string and float for comparison operator
'010' == 10.0 - TypeError Operand type mismatch string and float for comparison operator
'010' == 3.14 - TypeError Operand type mismatch string and float for comparison operator
'010' == '0' = false
'010' == '10' = false
'010' == '010' = true
'010' == '10 elephants' = false
'010' == 'foo' = false
'010' == array ( ) - TypeError Operand type mismatch string and array for comparison operator
'010' == array ( 0 => 1 ) - TypeError Operand type mismatch string and array for comparison operator
'010' == array ( 0 => 1, 1 => 100 ) - TypeError Operand type mismatch string and array for comparison operator
'010' == array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch string and array for comparison operator
'010' == array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch string and array for comparison operator
'010' == (object) array ( ) - TypeError Operand type mismatch string and object for comparison operator
'010' == (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch string and object for comparison operator
'010' == (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch string and object for comparison operator
'010' == DateTime - TypeError Operand type mismatch string and object for comparison operator
'010' == resource - TypeError Operand type mismatch string and resource for comparison operator
'010' == NULL - TypeError Operand type mismatch string and null for comparison operator
'10 elephants' == false - TypeError Operand type mismatch string and bool for comparison operator
'10 elephants' == true - TypeError Operand type mismatch string and bool for comparison operator
'10 elephants' == 0 - TypeError Operand type mismatch string and int for comparison operator
'10 elephants' == 10 - TypeError Operand type mismatch string and int for comparison operator
'10 elephants' == 0.0 - TypeError Operand type mismatch string and float for comparison operator
'10 elephants' == 10.0 - TypeError Operand type mismatch string and float for comparison operator
'10 elephants' == 3.14 - TypeError Operand type mismatch string and float for comparison operator
'10 elephants' == '0' = false
'10 elephants' == '10' = false
'10 elephants' == '010' = false
'10 elephants' == '10 elephants' = true
'10 elephants' == 'foo' = false
'10 elephants' == array ( ) - TypeError Operand type mismatch string and array for comparison operator
'10 elephants' == array ( 0 => 1 ) - TypeError Operand type mismatch string and array for comparison operator
'10 elephants' == array ( 0 => 1, 1 => 100 ) - TypeError Operand type mismatch string and array for comparison operator
'10 elephants' == array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch string and array for comparison operator
'10 elephants' == array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch string and array for comparison operator
'10 elephants' == (object) array ( ) - TypeError Operand type mismatch string and object for comparison operator
'10 elephants' == (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch string and object for comparison operator
'10 elephants' == (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch string and object for comparison operator
'10 elephants' == DateTime - TypeError Operand type mismatch string and object for comparison operator
'10 elephants' == resource - TypeError Operand type mismatch string and resource for comparison operator
'10 elephants' == NULL - TypeError Operand type mismatch string and null for comparison operator
'foo' == false - TypeError Operand type mismatch string and bool for comparison operator
'foo' == true - TypeError Operand type mismatch string and bool for comparison operator
'foo' == 0 - TypeError Operand type mismatch string and int for comparison operator
'foo' == 10 - TypeError Operand type mismatch string and int for comparison operator
'foo' == 0.0 - TypeError Operand type mismatch string and float for comparison operator
'foo' == 10.0 - TypeError Operand type mismatch string and float for comparison operator
'foo' == 3.14 - TypeError Operand type mismatch string and float for comparison operator
'foo' == '0' = false
'foo' == '10' = false
'foo' == '010' = false
'foo' == '10 elephants' = false
'foo' == 'foo' = true
'foo' == array ( ) - TypeError Operand type mismatch string and array for comparison operator
'foo' == array ( 0 => 1 ) - TypeError Operand type mismatch string and array for comparison operator
'foo' == array ( 0 => 1, 1 => 100 ) - TypeError Operand type mismatch string and array for comparison operator
'foo' == array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch string and array for comparison operator
'foo' == array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch string and array for comparison operator
'foo' == (object) array ( ) - TypeError Operand type mismatch string and object for comparison operator
'foo' == (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch string and object for comparison operator
'foo' == (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch string and object for comparison operator
'foo' == DateTime - TypeError Operand type mismatch string and object for comparison operator
'foo' == resource - TypeError Operand type mismatch string and resource for comparison operator
'foo' == NULL - TypeError Operand type mismatch string and null for comparison operator
array ( ) == false - TypeError Operand type mismatch array and bool for comparison operator
array ( ) == true - TypeError Operand type mismatch array and bool for comparison operator
array ( ) == 0 - TypeError Operand type mismatch array and int for comparison operator
array ( ) == 10 - TypeError Operand type mismatch array and int for comparison operator
array ( ) == 0.0 - TypeError Operand type mismatch array and float for comparison operator
array ( ) == 10.0 - TypeError Operand type mismatch array and float for comparison operator
array ( ) == 3.14 - TypeError Operand type mismatch array and float for comparison operator
array ( ) == '0' - TypeError Operand type mismatch array and string for comparison operator
array ( ) == '10' - TypeError Operand type mismatch array and string for comparison operator
array ( ) == '010' - TypeError Operand type mismatch array and string for comparison operator
array ( ) == '10 elephants' - TypeError Operand type mismatch array and string for comparison operator
array ( ) == 'foo' - TypeError Operand type mismatch array and string for comparison operator
array ( ) == array ( ) = true
array ( ) == array ( 0 => 1 ) = false
array ( ) == array ( 0 => 1, 1 => 100 ) = false
array ( ) == array ( 'foo' => 1, 'bar' => 2 ) = false
array ( ) == array ( 'bar' => 1, 'foo' => 2 ) = false
array ( ) == (object) array ( ) - TypeError Operand type mismatch array and object for comparison operator
array ( ) == (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch array and object for comparison operator
array ( ) == (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch array and object for comparison operator
array ( ) == DateTime - TypeError Operand type mismatch array and object for comparison operator
array ( ) == resource - TypeError Operand type mismatch array and resource for comparison operator
array ( ) == NULL - TypeError Operand type mismatch array and null for comparison operator
array ( 0 => 1 ) == false - TypeError Operand type mismatch array and bool for comparison operator
array ( 0 => 1 ) == true - TypeError Operand type mismatch array and bool for comparison operator
array ( 0 => 1 ) == 0 - TypeError Operand type mismatch array and int for comparison operator
array ( 0 => 1 ) == 10 - TypeError Operand type mismatch array and int for comparison operator
array ( 0 => 1 ) == 0.0 - TypeError Operand type mismatch array and float for comparison operator
array ( 0 => 1 ) == 10.0 - TypeError Operand type mismatch array and float for comparison operator
array ( 0 => 1 ) == 3.14 - TypeError Operand type mismatch array and float for comparison operator
array ( 0 => 1 ) == '0' - TypeError Operand type mismatch array and string for comparison operator
array ( 0 => 1 ) == '10' - TypeError Operand type mismatch array and string for comparison operator
array ( 0 => 1 ) == '010' - TypeError Operand type mismatch array and string for comparison operator
array ( 0 => 1 ) == '10 elephants' - TypeError Operand type mismatch array and string for comparison operator
array ( 0 => 1 ) == 'foo' - TypeError Operand type mismatch array and string for comparison operator
array ( 0 => 1 ) == array ( ) = false
array ( 0 => 1 ) == array ( 0 => 1 ) = true
array ( 0 => 1 ) == array ( 0 => 1, 1 => 100 ) = false
array ( 0 => 1 ) == array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 0 => 1 ) == array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 0 => 1 ) == (object) array ( ) - TypeError Operand type mismatch array and object for comparison operator
array ( 0 => 1 ) == (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch array and object for comparison operator
array ( 0 => 1 ) == (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch array and object for comparison operator
array ( 0 => 1 ) == DateTime - TypeError Operand type mismatch array and object for comparison operator
array ( 0 => 1 ) == resource - TypeError Operand type mismatch array and resource for comparison operator
array ( 0 => 1 ) == NULL - TypeError Operand type mismatch array and null for comparison operator
array ( 0 => 1, 1 => 100 ) == false - TypeError Operand type mismatch array and bool for comparison operator
array ( 0 => 1, 1 => 100 ) == true - TypeError Operand type mismatch array and bool for comparison operator
array ( 0 => 1, 1 => 100 ) == 0 - TypeError Operand type mismatch array and int for comparison operator
array ( 0 => 1, 1 => 100 ) == 10 - TypeError Operand type mismatch array and int for comparison operator
array ( 0 => 1, 1 => 100 ) == 0.0 - TypeError Operand type mismatch array and float for comparison operator
array ( 0 => 1, 1 => 100 ) == 10.0 - TypeError Operand type mismatch array and float for comparison operator
array ( 0 => 1, 1 => 100 ) == 3.14 - TypeError Operand type mismatch array and float for comparison operator
array ( 0 => 1, 1 => 100 ) == '0' - TypeError Operand type mismatch array and string for comparison operator
array ( 0 => 1, 1 => 100 ) == '10' - TypeError Operand type mismatch array and string for comparison operator
array ( 0 => 1, 1 => 100 ) == '010' - TypeError Operand type mismatch array and string for comparison operator
array ( 0 => 1, 1 => 100 ) == '10 elephants' - TypeError Operand type mismatch array and string for comparison operator
array ( 0 => 1, 1 => 100 ) == 'foo' - TypeError Operand type mismatch array and string for comparison operator
array ( 0 => 1, 1 => 100 ) == array ( ) = false
array ( 0 => 1, 1 => 100 ) == array ( 0 => 1 ) = false
array ( 0 => 1, 1 => 100 ) == array ( 0 => 1, 1 => 100 ) = true
array ( 0 => 1, 1 => 100 ) == array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 0 => 1, 1 => 100 ) == array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 0 => 1, 1 => 100 ) == (object) array ( ) - TypeError Operand type mismatch array and object for comparison operator
array ( 0 => 1, 1 => 100 ) == (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch array and object for comparison operator
array ( 0 => 1, 1 => 100 ) == (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch array and object for comparison operator
array ( 0 => 1, 1 => 100 ) == DateTime - TypeError Operand type mismatch array and object for comparison operator
array ( 0 => 1, 1 => 100 ) == resource - TypeError Operand type mismatch array and resource for comparison operator
array ( 0 => 1, 1 => 100 ) == NULL - TypeError Operand type mismatch array and null for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) == false - TypeError Operand type mismatch array and bool for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) == true - TypeError Operand type mismatch array and bool for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) == 0 - TypeError Operand type mismatch array and int for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) == 10 - TypeError Operand type mismatch array and int for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) == 0.0 - TypeError Operand type mismatch array and float for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) == 10.0 - TypeError Operand type mismatch array and float for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) == 3.14 - TypeError Operand type mismatch array and float for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) == '0' - TypeError Operand type mismatch array and string for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) == '10' - TypeError Operand type mismatch array and string for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) == '010' - TypeError Operand type mismatch array and string for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) == '10 elephants' - TypeError Operand type mismatch array and string for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) == 'foo' - TypeError Operand type mismatch array and string for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) == array ( ) = false
array ( 'foo' => 1, 'bar' => 2 ) == array ( 0 => 1 ) = false
array ( 'foo' => 1, 'bar' => 2 ) == array ( 0 => 1, 1 => 100 ) = false
array ( 'foo' => 1, 'bar' => 2 ) == array ( 'foo' => 1, 'bar' => 2 ) = true
array ( 'foo' => 1, 'bar' => 2 ) == array ( 'bar' => 1, 'foo' => 2 ) = false
array ( 'foo' => 1, 'bar' => 2 ) == (object) array ( ) - TypeError Operand type mismatch array and object for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) == (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch array and object for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) == (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch array and object for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) == DateTime - TypeError Operand type mismatch array and object for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) == resource - TypeError Operand type mismatch array and resource for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) == NULL - TypeError Operand type mismatch array and null for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) == false - TypeError Operand type mismatch array and bool for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) == true - TypeError Operand type mismatch array and bool for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) == 0 - TypeError Operand type mismatch array and int for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) == 10 - TypeError Operand type mismatch array and int for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) == 0.0 - TypeError Operand type mismatch array and float for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) == 10.0 - TypeError Operand type mismatch array and float for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) == 3.14 - TypeError Operand type mismatch array and float for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) == '0' - TypeError Operand type mismatch array and string for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) == '10' - TypeError Operand type mismatch array and string for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) == '010' - TypeError Operand type mismatch array and string for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) == '10 elephants' - TypeError Operand type mismatch array and string for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) == 'foo' - TypeError Operand type mismatch array and string for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) == array ( ) = false
array ( 'bar' => 1, 'foo' => 2 ) == array ( 0 => 1 ) = false
array ( 'bar' => 1, 'foo' => 2 ) == array ( 0 => 1, 1 => 100 ) = false
array ( 'bar' => 1, 'foo' => 2 ) == array ( 'foo' => 1, 'bar' => 2 ) = false
array ( 'bar' => 1, 'foo' => 2 ) == array ( 'bar' => 1, 'foo' => 2 ) = true
array ( 'bar' => 1, 'foo' => 2 ) == (object) array ( ) - TypeError Operand type mismatch array and object for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) == (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch array and object for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) == (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch array and object for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) == DateTime - TypeError Operand type mismatch array and object for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) == resource - TypeError Operand type mismatch array and resource for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) == NULL - TypeError Operand type mismatch array and null for comparison operator
(object) array ( ) == false - TypeError Operand type mismatch object and bool for comparison operator
(object) array ( ) == true - TypeError Operand type mismatch object and bool for comparison operator
(object) array ( ) == 0 - TypeError Operand type mismatch object and int for comparison operator
(object) array ( ) == 10 - TypeError Operand type mismatch object and int for comparison operator
(object) array ( ) == 0.0 - TypeError Operand type mismatch object and float for comparison operator
(object) array ( ) == 10.0 - TypeError Operand type mismatch object and float for comparison operator
(object) array ( ) == 3.14 - TypeError Operand type mismatch object and float for comparison operator
(object) array ( ) == '0' - TypeError Operand type mismatch object and string for comparison operator
(object) array ( ) == '10' - TypeError Operand type mismatch object and string for comparison operator
(object) array ( ) == '010' - TypeError Operand type mismatch object and string for comparison operator
(object) array ( ) == '10 elephants' - TypeError Operand type mismatch object and string for comparison operator
(object) array ( ) == 'foo' - TypeError Operand type mismatch object and string for comparison operator
(object) array ( ) == array ( ) - TypeError Operand type mismatch object and array for comparison operator
(object) array ( ) == array ( 0 => 1 ) - TypeError Operand type mismatch object and array for comparison operator
(object) array ( ) == array ( 0 => 1, 1 => 100 ) - TypeError Operand type mismatch object and array for comparison operator
(object) array ( ) == array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch object and array for comparison operator
(object) array ( ) == array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch object and array for comparison operator
(object) array ( ) == (object) array ( ) = true
(object) array ( ) == (object) array ( 'foo' => 1, 'bar' => 2 ) = false
(object) array ( ) == (object) array ( 'bar' => 1, 'foo' => 2 ) = false
(object) array ( ) == DateTime - TypeError Operand type mismatch object and object for comparison operator
(object) array ( ) == resource - TypeError Operand type mismatch object and resource for comparison operator
(object) array ( ) == NULL - TypeError Operand type mismatch object and null for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) == false - TypeError Operand type mismatch object and bool for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) == true - TypeError Operand type mismatch object and bool for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) == 0 - TypeError Operand type mismatch object and int for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) == 10 - TypeError Operand type mismatch object and int for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) == 0.0 - TypeError Operand type mismatch object and float for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) == 10.0 - TypeError Operand type mismatch object and float for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) == 3.14 - TypeError Operand type mismatch object and float for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) == '0' - TypeError Operand type mismatch object and string for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) == '10' - TypeError Operand type mismatch object and string for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) == '010' - TypeError Operand type mismatch object and string for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) == '10 elephants' - TypeError Operand type mismatch object and string for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) == 'foo' - TypeError Operand type mismatch object and string for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) == array ( ) - TypeError Operand type mismatch object and array for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) == array ( 0 => 1 ) - TypeError Operand type mismatch object and array for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) == array ( 0 => 1, 1 => 100 ) - TypeError Operand type mismatch object and array for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) == array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch object and array for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) == array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch object and array for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) == (object) array ( ) = false
(object) array ( 'foo' => 1, 'bar' => 2 ) == (object) array ( 'foo' => 1, 'bar' => 2 ) = true
(object) array ( 'foo' => 1, 'bar' => 2 ) == (object) array ( 'bar' => 1, 'foo' => 2 ) = false
(object) array ( 'foo' => 1, 'bar' => 2 ) == DateTime - TypeError Operand type mismatch object and object for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) == resource - TypeError Operand type mismatch object and resource for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) == NULL - TypeError Operand type mismatch object and null for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) == false - TypeError Operand type mismatch object and bool for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) == true - TypeError Operand type mismatch object and bool for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) == 0 - TypeError Operand type mismatch object and int for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) == 10 - TypeError Operand type mismatch object and int for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) == 0.0 - TypeError Operand type mismatch object and float for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) == 10.0 - TypeError Operand type mismatch object and float for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) == 3.14 - TypeError Operand type mismatch object and float for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) == '0' - TypeError Operand type mismatch object and string for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) == '10' - TypeError Operand type mismatch object and string for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) == '010' - TypeError Operand type mismatch object and string for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) == '10 elephants' - TypeError Operand type mismatch object and string for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) == 'foo' - TypeError Operand type mismatch object and string for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) == array ( ) - TypeError Operand type mismatch object and array for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) == array ( 0 => 1 ) - TypeError Operand type mismatch object and array for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) == array ( 0 => 1, 1 => 100 ) - TypeError Operand type mismatch object and array for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) == array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch object and array for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) == array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch object and array for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) == (object) array ( ) = false
(object) array ( 'bar' => 1, 'foo' => 2 ) == (object) array ( 'foo' => 1, 'bar' => 2 ) = false
(object) array ( 'bar' => 1, 'foo' => 2 ) == (object) array ( 'bar' => 1, 'foo' => 2 ) = true
(object) array ( 'bar' => 1, 'foo' => 2 ) == DateTime - TypeError Operand type mismatch object and object for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) == resource - TypeError Operand type mismatch object and resource for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) == NULL - TypeError Operand type mismatch object and null for comparison operator
DateTime == false - TypeError Operand type mismatch object and bool for comparison operator
DateTime == true - TypeError Operand type mismatch object and bool for comparison operator
DateTime == 0 - TypeError Operand type mismatch object and int for comparison operator
DateTime == 10 - TypeError Operand type mismatch object and int for comparison operator
DateTime == 0.0 - TypeError Operand type mismatch object and float for comparison operator
DateTime == 10.0 - TypeError Operand type mismatch object and float for comparison operator
DateTime == 3.14 - TypeError Operand type mismatch object and float for comparison operator
DateTime == '0' - TypeError Operand type mismatch object and string for comparison operator
DateTime == '10' - TypeError Operand type mismatch object and string for comparison operator
DateTime == '010' - TypeError Operand type mismatch object and string for comparison operator
DateTime == '10 elephants' - TypeError Operand type mismatch object and string for comparison operator
DateTime == 'foo' - TypeError Operand type mismatch object and string for comparison operator
DateTime == array ( ) - TypeError Operand type mismatch object and array for comparison operator
DateTime == array ( 0 => 1 ) - TypeError Operand type mismatch object and array for comparison operator
DateTime == array ( 0 => 1, 1 => 100 ) - TypeError Operand type mismatch object and array for comparison operator
DateTime == array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch object and array for comparison operator
DateTime == array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch object and array for comparison operator
DateTime == (object) array ( ) - TypeError Operand type mismatch object and object for comparison operator
DateTime == (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch object and object for comparison operator
DateTime == (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch object and object for comparison operator
DateTime == DateTime = true
DateTime == resource - TypeError Operand type mismatch object and resource for comparison operator
DateTime == NULL - TypeError Operand type mismatch object and null for comparison operator
resource == false - TypeError Operand type mismatch resource and bool for comparison operator
resource == true - TypeError Operand type mismatch resource and bool for comparison operator
resource == 0 - TypeError Operand type mismatch resource and int for comparison operator
resource == 10 - TypeError Operand type mismatch resource and int for comparison operator
resource == 0.0 - TypeError Operand type mismatch resource and float for comparison operator
resource == 10.0 - TypeError Operand type mismatch resource and float for comparison operator
resource == 3.14 - TypeError Operand type mismatch resource and float for comparison operator
resource == '0' - TypeError Operand type mismatch resource and string for comparison operator
resource == '10' - TypeError Operand type mismatch resource and string for comparison operator
resource == '010' - TypeError Operand type mismatch resource and string for comparison operator
resource == '10 elephants' - TypeError Operand type mismatch resource and string for comparison operator
resource == 'foo' - TypeError Operand type mismatch resource and string for comparison operator
resource == array ( ) - TypeError Operand type mismatch resource and array for comparison operator
resource == array ( 0 => 1 ) - TypeError Operand type mismatch resource and array for comparison operator
resource == array ( 0 => 1, 1 => 100 ) - TypeError Operand type mismatch resource and array for comparison operator
resource == array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch resource and array for comparison operator
resource == array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch resource and array for comparison operator
resource == (object) array ( ) - TypeError Operand type mismatch resource and object for comparison operator
resource == (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch resource and object for comparison operator
resource == (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch resource and object for comparison operator
resource == DateTime - TypeError Operand type mismatch resource and object for comparison operator
resource == resource = true
resource == NULL - TypeError Operand type mismatch resource and null for comparison operator
NULL == false - TypeError Operand type mismatch null and bool for comparison operator
NULL == true - TypeError Operand type mismatch null and bool for comparison operator
NULL == 0 - TypeError Operand type mismatch null and int for comparison operator
NULL == 10 - TypeError Operand type mismatch null and int for comparison operator
NULL == 0.0 - TypeError Operand type mismatch null and float for comparison operator
NULL == 10.0 - TypeError Operand type mismatch null and float for comparison operator
NULL == 3.14 - TypeError Operand type mismatch null and float for comparison operator
NULL == '0' - TypeError Operand type mismatch null and string for comparison operator
NULL == '10' - TypeError Operand type mismatch null and string for comparison operator
NULL == '010' - TypeError Operand type mismatch null and string for comparison operator
NULL == '10 elephants' - TypeError Operand type mismatch null and string for comparison operator
NULL == 'foo' - TypeError Operand type mismatch null and string for comparison operator
NULL == array ( ) - TypeError Operand type mismatch null and array for comparison operator
NULL == array ( 0 => 1 ) - TypeError Operand type mismatch null and array for comparison operator
NULL == array ( 0 => 1, 1 => 100 ) - TypeError Operand type mismatch null and array for comparison operator
NULL == array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch null and array for comparison operator
NULL == array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch null and array for comparison operator
NULL == (object) array ( ) - TypeError Operand type mismatch null and object for comparison operator
NULL == (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Operand type mismatch null and object for comparison operator
NULL == (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Operand type mismatch null and object for comparison operator
NULL == DateTime - TypeError Operand type mismatch null and object for comparison operator
NULL == resource - TypeError Operand type mismatch null and resource for comparison operator
NULL == NULL = true