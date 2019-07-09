--TEST--
spaceship '<=>' operator with strict_operators
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a <=> $b', function($a, $b) { return $a <=> $b; });

--EXPECT--
false <=> false = 0
false <=> true = -1
false <=> 0 - TypeError Operand type mismatch bool and int for comparison operator
false <=> 10 - TypeError Operand type mismatch bool and int for comparison operator
false <=> 0.0 - TypeError Operand type mismatch bool and float for comparison operator
false <=> 10.0 - TypeError Operand type mismatch bool and float for comparison operator
false <=> 3.14 - TypeError Operand type mismatch bool and float for comparison operator
false <=> '0' - TypeError Operand type mismatch bool and string for comparison operator
false <=> '10' - TypeError Operand type mismatch bool and string for comparison operator
false <=> '010' - TypeError Operand type mismatch bool and string for comparison operator
false <=> '10 elephants' - TypeError Operand type mismatch bool and string for comparison operator
false <=> 'foo' - TypeError Operand type mismatch bool and string for comparison operator
false <=> array ( ) - TypeError Unsupported operand type array for comparison operator
false <=> array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison operator
false <=> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison operator
false <=> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison operator
false <=> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison operator
false <=> (object) array ( ) - TypeError Unsupported operand type object for comparison operator
false <=> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison operator
false <=> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison operator
false <=> DateTime - TypeError Unsupported operand type object for comparison operator
false <=> resource - TypeError Unsupported operand type resource for comparison operator
false <=> NULL - TypeError Unsupported operand type null for comparison operator
true <=> false = 1
true <=> true = 0
true <=> 0 - TypeError Operand type mismatch bool and int for comparison operator
true <=> 10 - TypeError Operand type mismatch bool and int for comparison operator
true <=> 0.0 - TypeError Operand type mismatch bool and float for comparison operator
true <=> 10.0 - TypeError Operand type mismatch bool and float for comparison operator
true <=> 3.14 - TypeError Operand type mismatch bool and float for comparison operator
true <=> '0' - TypeError Operand type mismatch bool and string for comparison operator
true <=> '10' - TypeError Operand type mismatch bool and string for comparison operator
true <=> '010' - TypeError Operand type mismatch bool and string for comparison operator
true <=> '10 elephants' - TypeError Operand type mismatch bool and string for comparison operator
true <=> 'foo' - TypeError Operand type mismatch bool and string for comparison operator
true <=> array ( ) - TypeError Unsupported operand type array for comparison operator
true <=> array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison operator
true <=> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison operator
true <=> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison operator
true <=> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison operator
true <=> (object) array ( ) - TypeError Unsupported operand type object for comparison operator
true <=> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison operator
true <=> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison operator
true <=> DateTime - TypeError Unsupported operand type object for comparison operator
true <=> resource - TypeError Unsupported operand type resource for comparison operator
true <=> NULL - TypeError Unsupported operand type null for comparison operator
0 <=> false - TypeError Operand type mismatch int and bool for comparison operator
0 <=> true - TypeError Operand type mismatch int and bool for comparison operator
0 <=> 0 = 0
0 <=> 10 = -1
0 <=> 0.0 = 0
0 <=> 10.0 = -1
0 <=> 3.14 = -1
0 <=> '0' - TypeError Operand type mismatch int and string for comparison operator
0 <=> '10' - TypeError Operand type mismatch int and string for comparison operator
0 <=> '010' - TypeError Operand type mismatch int and string for comparison operator
0 <=> '10 elephants' - TypeError Operand type mismatch int and string for comparison operator
0 <=> 'foo' - TypeError Operand type mismatch int and string for comparison operator
0 <=> array ( ) - TypeError Unsupported operand type array for comparison operator
0 <=> array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison operator
0 <=> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison operator
0 <=> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison operator
0 <=> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison operator
0 <=> (object) array ( ) - TypeError Unsupported operand type object for comparison operator
0 <=> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison operator
0 <=> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison operator
0 <=> DateTime - TypeError Unsupported operand type object for comparison operator
0 <=> resource - TypeError Unsupported operand type resource for comparison operator
0 <=> NULL - TypeError Unsupported operand type null for comparison operator
10 <=> false - TypeError Operand type mismatch int and bool for comparison operator
10 <=> true - TypeError Operand type mismatch int and bool for comparison operator
10 <=> 0 = 1
10 <=> 10 = 0
10 <=> 0.0 = 1
10 <=> 10.0 = 0
10 <=> 3.14 = 1
10 <=> '0' - TypeError Operand type mismatch int and string for comparison operator
10 <=> '10' - TypeError Operand type mismatch int and string for comparison operator
10 <=> '010' - TypeError Operand type mismatch int and string for comparison operator
10 <=> '10 elephants' - TypeError Operand type mismatch int and string for comparison operator
10 <=> 'foo' - TypeError Operand type mismatch int and string for comparison operator
10 <=> array ( ) - TypeError Unsupported operand type array for comparison operator
10 <=> array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison operator
10 <=> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison operator
10 <=> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison operator
10 <=> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison operator
10 <=> (object) array ( ) - TypeError Unsupported operand type object for comparison operator
10 <=> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison operator
10 <=> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison operator
10 <=> DateTime - TypeError Unsupported operand type object for comparison operator
10 <=> resource - TypeError Unsupported operand type resource for comparison operator
10 <=> NULL - TypeError Unsupported operand type null for comparison operator
0.0 <=> false - TypeError Operand type mismatch float and bool for comparison operator
0.0 <=> true - TypeError Operand type mismatch float and bool for comparison operator
0.0 <=> 0 = 0
0.0 <=> 10 = -1
0.0 <=> 0.0 = 0
0.0 <=> 10.0 = -1
0.0 <=> 3.14 = -1
0.0 <=> '0' - TypeError Operand type mismatch float and string for comparison operator
0.0 <=> '10' - TypeError Operand type mismatch float and string for comparison operator
0.0 <=> '010' - TypeError Operand type mismatch float and string for comparison operator
0.0 <=> '10 elephants' - TypeError Operand type mismatch float and string for comparison operator
0.0 <=> 'foo' - TypeError Operand type mismatch float and string for comparison operator
0.0 <=> array ( ) - TypeError Unsupported operand type array for comparison operator
0.0 <=> array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison operator
0.0 <=> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison operator
0.0 <=> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison operator
0.0 <=> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison operator
0.0 <=> (object) array ( ) - TypeError Unsupported operand type object for comparison operator
0.0 <=> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison operator
0.0 <=> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison operator
0.0 <=> DateTime - TypeError Unsupported operand type object for comparison operator
0.0 <=> resource - TypeError Unsupported operand type resource for comparison operator
0.0 <=> NULL - TypeError Unsupported operand type null for comparison operator
10.0 <=> false - TypeError Operand type mismatch float and bool for comparison operator
10.0 <=> true - TypeError Operand type mismatch float and bool for comparison operator
10.0 <=> 0 = 1
10.0 <=> 10 = 0
10.0 <=> 0.0 = 1
10.0 <=> 10.0 = 0
10.0 <=> 3.14 = 1
10.0 <=> '0' - TypeError Operand type mismatch float and string for comparison operator
10.0 <=> '10' - TypeError Operand type mismatch float and string for comparison operator
10.0 <=> '010' - TypeError Operand type mismatch float and string for comparison operator
10.0 <=> '10 elephants' - TypeError Operand type mismatch float and string for comparison operator
10.0 <=> 'foo' - TypeError Operand type mismatch float and string for comparison operator
10.0 <=> array ( ) - TypeError Unsupported operand type array for comparison operator
10.0 <=> array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison operator
10.0 <=> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison operator
10.0 <=> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison operator
10.0 <=> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison operator
10.0 <=> (object) array ( ) - TypeError Unsupported operand type object for comparison operator
10.0 <=> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison operator
10.0 <=> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison operator
10.0 <=> DateTime - TypeError Unsupported operand type object for comparison operator
10.0 <=> resource - TypeError Unsupported operand type resource for comparison operator
10.0 <=> NULL - TypeError Unsupported operand type null for comparison operator
3.14 <=> false - TypeError Operand type mismatch float and bool for comparison operator
3.14 <=> true - TypeError Operand type mismatch float and bool for comparison operator
3.14 <=> 0 = 1
3.14 <=> 10 = -1
3.14 <=> 0.0 = 1
3.14 <=> 10.0 = -1
3.14 <=> 3.14 = 0
3.14 <=> '0' - TypeError Operand type mismatch float and string for comparison operator
3.14 <=> '10' - TypeError Operand type mismatch float and string for comparison operator
3.14 <=> '010' - TypeError Operand type mismatch float and string for comparison operator
3.14 <=> '10 elephants' - TypeError Operand type mismatch float and string for comparison operator
3.14 <=> 'foo' - TypeError Operand type mismatch float and string for comparison operator
3.14 <=> array ( ) - TypeError Unsupported operand type array for comparison operator
3.14 <=> array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison operator
3.14 <=> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison operator
3.14 <=> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison operator
3.14 <=> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison operator
3.14 <=> (object) array ( ) - TypeError Unsupported operand type object for comparison operator
3.14 <=> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison operator
3.14 <=> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison operator
3.14 <=> DateTime - TypeError Unsupported operand type object for comparison operator
3.14 <=> resource - TypeError Unsupported operand type resource for comparison operator
3.14 <=> NULL - TypeError Unsupported operand type null for comparison operator
'0' <=> false - TypeError Operand type mismatch string and bool for comparison operator
'0' <=> true - TypeError Operand type mismatch string and bool for comparison operator
'0' <=> 0 - TypeError Operand type mismatch string and int for comparison operator
'0' <=> 10 - TypeError Operand type mismatch string and int for comparison operator
'0' <=> 0.0 - TypeError Operand type mismatch string and float for comparison operator
'0' <=> 10.0 - TypeError Operand type mismatch string and float for comparison operator
'0' <=> 3.14 - TypeError Operand type mismatch string and float for comparison operator
'0' <=> '0' = 0
'0' <=> '10' = -1
'0' <=> '010' = -2
'0' <=> '10 elephants' = -1
'0' <=> 'foo' = -54
'0' <=> array ( ) - TypeError Unsupported operand type array for comparison operator
'0' <=> array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison operator
'0' <=> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison operator
'0' <=> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison operator
'0' <=> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison operator
'0' <=> (object) array ( ) - TypeError Unsupported operand type object for comparison operator
'0' <=> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison operator
'0' <=> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison operator
'0' <=> DateTime - TypeError Unsupported operand type object for comparison operator
'0' <=> resource - TypeError Unsupported operand type resource for comparison operator
'0' <=> NULL - TypeError Unsupported operand type null for comparison operator
'10' <=> false - TypeError Operand type mismatch string and bool for comparison operator
'10' <=> true - TypeError Operand type mismatch string and bool for comparison operator
'10' <=> 0 - TypeError Operand type mismatch string and int for comparison operator
'10' <=> 10 - TypeError Operand type mismatch string and int for comparison operator
'10' <=> 0.0 - TypeError Operand type mismatch string and float for comparison operator
'10' <=> 10.0 - TypeError Operand type mismatch string and float for comparison operator
'10' <=> 3.14 - TypeError Operand type mismatch string and float for comparison operator
'10' <=> '0' = 1
'10' <=> '10' = 0
'10' <=> '010' = 65279
'10' <=> '10 elephants' = -10
'10' <=> 'foo' = -3489599
'10' <=> array ( ) - TypeError Unsupported operand type array for comparison operator
'10' <=> array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison operator
'10' <=> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison operator
'10' <=> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison operator
'10' <=> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison operator
'10' <=> (object) array ( ) - TypeError Unsupported operand type object for comparison operator
'10' <=> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison operator
'10' <=> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison operator
'10' <=> DateTime - TypeError Unsupported operand type object for comparison operator
'10' <=> resource - TypeError Unsupported operand type resource for comparison operator
'10' <=> NULL - TypeError Unsupported operand type null for comparison operator
'010' <=> false - TypeError Operand type mismatch string and bool for comparison operator
'010' <=> true - TypeError Operand type mismatch string and bool for comparison operator
'010' <=> 0 - TypeError Operand type mismatch string and int for comparison operator
'010' <=> 10 - TypeError Operand type mismatch string and int for comparison operator
'010' <=> 0.0 - TypeError Operand type mismatch string and float for comparison operator
'010' <=> 10.0 - TypeError Operand type mismatch string and float for comparison operator
'010' <=> 3.14 - TypeError Operand type mismatch string and float for comparison operator
'010' <=> '0' = 2
'010' <=> '10' = -65279
'010' <=> '010' = 0
'010' <=> '10 elephants' = -65264
'010' <=> 'foo' = -3554879
'010' <=> array ( ) - TypeError Unsupported operand type array for comparison operator
'010' <=> array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison operator
'010' <=> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison operator
'010' <=> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison operator
'010' <=> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison operator
'010' <=> (object) array ( ) - TypeError Unsupported operand type object for comparison operator
'010' <=> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison operator
'010' <=> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison operator
'010' <=> DateTime - TypeError Unsupported operand type object for comparison operator
'010' <=> resource - TypeError Unsupported operand type resource for comparison operator
'010' <=> NULL - TypeError Unsupported operand type null for comparison operator
'10 elephants' <=> false - TypeError Operand type mismatch string and bool for comparison operator
'10 elephants' <=> true - TypeError Operand type mismatch string and bool for comparison operator
'10 elephants' <=> 0 - TypeError Operand type mismatch string and int for comparison operator
'10 elephants' <=> 10 - TypeError Operand type mismatch string and int for comparison operator
'10 elephants' <=> 0.0 - TypeError Operand type mismatch string and float for comparison operator
'10 elephants' <=> 10.0 - TypeError Operand type mismatch string and float for comparison operator
'10 elephants' <=> 3.14 - TypeError Operand type mismatch string and float for comparison operator
'10 elephants' <=> '0' = 1
'10 elephants' <=> '10' = 10
'10 elephants' <=> '010' = 65264
'10 elephants' <=> '10 elephants' = 0
'10 elephants' <=> 'foo' = -3489615
'10 elephants' <=> array ( ) - TypeError Unsupported operand type array for comparison operator
'10 elephants' <=> array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison operator
'10 elephants' <=> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison operator
'10 elephants' <=> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison operator
'10 elephants' <=> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison operator
'10 elephants' <=> (object) array ( ) - TypeError Unsupported operand type object for comparison operator
'10 elephants' <=> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison operator
'10 elephants' <=> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison operator
'10 elephants' <=> DateTime - TypeError Unsupported operand type object for comparison operator
'10 elephants' <=> resource - TypeError Unsupported operand type resource for comparison operator
'10 elephants' <=> NULL - TypeError Unsupported operand type null for comparison operator
'foo' <=> false - TypeError Operand type mismatch string and bool for comparison operator
'foo' <=> true - TypeError Operand type mismatch string and bool for comparison operator
'foo' <=> 0 - TypeError Operand type mismatch string and int for comparison operator
'foo' <=> 10 - TypeError Operand type mismatch string and int for comparison operator
'foo' <=> 0.0 - TypeError Operand type mismatch string and float for comparison operator
'foo' <=> 10.0 - TypeError Operand type mismatch string and float for comparison operator
'foo' <=> 3.14 - TypeError Operand type mismatch string and float for comparison operator
'foo' <=> '0' = 54
'foo' <=> '10' = 3489599
'foo' <=> '010' = 3554879
'foo' <=> '10 elephants' = 3489615
'foo' <=> 'foo' = 0
'foo' <=> array ( ) - TypeError Unsupported operand type array for comparison operator
'foo' <=> array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison operator
'foo' <=> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison operator
'foo' <=> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison operator
'foo' <=> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison operator
'foo' <=> (object) array ( ) - TypeError Unsupported operand type object for comparison operator
'foo' <=> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison operator
'foo' <=> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison operator
'foo' <=> DateTime - TypeError Unsupported operand type object for comparison operator
'foo' <=> resource - TypeError Unsupported operand type resource for comparison operator
'foo' <=> NULL - TypeError Unsupported operand type null for comparison operator
array ( ) <=> false - TypeError Unsupported operand type array for comparison operator
array ( ) <=> true - TypeError Unsupported operand type array for comparison operator
array ( ) <=> 0 - TypeError Unsupported operand type array for comparison operator
array ( ) <=> 10 - TypeError Unsupported operand type array for comparison operator
array ( ) <=> 0.0 - TypeError Unsupported operand type array for comparison operator
array ( ) <=> 10.0 - TypeError Unsupported operand type array for comparison operator
array ( ) <=> 3.14 - TypeError Unsupported operand type array for comparison operator
array ( ) <=> '0' - TypeError Unsupported operand type array for comparison operator
array ( ) <=> '10' - TypeError Unsupported operand type array for comparison operator
array ( ) <=> '010' - TypeError Unsupported operand type array for comparison operator
array ( ) <=> '10 elephants' - TypeError Unsupported operand type array for comparison operator
array ( ) <=> 'foo' - TypeError Unsupported operand type array for comparison operator
array ( ) <=> array ( ) - TypeError Unsupported operand type array for comparison operator
array ( ) <=> array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison operator
array ( ) <=> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison operator
array ( ) <=> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison operator
array ( ) <=> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison operator
array ( ) <=> (object) array ( ) - TypeError Unsupported operand type array for comparison operator
array ( ) <=> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison operator
array ( ) <=> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison operator
array ( ) <=> DateTime - TypeError Unsupported operand type array for comparison operator
array ( ) <=> resource - TypeError Unsupported operand type array for comparison operator
array ( ) <=> NULL - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1 ) <=> false - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1 ) <=> true - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1 ) <=> 0 - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1 ) <=> 10 - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1 ) <=> 0.0 - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1 ) <=> 10.0 - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1 ) <=> 3.14 - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1 ) <=> '0' - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1 ) <=> '10' - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1 ) <=> '010' - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1 ) <=> '10 elephants' - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1 ) <=> 'foo' - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1 ) <=> array ( ) - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1 ) <=> array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1 ) <=> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1 ) <=> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1 ) <=> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1 ) <=> (object) array ( ) - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1 ) <=> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1 ) <=> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1 ) <=> DateTime - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1 ) <=> resource - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1 ) <=> NULL - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1, 1 => 100 ) <=> false - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1, 1 => 100 ) <=> true - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1, 1 => 100 ) <=> 0 - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1, 1 => 100 ) <=> 10 - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1, 1 => 100 ) <=> 0.0 - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1, 1 => 100 ) <=> 10.0 - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1, 1 => 100 ) <=> 3.14 - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1, 1 => 100 ) <=> '0' - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1, 1 => 100 ) <=> '10' - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1, 1 => 100 ) <=> '010' - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1, 1 => 100 ) <=> '10 elephants' - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1, 1 => 100 ) <=> 'foo' - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1, 1 => 100 ) <=> array ( ) - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1, 1 => 100 ) <=> array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1, 1 => 100 ) <=> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1, 1 => 100 ) <=> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1, 1 => 100 ) <=> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1, 1 => 100 ) <=> (object) array ( ) - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1, 1 => 100 ) <=> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1, 1 => 100 ) <=> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1, 1 => 100 ) <=> DateTime - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1, 1 => 100 ) <=> resource - TypeError Unsupported operand type array for comparison operator
array ( 0 => 1, 1 => 100 ) <=> NULL - TypeError Unsupported operand type array for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) <=> false - TypeError Unsupported operand type array for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) <=> true - TypeError Unsupported operand type array for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) <=> 0 - TypeError Unsupported operand type array for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) <=> 10 - TypeError Unsupported operand type array for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) <=> 0.0 - TypeError Unsupported operand type array for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) <=> 10.0 - TypeError Unsupported operand type array for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) <=> 3.14 - TypeError Unsupported operand type array for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) <=> '0' - TypeError Unsupported operand type array for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) <=> '10' - TypeError Unsupported operand type array for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) <=> '010' - TypeError Unsupported operand type array for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) <=> '10 elephants' - TypeError Unsupported operand type array for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) <=> 'foo' - TypeError Unsupported operand type array for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) <=> array ( ) - TypeError Unsupported operand type array for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) <=> array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) <=> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) <=> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) <=> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) <=> (object) array ( ) - TypeError Unsupported operand type array for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) <=> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) <=> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) <=> DateTime - TypeError Unsupported operand type array for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) <=> resource - TypeError Unsupported operand type array for comparison operator
array ( 'foo' => 1, 'bar' => 2 ) <=> NULL - TypeError Unsupported operand type array for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) <=> false - TypeError Unsupported operand type array for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) <=> true - TypeError Unsupported operand type array for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) <=> 0 - TypeError Unsupported operand type array for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) <=> 10 - TypeError Unsupported operand type array for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) <=> 0.0 - TypeError Unsupported operand type array for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) <=> 10.0 - TypeError Unsupported operand type array for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) <=> 3.14 - TypeError Unsupported operand type array for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) <=> '0' - TypeError Unsupported operand type array for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) <=> '10' - TypeError Unsupported operand type array for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) <=> '010' - TypeError Unsupported operand type array for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) <=> '10 elephants' - TypeError Unsupported operand type array for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) <=> 'foo' - TypeError Unsupported operand type array for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) <=> array ( ) - TypeError Unsupported operand type array for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) <=> array ( 0 => 1 ) - TypeError Unsupported operand type array for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) <=> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) <=> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) <=> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) <=> (object) array ( ) - TypeError Unsupported operand type array for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) <=> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) <=> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) <=> DateTime - TypeError Unsupported operand type array for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) <=> resource - TypeError Unsupported operand type array for comparison operator
array ( 'bar' => 1, 'foo' => 2 ) <=> NULL - TypeError Unsupported operand type array for comparison operator
(object) array ( ) <=> false - TypeError Unsupported operand type object for comparison operator
(object) array ( ) <=> true - TypeError Unsupported operand type object for comparison operator
(object) array ( ) <=> 0 - TypeError Unsupported operand type object for comparison operator
(object) array ( ) <=> 10 - TypeError Unsupported operand type object for comparison operator
(object) array ( ) <=> 0.0 - TypeError Unsupported operand type object for comparison operator
(object) array ( ) <=> 10.0 - TypeError Unsupported operand type object for comparison operator
(object) array ( ) <=> 3.14 - TypeError Unsupported operand type object for comparison operator
(object) array ( ) <=> '0' - TypeError Unsupported operand type object for comparison operator
(object) array ( ) <=> '10' - TypeError Unsupported operand type object for comparison operator
(object) array ( ) <=> '010' - TypeError Unsupported operand type object for comparison operator
(object) array ( ) <=> '10 elephants' - TypeError Unsupported operand type object for comparison operator
(object) array ( ) <=> 'foo' - TypeError Unsupported operand type object for comparison operator
(object) array ( ) <=> array ( ) - TypeError Unsupported operand type object for comparison operator
(object) array ( ) <=> array ( 0 => 1 ) - TypeError Unsupported operand type object for comparison operator
(object) array ( ) <=> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for comparison operator
(object) array ( ) <=> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison operator
(object) array ( ) <=> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison operator
(object) array ( ) <=> (object) array ( ) - TypeError Unsupported operand type object for comparison operator
(object) array ( ) <=> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison operator
(object) array ( ) <=> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison operator
(object) array ( ) <=> DateTime - TypeError Unsupported operand type object for comparison operator
(object) array ( ) <=> resource - TypeError Unsupported operand type object for comparison operator
(object) array ( ) <=> NULL - TypeError Unsupported operand type object for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> false - TypeError Unsupported operand type object for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> true - TypeError Unsupported operand type object for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> 0 - TypeError Unsupported operand type object for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> 10 - TypeError Unsupported operand type object for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> 0.0 - TypeError Unsupported operand type object for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> 10.0 - TypeError Unsupported operand type object for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> 3.14 - TypeError Unsupported operand type object for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> '0' - TypeError Unsupported operand type object for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> '10' - TypeError Unsupported operand type object for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> '010' - TypeError Unsupported operand type object for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> '10 elephants' - TypeError Unsupported operand type object for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> 'foo' - TypeError Unsupported operand type object for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> array ( ) - TypeError Unsupported operand type object for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> array ( 0 => 1 ) - TypeError Unsupported operand type object for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> (object) array ( ) - TypeError Unsupported operand type object for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> DateTime - TypeError Unsupported operand type object for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> resource - TypeError Unsupported operand type object for comparison operator
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> NULL - TypeError Unsupported operand type object for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> false - TypeError Unsupported operand type object for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> true - TypeError Unsupported operand type object for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> 0 - TypeError Unsupported operand type object for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> 10 - TypeError Unsupported operand type object for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> 0.0 - TypeError Unsupported operand type object for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> 10.0 - TypeError Unsupported operand type object for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> 3.14 - TypeError Unsupported operand type object for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> '0' - TypeError Unsupported operand type object for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> '10' - TypeError Unsupported operand type object for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> '010' - TypeError Unsupported operand type object for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> '10 elephants' - TypeError Unsupported operand type object for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> 'foo' - TypeError Unsupported operand type object for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> array ( ) - TypeError Unsupported operand type object for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> array ( 0 => 1 ) - TypeError Unsupported operand type object for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> (object) array ( ) - TypeError Unsupported operand type object for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> DateTime - TypeError Unsupported operand type object for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> resource - TypeError Unsupported operand type object for comparison operator
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> NULL - TypeError Unsupported operand type object for comparison operator
DateTime <=> false - TypeError Unsupported operand type object for comparison operator
DateTime <=> true - TypeError Unsupported operand type object for comparison operator
DateTime <=> 0 - TypeError Unsupported operand type object for comparison operator
DateTime <=> 10 - TypeError Unsupported operand type object for comparison operator
DateTime <=> 0.0 - TypeError Unsupported operand type object for comparison operator
DateTime <=> 10.0 - TypeError Unsupported operand type object for comparison operator
DateTime <=> 3.14 - TypeError Unsupported operand type object for comparison operator
DateTime <=> '0' - TypeError Unsupported operand type object for comparison operator
DateTime <=> '10' - TypeError Unsupported operand type object for comparison operator
DateTime <=> '010' - TypeError Unsupported operand type object for comparison operator
DateTime <=> '10 elephants' - TypeError Unsupported operand type object for comparison operator
DateTime <=> 'foo' - TypeError Unsupported operand type object for comparison operator
DateTime <=> array ( ) - TypeError Unsupported operand type object for comparison operator
DateTime <=> array ( 0 => 1 ) - TypeError Unsupported operand type object for comparison operator
DateTime <=> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for comparison operator
DateTime <=> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison operator
DateTime <=> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison operator
DateTime <=> (object) array ( ) - TypeError Unsupported operand type object for comparison operator
DateTime <=> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for comparison operator
DateTime <=> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for comparison operator
DateTime <=> DateTime - TypeError Unsupported operand type object for comparison operator
DateTime <=> resource - TypeError Unsupported operand type object for comparison operator
DateTime <=> NULL - TypeError Unsupported operand type object for comparison operator
resource <=> false - TypeError Unsupported operand type resource for comparison operator
resource <=> true - TypeError Unsupported operand type resource for comparison operator
resource <=> 0 - TypeError Unsupported operand type resource for comparison operator
resource <=> 10 - TypeError Unsupported operand type resource for comparison operator
resource <=> 0.0 - TypeError Unsupported operand type resource for comparison operator
resource <=> 10.0 - TypeError Unsupported operand type resource for comparison operator
resource <=> 3.14 - TypeError Unsupported operand type resource for comparison operator
resource <=> '0' - TypeError Unsupported operand type resource for comparison operator
resource <=> '10' - TypeError Unsupported operand type resource for comparison operator
resource <=> '010' - TypeError Unsupported operand type resource for comparison operator
resource <=> '10 elephants' - TypeError Unsupported operand type resource for comparison operator
resource <=> 'foo' - TypeError Unsupported operand type resource for comparison operator
resource <=> array ( ) - TypeError Unsupported operand type resource for comparison operator
resource <=> array ( 0 => 1 ) - TypeError Unsupported operand type resource for comparison operator
resource <=> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type resource for comparison operator
resource <=> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for comparison operator
resource <=> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for comparison operator
resource <=> (object) array ( ) - TypeError Unsupported operand type resource for comparison operator
resource <=> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for comparison operator
resource <=> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for comparison operator
resource <=> DateTime - TypeError Unsupported operand type resource for comparison operator
resource <=> resource - TypeError Unsupported operand type resource for comparison operator
resource <=> NULL - TypeError Unsupported operand type resource for comparison operator
NULL <=> false - TypeError Unsupported operand type null for comparison operator
NULL <=> true - TypeError Unsupported operand type null for comparison operator
NULL <=> 0 - TypeError Unsupported operand type null for comparison operator
NULL <=> 10 - TypeError Unsupported operand type null for comparison operator
NULL <=> 0.0 - TypeError Unsupported operand type null for comparison operator
NULL <=> 10.0 - TypeError Unsupported operand type null for comparison operator
NULL <=> 3.14 - TypeError Unsupported operand type null for comparison operator
NULL <=> '0' - TypeError Unsupported operand type null for comparison operator
NULL <=> '10' - TypeError Unsupported operand type null for comparison operator
NULL <=> '010' - TypeError Unsupported operand type null for comparison operator
NULL <=> '10 elephants' - TypeError Unsupported operand type null for comparison operator
NULL <=> 'foo' - TypeError Unsupported operand type null for comparison operator
NULL <=> array ( ) - TypeError Unsupported operand type null for comparison operator
NULL <=> array ( 0 => 1 ) - TypeError Unsupported operand type null for comparison operator
NULL <=> array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type null for comparison operator
NULL <=> array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for comparison operator
NULL <=> array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for comparison operator
NULL <=> (object) array ( ) - TypeError Unsupported operand type null for comparison operator
NULL <=> (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for comparison operator
NULL <=> (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for comparison operator
NULL <=> DateTime - TypeError Unsupported operand type null for comparison operator
NULL <=> resource - TypeError Unsupported operand type null for comparison operator
NULL <=> NULL - TypeError Unsupported operand type null for comparison operator