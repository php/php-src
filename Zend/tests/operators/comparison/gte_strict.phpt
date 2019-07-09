--TEST--
greater than or equal to '>=' operator
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a >= $b', function($a, $b) { return $a >= $b; });

--EXPECT--
false >= false = true
false >= true = false
false >= 0 - TypeError Operand type mismatch int and bool for comparison
false >= 10 - TypeError Operand type mismatch int and bool for comparison
false >= 0.0 - TypeError Operand type mismatch float and bool for comparison
false >= 10.0 - TypeError Operand type mismatch float and bool for comparison
false >= 3.14 - TypeError Operand type mismatch float and bool for comparison
false >= '0' - TypeError Operand type mismatch string and bool for comparison
false >= '10' - TypeError Operand type mismatch string and bool for comparison
false >= '010' - TypeError Operand type mismatch string and bool for comparison
false >= '10 elephants' - TypeError Operand type mismatch string and bool for comparison
false >= 'foo' - TypeError Operand type mismatch string and bool for comparison
false >= array ( ) - TypeError Unsupported operand type array for comparison
false >= array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison
false >= array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison
false >= array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison
false >= array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison
false >= (object) array ( ) - TypeError Unsupported operand type object for comparison
false >= (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison
false >= (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison
false >= DateTime - TypeError Unsupported operand type object for comparison
false >= resource - TypeError Unsupported operand type resource for comparison
false >= NULL - TypeError Unsupported operand type null for comparison
true >= false = true
true >= true = true
true >= 0 - TypeError Operand type mismatch int and bool for comparison
true >= 10 - TypeError Operand type mismatch int and bool for comparison
true >= 0.0 - TypeError Operand type mismatch float and bool for comparison
true >= 10.0 - TypeError Operand type mismatch float and bool for comparison
true >= 3.14 - TypeError Operand type mismatch float and bool for comparison
true >= '0' - TypeError Operand type mismatch string and bool for comparison
true >= '10' - TypeError Operand type mismatch string and bool for comparison
true >= '010' - TypeError Operand type mismatch string and bool for comparison
true >= '10 elephants' - TypeError Operand type mismatch string and bool for comparison
true >= 'foo' - TypeError Operand type mismatch string and bool for comparison
true >= array ( ) - TypeError Unsupported operand type array for comparison
true >= array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison
true >= array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison
true >= array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison
true >= array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison
true >= (object) array ( ) - TypeError Unsupported operand type object for comparison
true >= (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison
true >= (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison
true >= DateTime - TypeError Unsupported operand type object for comparison
true >= resource - TypeError Unsupported operand type resource for comparison
true >= NULL - TypeError Unsupported operand type null for comparison
0 >= false - TypeError Operand type mismatch bool and int for comparison
0 >= true - TypeError Operand type mismatch bool and int for comparison
0 >= 0 = true
0 >= 10 = false
0 >= 0.0 = true
0 >= 10.0 = false
0 >= 3.14 = false
0 >= '0' - TypeError Operand type mismatch string and int for comparison
0 >= '10' - TypeError Operand type mismatch string and int for comparison
0 >= '010' - TypeError Operand type mismatch string and int for comparison
0 >= '10 elephants' - TypeError Operand type mismatch string and int for comparison
0 >= 'foo' - TypeError Operand type mismatch string and int for comparison
0 >= array ( ) - TypeError Unsupported operand type array for comparison
0 >= array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison
0 >= array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison
0 >= array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison
0 >= array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison
0 >= (object) array ( ) - TypeError Unsupported operand type object for comparison
0 >= (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison
0 >= (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison
0 >= DateTime - TypeError Unsupported operand type object for comparison
0 >= resource - TypeError Unsupported operand type resource for comparison
0 >= NULL - TypeError Unsupported operand type null for comparison
10 >= false - TypeError Operand type mismatch bool and int for comparison
10 >= true - TypeError Operand type mismatch bool and int for comparison
10 >= 0 = true
10 >= 10 = true
10 >= 0.0 = true
10 >= 10.0 = true
10 >= 3.14 = true
10 >= '0' - TypeError Operand type mismatch string and int for comparison
10 >= '10' - TypeError Operand type mismatch string and int for comparison
10 >= '010' - TypeError Operand type mismatch string and int for comparison
10 >= '10 elephants' - TypeError Operand type mismatch string and int for comparison
10 >= 'foo' - TypeError Operand type mismatch string and int for comparison
10 >= array ( ) - TypeError Unsupported operand type array for comparison
10 >= array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison
10 >= array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison
10 >= array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison
10 >= array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison
10 >= (object) array ( ) - TypeError Unsupported operand type object for comparison
10 >= (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison
10 >= (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison
10 >= DateTime - TypeError Unsupported operand type object for comparison
10 >= resource - TypeError Unsupported operand type resource for comparison
10 >= NULL - TypeError Unsupported operand type null for comparison
0.0 >= false - TypeError Operand type mismatch bool and float for comparison
0.0 >= true - TypeError Operand type mismatch bool and float for comparison
0.0 >= 0 = true
0.0 >= 10 = false
0.0 >= 0.0 = true
0.0 >= 10.0 = false
0.0 >= 3.14 = false
0.0 >= '0' - TypeError Operand type mismatch string and float for comparison
0.0 >= '10' - TypeError Operand type mismatch string and float for comparison
0.0 >= '010' - TypeError Operand type mismatch string and float for comparison
0.0 >= '10 elephants' - TypeError Operand type mismatch string and float for comparison
0.0 >= 'foo' - TypeError Operand type mismatch string and float for comparison
0.0 >= array ( ) - TypeError Unsupported operand type array for comparison
0.0 >= array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison
0.0 >= array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison
0.0 >= array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison
0.0 >= array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison
0.0 >= (object) array ( ) - TypeError Unsupported operand type object for comparison
0.0 >= (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison
0.0 >= (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison
0.0 >= DateTime - TypeError Unsupported operand type object for comparison
0.0 >= resource - TypeError Unsupported operand type resource for comparison
0.0 >= NULL - TypeError Unsupported operand type null for comparison
10.0 >= false - TypeError Operand type mismatch bool and float for comparison
10.0 >= true - TypeError Operand type mismatch bool and float for comparison
10.0 >= 0 = true
10.0 >= 10 = true
10.0 >= 0.0 = true
10.0 >= 10.0 = true
10.0 >= 3.14 = true
10.0 >= '0' - TypeError Operand type mismatch string and float for comparison
10.0 >= '10' - TypeError Operand type mismatch string and float for comparison
10.0 >= '010' - TypeError Operand type mismatch string and float for comparison
10.0 >= '10 elephants' - TypeError Operand type mismatch string and float for comparison
10.0 >= 'foo' - TypeError Operand type mismatch string and float for comparison
10.0 >= array ( ) - TypeError Unsupported operand type array for comparison
10.0 >= array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison
10.0 >= array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison
10.0 >= array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison
10.0 >= array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison
10.0 >= (object) array ( ) - TypeError Unsupported operand type object for comparison
10.0 >= (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison
10.0 >= (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison
10.0 >= DateTime - TypeError Unsupported operand type object for comparison
10.0 >= resource - TypeError Unsupported operand type resource for comparison
10.0 >= NULL - TypeError Unsupported operand type null for comparison
3.14 >= false - TypeError Operand type mismatch bool and float for comparison
3.14 >= true - TypeError Operand type mismatch bool and float for comparison
3.14 >= 0 = true
3.14 >= 10 = false
3.14 >= 0.0 = true
3.14 >= 10.0 = false
3.14 >= 3.14 = true
3.14 >= '0' - TypeError Operand type mismatch string and float for comparison
3.14 >= '10' - TypeError Operand type mismatch string and float for comparison
3.14 >= '010' - TypeError Operand type mismatch string and float for comparison
3.14 >= '10 elephants' - TypeError Operand type mismatch string and float for comparison
3.14 >= 'foo' - TypeError Operand type mismatch string and float for comparison
3.14 >= array ( ) - TypeError Unsupported operand type array for comparison
3.14 >= array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison
3.14 >= array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison
3.14 >= array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison
3.14 >= array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison
3.14 >= (object) array ( ) - TypeError Unsupported operand type object for comparison
3.14 >= (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison
3.14 >= (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison
3.14 >= DateTime - TypeError Unsupported operand type object for comparison
3.14 >= resource - TypeError Unsupported operand type resource for comparison
3.14 >= NULL - TypeError Unsupported operand type null for comparison
'0' >= false - TypeError Operand type mismatch bool and string for comparison
'0' >= true - TypeError Operand type mismatch bool and string for comparison
'0' >= 0 - TypeError Operand type mismatch int and string for comparison
'0' >= 10 - TypeError Operand type mismatch int and string for comparison
'0' >= 0.0 - TypeError Operand type mismatch float and string for comparison
'0' >= 10.0 - TypeError Operand type mismatch float and string for comparison
'0' >= 3.14 - TypeError Operand type mismatch float and string for comparison
'0' >= '0' = true
'0' >= '10' = false
'0' >= '010' = false
'0' >= '10 elephants' = false
'0' >= 'foo' = false
'0' >= array ( ) - TypeError Unsupported operand type array for comparison
'0' >= array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison
'0' >= array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison
'0' >= array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison
'0' >= array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison
'0' >= (object) array ( ) - TypeError Unsupported operand type object for comparison
'0' >= (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison
'0' >= (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison
'0' >= DateTime - TypeError Unsupported operand type object for comparison
'0' >= resource - TypeError Unsupported operand type resource for comparison
'0' >= NULL - TypeError Unsupported operand type null for comparison
'10' >= false - TypeError Operand type mismatch bool and string for comparison
'10' >= true - TypeError Operand type mismatch bool and string for comparison
'10' >= 0 - TypeError Operand type mismatch int and string for comparison
'10' >= 10 - TypeError Operand type mismatch int and string for comparison
'10' >= 0.0 - TypeError Operand type mismatch float and string for comparison
'10' >= 10.0 - TypeError Operand type mismatch float and string for comparison
'10' >= 3.14 - TypeError Operand type mismatch float and string for comparison
'10' >= '0' = true
'10' >= '10' = true
'10' >= '010' = true
'10' >= '10 elephants' = false
'10' >= 'foo' = false
'10' >= array ( ) - TypeError Unsupported operand type array for comparison
'10' >= array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison
'10' >= array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison
'10' >= array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison
'10' >= array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison
'10' >= (object) array ( ) - TypeError Unsupported operand type object for comparison
'10' >= (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison
'10' >= (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison
'10' >= DateTime - TypeError Unsupported operand type object for comparison
'10' >= resource - TypeError Unsupported operand type resource for comparison
'10' >= NULL - TypeError Unsupported operand type null for comparison
'010' >= false - TypeError Operand type mismatch bool and string for comparison
'010' >= true - TypeError Operand type mismatch bool and string for comparison
'010' >= 0 - TypeError Operand type mismatch int and string for comparison
'010' >= 10 - TypeError Operand type mismatch int and string for comparison
'010' >= 0.0 - TypeError Operand type mismatch float and string for comparison
'010' >= 10.0 - TypeError Operand type mismatch float and string for comparison
'010' >= 3.14 - TypeError Operand type mismatch float and string for comparison
'010' >= '0' = true
'010' >= '10' = false
'010' >= '010' = true
'010' >= '10 elephants' = false
'010' >= 'foo' = false
'010' >= array ( ) - TypeError Unsupported operand type array for comparison
'010' >= array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison
'010' >= array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison
'010' >= array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison
'010' >= array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison
'010' >= (object) array ( ) - TypeError Unsupported operand type object for comparison
'010' >= (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison
'010' >= (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison
'010' >= DateTime - TypeError Unsupported operand type object for comparison
'010' >= resource - TypeError Unsupported operand type resource for comparison
'010' >= NULL - TypeError Unsupported operand type null for comparison
'10 elephants' >= false - TypeError Operand type mismatch bool and string for comparison
'10 elephants' >= true - TypeError Operand type mismatch bool and string for comparison
'10 elephants' >= 0 - TypeError Operand type mismatch int and string for comparison
'10 elephants' >= 10 - TypeError Operand type mismatch int and string for comparison
'10 elephants' >= 0.0 - TypeError Operand type mismatch float and string for comparison
'10 elephants' >= 10.0 - TypeError Operand type mismatch float and string for comparison
'10 elephants' >= 3.14 - TypeError Operand type mismatch float and string for comparison
'10 elephants' >= '0' = true
'10 elephants' >= '10' = true
'10 elephants' >= '010' = true
'10 elephants' >= '10 elephants' = true
'10 elephants' >= 'foo' = false
'10 elephants' >= array ( ) - TypeError Unsupported operand type array for comparison
'10 elephants' >= array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison
'10 elephants' >= array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison
'10 elephants' >= array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison
'10 elephants' >= array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison
'10 elephants' >= (object) array ( ) - TypeError Unsupported operand type object for comparison
'10 elephants' >= (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison
'10 elephants' >= (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison
'10 elephants' >= DateTime - TypeError Unsupported operand type object for comparison
'10 elephants' >= resource - TypeError Unsupported operand type resource for comparison
'10 elephants' >= NULL - TypeError Unsupported operand type null for comparison
'foo' >= false - TypeError Operand type mismatch bool and string for comparison
'foo' >= true - TypeError Operand type mismatch bool and string for comparison
'foo' >= 0 - TypeError Operand type mismatch int and string for comparison
'foo' >= 10 - TypeError Operand type mismatch int and string for comparison
'foo' >= 0.0 - TypeError Operand type mismatch float and string for comparison
'foo' >= 10.0 - TypeError Operand type mismatch float and string for comparison
'foo' >= 3.14 - TypeError Operand type mismatch float and string for comparison
'foo' >= '0' = true
'foo' >= '10' = true
'foo' >= '010' = true
'foo' >= '10 elephants' = true
'foo' >= 'foo' = true
'foo' >= array ( ) - TypeError Unsupported operand type array for comparison
'foo' >= array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison
'foo' >= array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison
'foo' >= array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison
'foo' >= array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison
'foo' >= (object) array ( ) - TypeError Unsupported operand type object for comparison
'foo' >= (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison
'foo' >= (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison
'foo' >= DateTime - TypeError Unsupported operand type object for comparison
'foo' >= resource - TypeError Unsupported operand type resource for comparison
'foo' >= NULL - TypeError Unsupported operand type null for comparison
array ( ) >= false - TypeError Unsupported operand type array for comparison
array ( ) >= true - TypeError Unsupported operand type array for comparison
array ( ) >= 0 - TypeError Unsupported operand type array for comparison
array ( ) >= 10 - TypeError Unsupported operand type array for comparison
array ( ) >= 0.0 - TypeError Unsupported operand type array for comparison
array ( ) >= 10.0 - TypeError Unsupported operand type array for comparison
array ( ) >= 3.14 - TypeError Unsupported operand type array for comparison
array ( ) >= '0' - TypeError Unsupported operand type array for comparison
array ( ) >= '10' - TypeError Unsupported operand type array for comparison
array ( ) >= '010' - TypeError Unsupported operand type array for comparison
array ( ) >= '10 elephants' - TypeError Unsupported operand type array for comparison
array ( ) >= 'foo' - TypeError Unsupported operand type array for comparison
array ( ) >= array ( ) - TypeError Unsupported operand type array for comparison
array ( ) >= array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison
array ( ) >= array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison
array ( ) >= array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison
array ( ) >= array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison
array ( ) >= (object) array ( ) - TypeError Unsupported operand type object for comparison
array ( ) >= (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison
array ( ) >= (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison
array ( ) >= DateTime - TypeError Unsupported operand type object for comparison
array ( ) >= resource - TypeError Unsupported operand type resource for comparison
array ( ) >= NULL - TypeError Unsupported operand type null for comparison
array ( 0 => 1 ) >= false - TypeError Unsupported operand type array for comparison
array ( 0 => 1 ) >= true - TypeError Unsupported operand type array for comparison
array ( 0 => 1 ) >= 0 - TypeError Unsupported operand type array for comparison
array ( 0 => 1 ) >= 10 - TypeError Unsupported operand type array for comparison
array ( 0 => 1 ) >= 0.0 - TypeError Unsupported operand type array for comparison
array ( 0 => 1 ) >= 10.0 - TypeError Unsupported operand type array for comparison
array ( 0 => 1 ) >= 3.14 - TypeError Unsupported operand type array for comparison
array ( 0 => 1 ) >= '0' - TypeError Unsupported operand type array for comparison
array ( 0 => 1 ) >= '10' - TypeError Unsupported operand type array for comparison
array ( 0 => 1 ) >= '010' - TypeError Unsupported operand type array for comparison
array ( 0 => 1 ) >= '10 elephants' - TypeError Unsupported operand type array for comparison
array ( 0 => 1 ) >= 'foo' - TypeError Unsupported operand type array for comparison
array ( 0 => 1 ) >= array ( ) - TypeError Unsupported operand type array for comparison
array ( 0 => 1 ) >= array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison
array ( 0 => 1 ) >= array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison
array ( 0 => 1 ) >= array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison
array ( 0 => 1 ) >= array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison
array ( 0 => 1 ) >= (object) array ( ) - TypeError Unsupported operand type object for comparison
array ( 0 => 1 ) >= (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison
array ( 0 => 1 ) >= (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison
array ( 0 => 1 ) >= DateTime - TypeError Unsupported operand type object for comparison
array ( 0 => 1 ) >= resource - TypeError Unsupported operand type resource for comparison
array ( 0 => 1 ) >= NULL - TypeError Unsupported operand type null for comparison
array ( 0 => 1, 1 => 100 ) >= false - TypeError Unsupported operand type array for comparison
array ( 0 => 1, 1 => 100 ) >= true - TypeError Unsupported operand type array for comparison
array ( 0 => 1, 1 => 100 ) >= 0 - TypeError Unsupported operand type array for comparison
array ( 0 => 1, 1 => 100 ) >= 10 - TypeError Unsupported operand type array for comparison
array ( 0 => 1, 1 => 100 ) >= 0.0 - TypeError Unsupported operand type array for comparison
array ( 0 => 1, 1 => 100 ) >= 10.0 - TypeError Unsupported operand type array for comparison
array ( 0 => 1, 1 => 100 ) >= 3.14 - TypeError Unsupported operand type array for comparison
array ( 0 => 1, 1 => 100 ) >= '0' - TypeError Unsupported operand type array for comparison
array ( 0 => 1, 1 => 100 ) >= '10' - TypeError Unsupported operand type array for comparison
array ( 0 => 1, 1 => 100 ) >= '010' - TypeError Unsupported operand type array for comparison
array ( 0 => 1, 1 => 100 ) >= '10 elephants' - TypeError Unsupported operand type array for comparison
array ( 0 => 1, 1 => 100 ) >= 'foo' - TypeError Unsupported operand type array for comparison
array ( 0 => 1, 1 => 100 ) >= array ( ) - TypeError Unsupported operand type array for comparison
array ( 0 => 1, 1 => 100 ) >= array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison
array ( 0 => 1, 1 => 100 ) >= array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison
array ( 0 => 1, 1 => 100 ) >= array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison
array ( 0 => 1, 1 => 100 ) >= array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison
array ( 0 => 1, 1 => 100 ) >= (object) array ( ) - TypeError Unsupported operand type object for comparison
array ( 0 => 1, 1 => 100 ) >= (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison
array ( 0 => 1, 1 => 100 ) >= (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison
array ( 0 => 1, 1 => 100 ) >= DateTime - TypeError Unsupported operand type object for comparison
array ( 0 => 1, 1 => 100 ) >= resource - TypeError Unsupported operand type resource for comparison
array ( 0 => 1, 1 => 100 ) >= NULL - TypeError Unsupported operand type null for comparison
array ( 'foo' => 1, 'bar' => 2 ) >= false - TypeError Unsupported operand type array for comparison
array ( 'foo' => 1, 'bar' => 2 ) >= true - TypeError Unsupported operand type array for comparison
array ( 'foo' => 1, 'bar' => 2 ) >= 0 - TypeError Unsupported operand type array for comparison
array ( 'foo' => 1, 'bar' => 2 ) >= 10 - TypeError Unsupported operand type array for comparison
array ( 'foo' => 1, 'bar' => 2 ) >= 0.0 - TypeError Unsupported operand type array for comparison
array ( 'foo' => 1, 'bar' => 2 ) >= 10.0 - TypeError Unsupported operand type array for comparison
array ( 'foo' => 1, 'bar' => 2 ) >= 3.14 - TypeError Unsupported operand type array for comparison
array ( 'foo' => 1, 'bar' => 2 ) >= '0' - TypeError Unsupported operand type array for comparison
array ( 'foo' => 1, 'bar' => 2 ) >= '10' - TypeError Unsupported operand type array for comparison
array ( 'foo' => 1, 'bar' => 2 ) >= '010' - TypeError Unsupported operand type array for comparison
array ( 'foo' => 1, 'bar' => 2 ) >= '10 elephants' - TypeError Unsupported operand type array for comparison
array ( 'foo' => 1, 'bar' => 2 ) >= 'foo' - TypeError Unsupported operand type array for comparison
array ( 'foo' => 1, 'bar' => 2 ) >= array ( ) - TypeError Unsupported operand type array for comparison
array ( 'foo' => 1, 'bar' => 2 ) >= array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison
array ( 'foo' => 1, 'bar' => 2 ) >= array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison
array ( 'foo' => 1, 'bar' => 2 ) >= array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison
array ( 'foo' => 1, 'bar' => 2 ) >= array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison
array ( 'foo' => 1, 'bar' => 2 ) >= (object) array ( ) - TypeError Unsupported operand type object for comparison
array ( 'foo' => 1, 'bar' => 2 ) >= (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison
array ( 'foo' => 1, 'bar' => 2 ) >= (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison
array ( 'foo' => 1, 'bar' => 2 ) >= DateTime - TypeError Unsupported operand type object for comparison
array ( 'foo' => 1, 'bar' => 2 ) >= resource - TypeError Unsupported operand type resource for comparison
array ( 'foo' => 1, 'bar' => 2 ) >= NULL - TypeError Unsupported operand type null for comparison
array ( 'bar' => 1, 'foo' => 2 ) >= false - TypeError Unsupported operand type array for comparison
array ( 'bar' => 1, 'foo' => 2 ) >= true - TypeError Unsupported operand type array for comparison
array ( 'bar' => 1, 'foo' => 2 ) >= 0 - TypeError Unsupported operand type array for comparison
array ( 'bar' => 1, 'foo' => 2 ) >= 10 - TypeError Unsupported operand type array for comparison
array ( 'bar' => 1, 'foo' => 2 ) >= 0.0 - TypeError Unsupported operand type array for comparison
array ( 'bar' => 1, 'foo' => 2 ) >= 10.0 - TypeError Unsupported operand type array for comparison
array ( 'bar' => 1, 'foo' => 2 ) >= 3.14 - TypeError Unsupported operand type array for comparison
array ( 'bar' => 1, 'foo' => 2 ) >= '0' - TypeError Unsupported operand type array for comparison
array ( 'bar' => 1, 'foo' => 2 ) >= '10' - TypeError Unsupported operand type array for comparison
array ( 'bar' => 1, 'foo' => 2 ) >= '010' - TypeError Unsupported operand type array for comparison
array ( 'bar' => 1, 'foo' => 2 ) >= '10 elephants' - TypeError Unsupported operand type array for comparison
array ( 'bar' => 1, 'foo' => 2 ) >= 'foo' - TypeError Unsupported operand type array for comparison
array ( 'bar' => 1, 'foo' => 2 ) >= array ( ) - TypeError Unsupported operand type array for comparison
array ( 'bar' => 1, 'foo' => 2 ) >= array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison
array ( 'bar' => 1, 'foo' => 2 ) >= array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison
array ( 'bar' => 1, 'foo' => 2 ) >= array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison
array ( 'bar' => 1, 'foo' => 2 ) >= array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison
array ( 'bar' => 1, 'foo' => 2 ) >= (object) array ( ) - TypeError Unsupported operand type object for comparison
array ( 'bar' => 1, 'foo' => 2 ) >= (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison
array ( 'bar' => 1, 'foo' => 2 ) >= (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison
array ( 'bar' => 1, 'foo' => 2 ) >= DateTime - TypeError Unsupported operand type object for comparison
array ( 'bar' => 1, 'foo' => 2 ) >= resource - TypeError Unsupported operand type resource for comparison
array ( 'bar' => 1, 'foo' => 2 ) >= NULL - TypeError Unsupported operand type null for comparison
(object) array ( ) >= false - TypeError Unsupported operand type object for comparison
(object) array ( ) >= true - TypeError Unsupported operand type object for comparison
(object) array ( ) >= 0 - TypeError Unsupported operand type object for comparison
(object) array ( ) >= 10 - TypeError Unsupported operand type object for comparison
(object) array ( ) >= 0.0 - TypeError Unsupported operand type object for comparison
(object) array ( ) >= 10.0 - TypeError Unsupported operand type object for comparison
(object) array ( ) >= 3.14 - TypeError Unsupported operand type object for comparison
(object) array ( ) >= '0' - TypeError Unsupported operand type object for comparison
(object) array ( ) >= '10' - TypeError Unsupported operand type object for comparison
(object) array ( ) >= '010' - TypeError Unsupported operand type object for comparison
(object) array ( ) >= '10 elephants' - TypeError Unsupported operand type object for comparison
(object) array ( ) >= 'foo' - TypeError Unsupported operand type object for comparison
(object) array ( ) >= array ( ) - TypeError Unsupported operand type array for comparison
(object) array ( ) >= array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison
(object) array ( ) >= array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison
(object) array ( ) >= array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison
(object) array ( ) >= array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison
(object) array ( ) >= (object) array ( ) - TypeError Unsupported operand type object for comparison
(object) array ( ) >= (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison
(object) array ( ) >= (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison
(object) array ( ) >= DateTime - TypeError Unsupported operand type object for comparison
(object) array ( ) >= resource - TypeError Unsupported operand type resource for comparison
(object) array ( ) >= NULL - TypeError Unsupported operand type null for comparison
(object) array ( 'foo' => 1, 'bar' => 2 ) >= false - TypeError Unsupported operand type object for comparison
(object) array ( 'foo' => 1, 'bar' => 2 ) >= true - TypeError Unsupported operand type object for comparison
(object) array ( 'foo' => 1, 'bar' => 2 ) >= 0 - TypeError Unsupported operand type object for comparison
(object) array ( 'foo' => 1, 'bar' => 2 ) >= 10 - TypeError Unsupported operand type object for comparison
(object) array ( 'foo' => 1, 'bar' => 2 ) >= 0.0 - TypeError Unsupported operand type object for comparison
(object) array ( 'foo' => 1, 'bar' => 2 ) >= 10.0 - TypeError Unsupported operand type object for comparison
(object) array ( 'foo' => 1, 'bar' => 2 ) >= 3.14 - TypeError Unsupported operand type object for comparison
(object) array ( 'foo' => 1, 'bar' => 2 ) >= '0' - TypeError Unsupported operand type object for comparison
(object) array ( 'foo' => 1, 'bar' => 2 ) >= '10' - TypeError Unsupported operand type object for comparison
(object) array ( 'foo' => 1, 'bar' => 2 ) >= '010' - TypeError Unsupported operand type object for comparison
(object) array ( 'foo' => 1, 'bar' => 2 ) >= '10 elephants' - TypeError Unsupported operand type object for comparison
(object) array ( 'foo' => 1, 'bar' => 2 ) >= 'foo' - TypeError Unsupported operand type object for comparison
(object) array ( 'foo' => 1, 'bar' => 2 ) >= array ( ) - TypeError Unsupported operand type array for comparison
(object) array ( 'foo' => 1, 'bar' => 2 ) >= array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison
(object) array ( 'foo' => 1, 'bar' => 2 ) >= array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison
(object) array ( 'foo' => 1, 'bar' => 2 ) >= array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison
(object) array ( 'foo' => 1, 'bar' => 2 ) >= array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison
(object) array ( 'foo' => 1, 'bar' => 2 ) >= (object) array ( ) - TypeError Unsupported operand type object for comparison
(object) array ( 'foo' => 1, 'bar' => 2 ) >= (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison
(object) array ( 'foo' => 1, 'bar' => 2 ) >= (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison
(object) array ( 'foo' => 1, 'bar' => 2 ) >= DateTime - TypeError Unsupported operand type object for comparison
(object) array ( 'foo' => 1, 'bar' => 2 ) >= resource - TypeError Unsupported operand type resource for comparison
(object) array ( 'foo' => 1, 'bar' => 2 ) >= NULL - TypeError Unsupported operand type null for comparison
(object) array ( 'bar' => 1, 'foo' => 2 ) >= false - TypeError Unsupported operand type object for comparison
(object) array ( 'bar' => 1, 'foo' => 2 ) >= true - TypeError Unsupported operand type object for comparison
(object) array ( 'bar' => 1, 'foo' => 2 ) >= 0 - TypeError Unsupported operand type object for comparison
(object) array ( 'bar' => 1, 'foo' => 2 ) >= 10 - TypeError Unsupported operand type object for comparison
(object) array ( 'bar' => 1, 'foo' => 2 ) >= 0.0 - TypeError Unsupported operand type object for comparison
(object) array ( 'bar' => 1, 'foo' => 2 ) >= 10.0 - TypeError Unsupported operand type object for comparison
(object) array ( 'bar' => 1, 'foo' => 2 ) >= 3.14 - TypeError Unsupported operand type object for comparison
(object) array ( 'bar' => 1, 'foo' => 2 ) >= '0' - TypeError Unsupported operand type object for comparison
(object) array ( 'bar' => 1, 'foo' => 2 ) >= '10' - TypeError Unsupported operand type object for comparison
(object) array ( 'bar' => 1, 'foo' => 2 ) >= '010' - TypeError Unsupported operand type object for comparison
(object) array ( 'bar' => 1, 'foo' => 2 ) >= '10 elephants' - TypeError Unsupported operand type object for comparison
(object) array ( 'bar' => 1, 'foo' => 2 ) >= 'foo' - TypeError Unsupported operand type object for comparison
(object) array ( 'bar' => 1, 'foo' => 2 ) >= array ( ) - TypeError Unsupported operand type array for comparison
(object) array ( 'bar' => 1, 'foo' => 2 ) >= array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison
(object) array ( 'bar' => 1, 'foo' => 2 ) >= array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison
(object) array ( 'bar' => 1, 'foo' => 2 ) >= array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison
(object) array ( 'bar' => 1, 'foo' => 2 ) >= array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison
(object) array ( 'bar' => 1, 'foo' => 2 ) >= (object) array ( ) - TypeError Unsupported operand type object for comparison
(object) array ( 'bar' => 1, 'foo' => 2 ) >= (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison
(object) array ( 'bar' => 1, 'foo' => 2 ) >= (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison
(object) array ( 'bar' => 1, 'foo' => 2 ) >= DateTime - TypeError Unsupported operand type object for comparison
(object) array ( 'bar' => 1, 'foo' => 2 ) >= resource - TypeError Unsupported operand type resource for comparison
(object) array ( 'bar' => 1, 'foo' => 2 ) >= NULL - TypeError Unsupported operand type null for comparison
DateTime >= false - TypeError Unsupported operand type object for comparison
DateTime >= true - TypeError Unsupported operand type object for comparison
DateTime >= 0 - TypeError Unsupported operand type object for comparison
DateTime >= 10 - TypeError Unsupported operand type object for comparison
DateTime >= 0.0 - TypeError Unsupported operand type object for comparison
DateTime >= 10.0 - TypeError Unsupported operand type object for comparison
DateTime >= 3.14 - TypeError Unsupported operand type object for comparison
DateTime >= '0' - TypeError Unsupported operand type object for comparison
DateTime >= '10' - TypeError Unsupported operand type object for comparison
DateTime >= '010' - TypeError Unsupported operand type object for comparison
DateTime >= '10 elephants' - TypeError Unsupported operand type object for comparison
DateTime >= 'foo' - TypeError Unsupported operand type object for comparison
DateTime >= array ( ) - TypeError Unsupported operand type array for comparison
DateTime >= array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison
DateTime >= array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison
DateTime >= array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison
DateTime >= array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison
DateTime >= (object) array ( ) - TypeError Unsupported operand type object for comparison
DateTime >= (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison
DateTime >= (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison
DateTime >= DateTime - TypeError Unsupported operand type object for comparison
DateTime >= resource - TypeError Unsupported operand type resource for comparison
DateTime >= NULL - TypeError Unsupported operand type null for comparison
resource >= false - TypeError Unsupported operand type resource for comparison
resource >= true - TypeError Unsupported operand type resource for comparison
resource >= 0 - TypeError Unsupported operand type resource for comparison
resource >= 10 - TypeError Unsupported operand type resource for comparison
resource >= 0.0 - TypeError Unsupported operand type resource for comparison
resource >= 10.0 - TypeError Unsupported operand type resource for comparison
resource >= 3.14 - TypeError Unsupported operand type resource for comparison
resource >= '0' - TypeError Unsupported operand type resource for comparison
resource >= '10' - TypeError Unsupported operand type resource for comparison
resource >= '010' - TypeError Unsupported operand type resource for comparison
resource >= '10 elephants' - TypeError Unsupported operand type resource for comparison
resource >= 'foo' - TypeError Unsupported operand type resource for comparison
resource >= array ( ) - TypeError Unsupported operand type array for comparison
resource >= array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison
resource >= array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison
resource >= array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison
resource >= array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison
resource >= (object) array ( ) - TypeError Unsupported operand type object for comparison
resource >= (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison
resource >= (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison
resource >= DateTime - TypeError Unsupported operand type object for comparison
resource >= resource - TypeError Unsupported operand type resource for comparison
resource >= NULL - TypeError Unsupported operand type null for comparison
NULL >= false - TypeError Unsupported operand type null for comparison
NULL >= true - TypeError Unsupported operand type null for comparison
NULL >= 0 - TypeError Unsupported operand type null for comparison
NULL >= 10 - TypeError Unsupported operand type null for comparison
NULL >= 0.0 - TypeError Unsupported operand type null for comparison
NULL >= 10.0 - TypeError Unsupported operand type null for comparison
NULL >= 3.14 - TypeError Unsupported operand type null for comparison
NULL >= '0' - TypeError Unsupported operand type null for comparison
NULL >= '10' - TypeError Unsupported operand type null for comparison
NULL >= '010' - TypeError Unsupported operand type null for comparison
NULL >= '10 elephants' - TypeError Unsupported operand type null for comparison
NULL >= 'foo' - TypeError Unsupported operand type null for comparison
NULL >= array ( ) - TypeError Unsupported operand type array for comparison
NULL >= array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison
NULL >= array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison
NULL >= array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison
NULL >= array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison
NULL >= (object) array ( ) - TypeError Unsupported operand type object for comparison
NULL >= (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison
NULL >= (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison
NULL >= DateTime - TypeError Unsupported operand type object for comparison
NULL >= resource - TypeError Unsupported operand type resource for comparison
NULL >= NULL - TypeError Unsupported operand type null for comparison