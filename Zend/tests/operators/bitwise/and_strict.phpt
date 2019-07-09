--TEST--
bitwise and operator with strict operators
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a & $b', function($a, $b) { return $a & $b; }, 'var_out_base64');

--EXPECT--
false & false - TypeError Unsupported operand type bool for bitwise and
false & true - TypeError Unsupported operand type bool for bitwise and
false & 0 - TypeError Unsupported operand type bool for bitwise and
false & 10 - TypeError Unsupported operand type bool for bitwise and
false & 0.0 - TypeError Unsupported operand type bool for bitwise and
false & 10.0 - TypeError Unsupported operand type bool for bitwise and
false & 3.14 - TypeError Unsupported operand type bool for bitwise and
false & '0' - TypeError Unsupported operand type bool for bitwise and
false & '10' - TypeError Unsupported operand type bool for bitwise and
false & '010' - TypeError Unsupported operand type bool for bitwise and
false & '10 elephants' - TypeError Unsupported operand type bool for bitwise and
false & 'foo' - TypeError Unsupported operand type bool for bitwise and
false & array ( ) - TypeError Unsupported operand type bool for bitwise and
false & array ( 0 => 1 ) - TypeError Unsupported operand type bool for bitwise and
false & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for bitwise and
false & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for bitwise and
false & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for bitwise and
false & (object) array ( ) - TypeError Unsupported operand type bool for bitwise and
false & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for bitwise and
false & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for bitwise and
false & DateTime - TypeError Unsupported operand type bool for bitwise and
false & resource - TypeError Unsupported operand type bool for bitwise and
false & NULL - TypeError Unsupported operand type bool for bitwise and
true & false - TypeError Unsupported operand type bool for bitwise and
true & true - TypeError Unsupported operand type bool for bitwise and
true & 0 - TypeError Unsupported operand type bool for bitwise and
true & 10 - TypeError Unsupported operand type bool for bitwise and
true & 0.0 - TypeError Unsupported operand type bool for bitwise and
true & 10.0 - TypeError Unsupported operand type bool for bitwise and
true & 3.14 - TypeError Unsupported operand type bool for bitwise and
true & '0' - TypeError Unsupported operand type bool for bitwise and
true & '10' - TypeError Unsupported operand type bool for bitwise and
true & '010' - TypeError Unsupported operand type bool for bitwise and
true & '10 elephants' - TypeError Unsupported operand type bool for bitwise and
true & 'foo' - TypeError Unsupported operand type bool for bitwise and
true & array ( ) - TypeError Unsupported operand type bool for bitwise and
true & array ( 0 => 1 ) - TypeError Unsupported operand type bool for bitwise and
true & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for bitwise and
true & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for bitwise and
true & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for bitwise and
true & (object) array ( ) - TypeError Unsupported operand type bool for bitwise and
true & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for bitwise and
true & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for bitwise and
true & DateTime - TypeError Unsupported operand type bool for bitwise and
true & resource - TypeError Unsupported operand type bool for bitwise and
true & NULL - TypeError Unsupported operand type bool for bitwise and
0 & false - TypeError Unsupported operand type bool for bitwise and
0 & true - TypeError Unsupported operand type bool for bitwise and
0 & 0 = 0
0 & 10 = 0
0 & 0.0 - TypeError Unsupported operand type float for bitwise and
0 & 10.0 - TypeError Unsupported operand type float for bitwise and
0 & 3.14 - TypeError Unsupported operand type float for bitwise and
0 & '0' - TypeError Operand type mismatch int and string for bitwise and
0 & '10' - TypeError Operand type mismatch int and string for bitwise and
0 & '010' - TypeError Operand type mismatch int and string for bitwise and
0 & '10 elephants' - TypeError Operand type mismatch int and string for bitwise and
0 & 'foo' - TypeError Operand type mismatch int and string for bitwise and
0 & array ( ) - TypeError Unsupported operand type array for bitwise and
0 & array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise and
0 & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise and
0 & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise and
0 & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise and
0 & (object) array ( ) - TypeError Unsupported operand type object for bitwise and
0 & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise and
0 & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise and
0 & DateTime - TypeError Unsupported operand type object for bitwise and
0 & resource - TypeError Unsupported operand type resource for bitwise and
0 & NULL - TypeError Unsupported operand type null for bitwise and
10 & false - TypeError Unsupported operand type bool for bitwise and
10 & true - TypeError Unsupported operand type bool for bitwise and
10 & 0 = 0
10 & 10 = 10
10 & 0.0 - TypeError Unsupported operand type float for bitwise and
10 & 10.0 - TypeError Unsupported operand type float for bitwise and
10 & 3.14 - TypeError Unsupported operand type float for bitwise and
10 & '0' - TypeError Operand type mismatch int and string for bitwise and
10 & '10' - TypeError Operand type mismatch int and string for bitwise and
10 & '010' - TypeError Operand type mismatch int and string for bitwise and
10 & '10 elephants' - TypeError Operand type mismatch int and string for bitwise and
10 & 'foo' - TypeError Operand type mismatch int and string for bitwise and
10 & array ( ) - TypeError Unsupported operand type array for bitwise and
10 & array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise and
10 & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise and
10 & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise and
10 & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise and
10 & (object) array ( ) - TypeError Unsupported operand type object for bitwise and
10 & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise and
10 & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise and
10 & DateTime - TypeError Unsupported operand type object for bitwise and
10 & resource - TypeError Unsupported operand type resource for bitwise and
10 & NULL - TypeError Unsupported operand type null for bitwise and
0.0 & false - TypeError Unsupported operand type float for bitwise and
0.0 & true - TypeError Unsupported operand type float for bitwise and
0.0 & 0 - TypeError Unsupported operand type float for bitwise and
0.0 & 10 - TypeError Unsupported operand type float for bitwise and
0.0 & 0.0 - TypeError Unsupported operand type float for bitwise and
0.0 & 10.0 - TypeError Unsupported operand type float for bitwise and
0.0 & 3.14 - TypeError Unsupported operand type float for bitwise and
0.0 & '0' - TypeError Unsupported operand type float for bitwise and
0.0 & '10' - TypeError Unsupported operand type float for bitwise and
0.0 & '010' - TypeError Unsupported operand type float for bitwise and
0.0 & '10 elephants' - TypeError Unsupported operand type float for bitwise and
0.0 & 'foo' - TypeError Unsupported operand type float for bitwise and
0.0 & array ( ) - TypeError Unsupported operand type float for bitwise and
0.0 & array ( 0 => 1 ) - TypeError Unsupported operand type float for bitwise and
0.0 & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type float for bitwise and
0.0 & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bitwise and
0.0 & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bitwise and
0.0 & (object) array ( ) - TypeError Unsupported operand type float for bitwise and
0.0 & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bitwise and
0.0 & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bitwise and
0.0 & DateTime - TypeError Unsupported operand type float for bitwise and
0.0 & resource - TypeError Unsupported operand type float for bitwise and
0.0 & NULL - TypeError Unsupported operand type float for bitwise and
10.0 & false - TypeError Unsupported operand type float for bitwise and
10.0 & true - TypeError Unsupported operand type float for bitwise and
10.0 & 0 - TypeError Unsupported operand type float for bitwise and
10.0 & 10 - TypeError Unsupported operand type float for bitwise and
10.0 & 0.0 - TypeError Unsupported operand type float for bitwise and
10.0 & 10.0 - TypeError Unsupported operand type float for bitwise and
10.0 & 3.14 - TypeError Unsupported operand type float for bitwise and
10.0 & '0' - TypeError Unsupported operand type float for bitwise and
10.0 & '10' - TypeError Unsupported operand type float for bitwise and
10.0 & '010' - TypeError Unsupported operand type float for bitwise and
10.0 & '10 elephants' - TypeError Unsupported operand type float for bitwise and
10.0 & 'foo' - TypeError Unsupported operand type float for bitwise and
10.0 & array ( ) - TypeError Unsupported operand type float for bitwise and
10.0 & array ( 0 => 1 ) - TypeError Unsupported operand type float for bitwise and
10.0 & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type float for bitwise and
10.0 & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bitwise and
10.0 & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bitwise and
10.0 & (object) array ( ) - TypeError Unsupported operand type float for bitwise and
10.0 & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bitwise and
10.0 & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bitwise and
10.0 & DateTime - TypeError Unsupported operand type float for bitwise and
10.0 & resource - TypeError Unsupported operand type float for bitwise and
10.0 & NULL - TypeError Unsupported operand type float for bitwise and
3.14 & false - TypeError Unsupported operand type float for bitwise and
3.14 & true - TypeError Unsupported operand type float for bitwise and
3.14 & 0 - TypeError Unsupported operand type float for bitwise and
3.14 & 10 - TypeError Unsupported operand type float for bitwise and
3.14 & 0.0 - TypeError Unsupported operand type float for bitwise and
3.14 & 10.0 - TypeError Unsupported operand type float for bitwise and
3.14 & 3.14 - TypeError Unsupported operand type float for bitwise and
3.14 & '0' - TypeError Unsupported operand type float for bitwise and
3.14 & '10' - TypeError Unsupported operand type float for bitwise and
3.14 & '010' - TypeError Unsupported operand type float for bitwise and
3.14 & '10 elephants' - TypeError Unsupported operand type float for bitwise and
3.14 & 'foo' - TypeError Unsupported operand type float for bitwise and
3.14 & array ( ) - TypeError Unsupported operand type float for bitwise and
3.14 & array ( 0 => 1 ) - TypeError Unsupported operand type float for bitwise and
3.14 & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type float for bitwise and
3.14 & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bitwise and
3.14 & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bitwise and
3.14 & (object) array ( ) - TypeError Unsupported operand type float for bitwise and
3.14 & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bitwise and
3.14 & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bitwise and
3.14 & DateTime - TypeError Unsupported operand type float for bitwise and
3.14 & resource - TypeError Unsupported operand type float for bitwise and
3.14 & NULL - TypeError Unsupported operand type float for bitwise and
'0' & false - TypeError Unsupported operand type bool for bitwise and
'0' & true - TypeError Unsupported operand type bool for bitwise and
'0' & 0 - TypeError Operand type mismatch string and int for bitwise and
'0' & 10 - TypeError Operand type mismatch string and int for bitwise and
'0' & 0.0 - TypeError Unsupported operand type float for bitwise and
'0' & 10.0 - TypeError Unsupported operand type float for bitwise and
'0' & 3.14 - TypeError Unsupported operand type float for bitwise and
'0' & '0' = base64:MA==
'0' & '10' = base64:MA==
'0' & '010' = base64:MA==
'0' & '10 elephants' = base64:MA==
'0' & 'foo' = base64:IA==
'0' & array ( ) - TypeError Unsupported operand type array for bitwise and
'0' & array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise and
'0' & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise and
'0' & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise and
'0' & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise and
'0' & (object) array ( ) - TypeError Unsupported operand type object for bitwise and
'0' & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise and
'0' & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise and
'0' & DateTime - TypeError Unsupported operand type object for bitwise and
'0' & resource - TypeError Unsupported operand type resource for bitwise and
'0' & NULL - TypeError Unsupported operand type null for bitwise and
'10' & false - TypeError Unsupported operand type bool for bitwise and
'10' & true - TypeError Unsupported operand type bool for bitwise and
'10' & 0 - TypeError Operand type mismatch string and int for bitwise and
'10' & 10 - TypeError Operand type mismatch string and int for bitwise and
'10' & 0.0 - TypeError Unsupported operand type float for bitwise and
'10' & 10.0 - TypeError Unsupported operand type float for bitwise and
'10' & 3.14 - TypeError Unsupported operand type float for bitwise and
'10' & '0' = base64:MA==
'10' & '10' = base64:MTA=
'10' & '010' = base64:MDA=
'10' & '10 elephants' = base64:MTA=
'10' & 'foo' = base64:ICA=
'10' & array ( ) - TypeError Unsupported operand type array for bitwise and
'10' & array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise and
'10' & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise and
'10' & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise and
'10' & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise and
'10' & (object) array ( ) - TypeError Unsupported operand type object for bitwise and
'10' & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise and
'10' & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise and
'10' & DateTime - TypeError Unsupported operand type object for bitwise and
'10' & resource - TypeError Unsupported operand type resource for bitwise and
'10' & NULL - TypeError Unsupported operand type null for bitwise and
'010' & false - TypeError Unsupported operand type bool for bitwise and
'010' & true - TypeError Unsupported operand type bool for bitwise and
'010' & 0 - TypeError Operand type mismatch string and int for bitwise and
'010' & 10 - TypeError Operand type mismatch string and int for bitwise and
'010' & 0.0 - TypeError Unsupported operand type float for bitwise and
'010' & 10.0 - TypeError Unsupported operand type float for bitwise and
'010' & 3.14 - TypeError Unsupported operand type float for bitwise and
'010' & '0' = base64:MA==
'010' & '10' = base64:MDA=
'010' & '010' = base64:MDEw
'010' & '10 elephants' = base64:MDAg
'010' & 'foo' = base64:ICEg
'010' & array ( ) - TypeError Unsupported operand type array for bitwise and
'010' & array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise and
'010' & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise and
'010' & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise and
'010' & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise and
'010' & (object) array ( ) - TypeError Unsupported operand type object for bitwise and
'010' & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise and
'010' & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise and
'010' & DateTime - TypeError Unsupported operand type object for bitwise and
'010' & resource - TypeError Unsupported operand type resource for bitwise and
'010' & NULL - TypeError Unsupported operand type null for bitwise and
'10 elephants' & false - TypeError Unsupported operand type bool for bitwise and
'10 elephants' & true - TypeError Unsupported operand type bool for bitwise and
'10 elephants' & 0 - TypeError Operand type mismatch string and int for bitwise and
'10 elephants' & 10 - TypeError Operand type mismatch string and int for bitwise and
'10 elephants' & 0.0 - TypeError Unsupported operand type float for bitwise and
'10 elephants' & 10.0 - TypeError Unsupported operand type float for bitwise and
'10 elephants' & 3.14 - TypeError Unsupported operand type float for bitwise and
'10 elephants' & '0' = base64:MA==
'10 elephants' & '10' = base64:MTA=
'10 elephants' & '010' = base64:MDAg
'10 elephants' & '10 elephants' = base64:MTAgZWxlcGhhbnRz
'10 elephants' & 'foo' = base64:ICAg
'10 elephants' & array ( ) - TypeError Unsupported operand type array for bitwise and
'10 elephants' & array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise and
'10 elephants' & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise and
'10 elephants' & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise and
'10 elephants' & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise and
'10 elephants' & (object) array ( ) - TypeError Unsupported operand type object for bitwise and
'10 elephants' & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise and
'10 elephants' & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise and
'10 elephants' & DateTime - TypeError Unsupported operand type object for bitwise and
'10 elephants' & resource - TypeError Unsupported operand type resource for bitwise and
'10 elephants' & NULL - TypeError Unsupported operand type null for bitwise and
'foo' & false - TypeError Unsupported operand type bool for bitwise and
'foo' & true - TypeError Unsupported operand type bool for bitwise and
'foo' & 0 - TypeError Operand type mismatch string and int for bitwise and
'foo' & 10 - TypeError Operand type mismatch string and int for bitwise and
'foo' & 0.0 - TypeError Unsupported operand type float for bitwise and
'foo' & 10.0 - TypeError Unsupported operand type float for bitwise and
'foo' & 3.14 - TypeError Unsupported operand type float for bitwise and
'foo' & '0' = base64:IA==
'foo' & '10' = base64:ICA=
'foo' & '010' = base64:ICEg
'foo' & '10 elephants' = base64:ICAg
'foo' & 'foo' = base64:Zm9v
'foo' & array ( ) - TypeError Unsupported operand type array for bitwise and
'foo' & array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise and
'foo' & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise and
'foo' & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise and
'foo' & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise and
'foo' & (object) array ( ) - TypeError Unsupported operand type object for bitwise and
'foo' & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise and
'foo' & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise and
'foo' & DateTime - TypeError Unsupported operand type object for bitwise and
'foo' & resource - TypeError Unsupported operand type resource for bitwise and
'foo' & NULL - TypeError Unsupported operand type null for bitwise and
array ( ) & false - TypeError Unsupported operand type array for bitwise and
array ( ) & true - TypeError Unsupported operand type array for bitwise and
array ( ) & 0 - TypeError Unsupported operand type array for bitwise and
array ( ) & 10 - TypeError Unsupported operand type array for bitwise and
array ( ) & 0.0 - TypeError Unsupported operand type array for bitwise and
array ( ) & 10.0 - TypeError Unsupported operand type array for bitwise and
array ( ) & 3.14 - TypeError Unsupported operand type array for bitwise and
array ( ) & '0' - TypeError Unsupported operand type array for bitwise and
array ( ) & '10' - TypeError Unsupported operand type array for bitwise and
array ( ) & '010' - TypeError Unsupported operand type array for bitwise and
array ( ) & '10 elephants' - TypeError Unsupported operand type array for bitwise and
array ( ) & 'foo' - TypeError Unsupported operand type array for bitwise and
array ( ) & array ( ) - TypeError Unsupported operand type array for bitwise and
array ( ) & array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise and
array ( ) & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise and
array ( ) & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise and
array ( ) & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise and
array ( ) & (object) array ( ) - TypeError Unsupported operand type array for bitwise and
array ( ) & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise and
array ( ) & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise and
array ( ) & DateTime - TypeError Unsupported operand type array for bitwise and
array ( ) & resource - TypeError Unsupported operand type array for bitwise and
array ( ) & NULL - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1 ) & false - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1 ) & true - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1 ) & 0 - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1 ) & 10 - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1 ) & 0.0 - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1 ) & 10.0 - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1 ) & 3.14 - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1 ) & '0' - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1 ) & '10' - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1 ) & '010' - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1 ) & '10 elephants' - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1 ) & 'foo' - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1 ) & array ( ) - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1 ) & array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1 ) & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1 ) & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1 ) & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1 ) & (object) array ( ) - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1 ) & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1 ) & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1 ) & DateTime - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1 ) & resource - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1 ) & NULL - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1, 1 => 100 ) & false - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1, 1 => 100 ) & true - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1, 1 => 100 ) & 0 - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1, 1 => 100 ) & 10 - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1, 1 => 100 ) & 0.0 - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1, 1 => 100 ) & 10.0 - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1, 1 => 100 ) & 3.14 - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1, 1 => 100 ) & '0' - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1, 1 => 100 ) & '10' - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1, 1 => 100 ) & '010' - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1, 1 => 100 ) & '10 elephants' - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1, 1 => 100 ) & 'foo' - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1, 1 => 100 ) & array ( ) - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1, 1 => 100 ) & array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1, 1 => 100 ) & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1, 1 => 100 ) & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1, 1 => 100 ) & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1, 1 => 100 ) & (object) array ( ) - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1, 1 => 100 ) & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1, 1 => 100 ) & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1, 1 => 100 ) & DateTime - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1, 1 => 100 ) & resource - TypeError Unsupported operand type array for bitwise and
array ( 0 => 1, 1 => 100 ) & NULL - TypeError Unsupported operand type array for bitwise and
array ( 'foo' => 1, 'bar' => 2 ) & false - TypeError Unsupported operand type array for bitwise and
array ( 'foo' => 1, 'bar' => 2 ) & true - TypeError Unsupported operand type array for bitwise and
array ( 'foo' => 1, 'bar' => 2 ) & 0 - TypeError Unsupported operand type array for bitwise and
array ( 'foo' => 1, 'bar' => 2 ) & 10 - TypeError Unsupported operand type array for bitwise and
array ( 'foo' => 1, 'bar' => 2 ) & 0.0 - TypeError Unsupported operand type array for bitwise and
array ( 'foo' => 1, 'bar' => 2 ) & 10.0 - TypeError Unsupported operand type array for bitwise and
array ( 'foo' => 1, 'bar' => 2 ) & 3.14 - TypeError Unsupported operand type array for bitwise and
array ( 'foo' => 1, 'bar' => 2 ) & '0' - TypeError Unsupported operand type array for bitwise and
array ( 'foo' => 1, 'bar' => 2 ) & '10' - TypeError Unsupported operand type array for bitwise and
array ( 'foo' => 1, 'bar' => 2 ) & '010' - TypeError Unsupported operand type array for bitwise and
array ( 'foo' => 1, 'bar' => 2 ) & '10 elephants' - TypeError Unsupported operand type array for bitwise and
array ( 'foo' => 1, 'bar' => 2 ) & 'foo' - TypeError Unsupported operand type array for bitwise and
array ( 'foo' => 1, 'bar' => 2 ) & array ( ) - TypeError Unsupported operand type array for bitwise and
array ( 'foo' => 1, 'bar' => 2 ) & array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise and
array ( 'foo' => 1, 'bar' => 2 ) & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise and
array ( 'foo' => 1, 'bar' => 2 ) & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise and
array ( 'foo' => 1, 'bar' => 2 ) & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise and
array ( 'foo' => 1, 'bar' => 2 ) & (object) array ( ) - TypeError Unsupported operand type array for bitwise and
array ( 'foo' => 1, 'bar' => 2 ) & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise and
array ( 'foo' => 1, 'bar' => 2 ) & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise and
array ( 'foo' => 1, 'bar' => 2 ) & DateTime - TypeError Unsupported operand type array for bitwise and
array ( 'foo' => 1, 'bar' => 2 ) & resource - TypeError Unsupported operand type array for bitwise and
array ( 'foo' => 1, 'bar' => 2 ) & NULL - TypeError Unsupported operand type array for bitwise and
array ( 'bar' => 1, 'foo' => 2 ) & false - TypeError Unsupported operand type array for bitwise and
array ( 'bar' => 1, 'foo' => 2 ) & true - TypeError Unsupported operand type array for bitwise and
array ( 'bar' => 1, 'foo' => 2 ) & 0 - TypeError Unsupported operand type array for bitwise and
array ( 'bar' => 1, 'foo' => 2 ) & 10 - TypeError Unsupported operand type array for bitwise and
array ( 'bar' => 1, 'foo' => 2 ) & 0.0 - TypeError Unsupported operand type array for bitwise and
array ( 'bar' => 1, 'foo' => 2 ) & 10.0 - TypeError Unsupported operand type array for bitwise and
array ( 'bar' => 1, 'foo' => 2 ) & 3.14 - TypeError Unsupported operand type array for bitwise and
array ( 'bar' => 1, 'foo' => 2 ) & '0' - TypeError Unsupported operand type array for bitwise and
array ( 'bar' => 1, 'foo' => 2 ) & '10' - TypeError Unsupported operand type array for bitwise and
array ( 'bar' => 1, 'foo' => 2 ) & '010' - TypeError Unsupported operand type array for bitwise and
array ( 'bar' => 1, 'foo' => 2 ) & '10 elephants' - TypeError Unsupported operand type array for bitwise and
array ( 'bar' => 1, 'foo' => 2 ) & 'foo' - TypeError Unsupported operand type array for bitwise and
array ( 'bar' => 1, 'foo' => 2 ) & array ( ) - TypeError Unsupported operand type array for bitwise and
array ( 'bar' => 1, 'foo' => 2 ) & array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise and
array ( 'bar' => 1, 'foo' => 2 ) & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise and
array ( 'bar' => 1, 'foo' => 2 ) & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise and
array ( 'bar' => 1, 'foo' => 2 ) & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise and
array ( 'bar' => 1, 'foo' => 2 ) & (object) array ( ) - TypeError Unsupported operand type array for bitwise and
array ( 'bar' => 1, 'foo' => 2 ) & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise and
array ( 'bar' => 1, 'foo' => 2 ) & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise and
array ( 'bar' => 1, 'foo' => 2 ) & DateTime - TypeError Unsupported operand type array for bitwise and
array ( 'bar' => 1, 'foo' => 2 ) & resource - TypeError Unsupported operand type array for bitwise and
array ( 'bar' => 1, 'foo' => 2 ) & NULL - TypeError Unsupported operand type array for bitwise and
(object) array ( ) & false - TypeError Unsupported operand type object for bitwise and
(object) array ( ) & true - TypeError Unsupported operand type object for bitwise and
(object) array ( ) & 0 - TypeError Unsupported operand type object for bitwise and
(object) array ( ) & 10 - TypeError Unsupported operand type object for bitwise and
(object) array ( ) & 0.0 - TypeError Unsupported operand type object for bitwise and
(object) array ( ) & 10.0 - TypeError Unsupported operand type object for bitwise and
(object) array ( ) & 3.14 - TypeError Unsupported operand type object for bitwise and
(object) array ( ) & '0' - TypeError Unsupported operand type object for bitwise and
(object) array ( ) & '10' - TypeError Unsupported operand type object for bitwise and
(object) array ( ) & '010' - TypeError Unsupported operand type object for bitwise and
(object) array ( ) & '10 elephants' - TypeError Unsupported operand type object for bitwise and
(object) array ( ) & 'foo' - TypeError Unsupported operand type object for bitwise and
(object) array ( ) & array ( ) - TypeError Unsupported operand type object for bitwise and
(object) array ( ) & array ( 0 => 1 ) - TypeError Unsupported operand type object for bitwise and
(object) array ( ) & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for bitwise and
(object) array ( ) & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise and
(object) array ( ) & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise and
(object) array ( ) & (object) array ( ) - TypeError Unsupported operand type object for bitwise and
(object) array ( ) & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise and
(object) array ( ) & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise and
(object) array ( ) & DateTime - TypeError Unsupported operand type object for bitwise and
(object) array ( ) & resource - TypeError Unsupported operand type object for bitwise and
(object) array ( ) & NULL - TypeError Unsupported operand type object for bitwise and
(object) array ( 'foo' => 1, 'bar' => 2 ) & false - TypeError Unsupported operand type object for bitwise and
(object) array ( 'foo' => 1, 'bar' => 2 ) & true - TypeError Unsupported operand type object for bitwise and
(object) array ( 'foo' => 1, 'bar' => 2 ) & 0 - TypeError Unsupported operand type object for bitwise and
(object) array ( 'foo' => 1, 'bar' => 2 ) & 10 - TypeError Unsupported operand type object for bitwise and
(object) array ( 'foo' => 1, 'bar' => 2 ) & 0.0 - TypeError Unsupported operand type object for bitwise and
(object) array ( 'foo' => 1, 'bar' => 2 ) & 10.0 - TypeError Unsupported operand type object for bitwise and
(object) array ( 'foo' => 1, 'bar' => 2 ) & 3.14 - TypeError Unsupported operand type object for bitwise and
(object) array ( 'foo' => 1, 'bar' => 2 ) & '0' - TypeError Unsupported operand type object for bitwise and
(object) array ( 'foo' => 1, 'bar' => 2 ) & '10' - TypeError Unsupported operand type object for bitwise and
(object) array ( 'foo' => 1, 'bar' => 2 ) & '010' - TypeError Unsupported operand type object for bitwise and
(object) array ( 'foo' => 1, 'bar' => 2 ) & '10 elephants' - TypeError Unsupported operand type object for bitwise and
(object) array ( 'foo' => 1, 'bar' => 2 ) & 'foo' - TypeError Unsupported operand type object for bitwise and
(object) array ( 'foo' => 1, 'bar' => 2 ) & array ( ) - TypeError Unsupported operand type object for bitwise and
(object) array ( 'foo' => 1, 'bar' => 2 ) & array ( 0 => 1 ) - TypeError Unsupported operand type object for bitwise and
(object) array ( 'foo' => 1, 'bar' => 2 ) & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for bitwise and
(object) array ( 'foo' => 1, 'bar' => 2 ) & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise and
(object) array ( 'foo' => 1, 'bar' => 2 ) & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise and
(object) array ( 'foo' => 1, 'bar' => 2 ) & (object) array ( ) - TypeError Unsupported operand type object for bitwise and
(object) array ( 'foo' => 1, 'bar' => 2 ) & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise and
(object) array ( 'foo' => 1, 'bar' => 2 ) & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise and
(object) array ( 'foo' => 1, 'bar' => 2 ) & DateTime - TypeError Unsupported operand type object for bitwise and
(object) array ( 'foo' => 1, 'bar' => 2 ) & resource - TypeError Unsupported operand type object for bitwise and
(object) array ( 'foo' => 1, 'bar' => 2 ) & NULL - TypeError Unsupported operand type object for bitwise and
(object) array ( 'bar' => 1, 'foo' => 2 ) & false - TypeError Unsupported operand type object for bitwise and
(object) array ( 'bar' => 1, 'foo' => 2 ) & true - TypeError Unsupported operand type object for bitwise and
(object) array ( 'bar' => 1, 'foo' => 2 ) & 0 - TypeError Unsupported operand type object for bitwise and
(object) array ( 'bar' => 1, 'foo' => 2 ) & 10 - TypeError Unsupported operand type object for bitwise and
(object) array ( 'bar' => 1, 'foo' => 2 ) & 0.0 - TypeError Unsupported operand type object for bitwise and
(object) array ( 'bar' => 1, 'foo' => 2 ) & 10.0 - TypeError Unsupported operand type object for bitwise and
(object) array ( 'bar' => 1, 'foo' => 2 ) & 3.14 - TypeError Unsupported operand type object for bitwise and
(object) array ( 'bar' => 1, 'foo' => 2 ) & '0' - TypeError Unsupported operand type object for bitwise and
(object) array ( 'bar' => 1, 'foo' => 2 ) & '10' - TypeError Unsupported operand type object for bitwise and
(object) array ( 'bar' => 1, 'foo' => 2 ) & '010' - TypeError Unsupported operand type object for bitwise and
(object) array ( 'bar' => 1, 'foo' => 2 ) & '10 elephants' - TypeError Unsupported operand type object for bitwise and
(object) array ( 'bar' => 1, 'foo' => 2 ) & 'foo' - TypeError Unsupported operand type object for bitwise and
(object) array ( 'bar' => 1, 'foo' => 2 ) & array ( ) - TypeError Unsupported operand type object for bitwise and
(object) array ( 'bar' => 1, 'foo' => 2 ) & array ( 0 => 1 ) - TypeError Unsupported operand type object for bitwise and
(object) array ( 'bar' => 1, 'foo' => 2 ) & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for bitwise and
(object) array ( 'bar' => 1, 'foo' => 2 ) & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise and
(object) array ( 'bar' => 1, 'foo' => 2 ) & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise and
(object) array ( 'bar' => 1, 'foo' => 2 ) & (object) array ( ) - TypeError Unsupported operand type object for bitwise and
(object) array ( 'bar' => 1, 'foo' => 2 ) & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise and
(object) array ( 'bar' => 1, 'foo' => 2 ) & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise and
(object) array ( 'bar' => 1, 'foo' => 2 ) & DateTime - TypeError Unsupported operand type object for bitwise and
(object) array ( 'bar' => 1, 'foo' => 2 ) & resource - TypeError Unsupported operand type object for bitwise and
(object) array ( 'bar' => 1, 'foo' => 2 ) & NULL - TypeError Unsupported operand type object for bitwise and
DateTime & false - TypeError Unsupported operand type object for bitwise and
DateTime & true - TypeError Unsupported operand type object for bitwise and
DateTime & 0 - TypeError Unsupported operand type object for bitwise and
DateTime & 10 - TypeError Unsupported operand type object for bitwise and
DateTime & 0.0 - TypeError Unsupported operand type object for bitwise and
DateTime & 10.0 - TypeError Unsupported operand type object for bitwise and
DateTime & 3.14 - TypeError Unsupported operand type object for bitwise and
DateTime & '0' - TypeError Unsupported operand type object for bitwise and
DateTime & '10' - TypeError Unsupported operand type object for bitwise and
DateTime & '010' - TypeError Unsupported operand type object for bitwise and
DateTime & '10 elephants' - TypeError Unsupported operand type object for bitwise and
DateTime & 'foo' - TypeError Unsupported operand type object for bitwise and
DateTime & array ( ) - TypeError Unsupported operand type object for bitwise and
DateTime & array ( 0 => 1 ) - TypeError Unsupported operand type object for bitwise and
DateTime & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for bitwise and
DateTime & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise and
DateTime & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise and
DateTime & (object) array ( ) - TypeError Unsupported operand type object for bitwise and
DateTime & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise and
DateTime & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise and
DateTime & DateTime - TypeError Unsupported operand type object for bitwise and
DateTime & resource - TypeError Unsupported operand type object for bitwise and
DateTime & NULL - TypeError Unsupported operand type object for bitwise and
resource & false - TypeError Unsupported operand type resource for bitwise and
resource & true - TypeError Unsupported operand type resource for bitwise and
resource & 0 - TypeError Unsupported operand type resource for bitwise and
resource & 10 - TypeError Unsupported operand type resource for bitwise and
resource & 0.0 - TypeError Unsupported operand type resource for bitwise and
resource & 10.0 - TypeError Unsupported operand type resource for bitwise and
resource & 3.14 - TypeError Unsupported operand type resource for bitwise and
resource & '0' - TypeError Unsupported operand type resource for bitwise and
resource & '10' - TypeError Unsupported operand type resource for bitwise and
resource & '010' - TypeError Unsupported operand type resource for bitwise and
resource & '10 elephants' - TypeError Unsupported operand type resource for bitwise and
resource & 'foo' - TypeError Unsupported operand type resource for bitwise and
resource & array ( ) - TypeError Unsupported operand type resource for bitwise and
resource & array ( 0 => 1 ) - TypeError Unsupported operand type resource for bitwise and
resource & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type resource for bitwise and
resource & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for bitwise and
resource & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for bitwise and
resource & (object) array ( ) - TypeError Unsupported operand type resource for bitwise and
resource & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for bitwise and
resource & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for bitwise and
resource & DateTime - TypeError Unsupported operand type resource for bitwise and
resource & resource - TypeError Unsupported operand type resource for bitwise and
resource & NULL - TypeError Unsupported operand type resource for bitwise and
NULL & false - TypeError Unsupported operand type null for bitwise and
NULL & true - TypeError Unsupported operand type null for bitwise and
NULL & 0 - TypeError Unsupported operand type null for bitwise and
NULL & 10 - TypeError Unsupported operand type null for bitwise and
NULL & 0.0 - TypeError Unsupported operand type null for bitwise and
NULL & 10.0 - TypeError Unsupported operand type null for bitwise and
NULL & 3.14 - TypeError Unsupported operand type null for bitwise and
NULL & '0' - TypeError Unsupported operand type null for bitwise and
NULL & '10' - TypeError Unsupported operand type null for bitwise and
NULL & '010' - TypeError Unsupported operand type null for bitwise and
NULL & '10 elephants' - TypeError Unsupported operand type null for bitwise and
NULL & 'foo' - TypeError Unsupported operand type null for bitwise and
NULL & array ( ) - TypeError Unsupported operand type null for bitwise and
NULL & array ( 0 => 1 ) - TypeError Unsupported operand type null for bitwise and
NULL & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type null for bitwise and
NULL & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for bitwise and
NULL & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for bitwise and
NULL & (object) array ( ) - TypeError Unsupported operand type null for bitwise and
NULL & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for bitwise and
NULL & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for bitwise and
NULL & DateTime - TypeError Unsupported operand type null for bitwise and
NULL & resource - TypeError Unsupported operand type null for bitwise and
NULL & NULL - TypeError Unsupported operand type null for bitwise and