--TEST--
bitwise and operator with strict operators
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a & $b', function($a, $b) { return $a & $b; }, 'var_out_base64');

--EXPECT--
false & false - TypeError Unsupported operand type bool for '&' (bitwise and) operator
false & true - TypeError Unsupported operand type bool for '&' (bitwise and) operator
false & 0 - TypeError Unsupported operand type bool for '&' (bitwise and) operator
false & 10 - TypeError Unsupported operand type bool for '&' (bitwise and) operator
false & 0.0 - TypeError Unsupported operand type bool for '&' (bitwise and) operator
false & 10.0 - TypeError Unsupported operand type bool for '&' (bitwise and) operator
false & 3.14 - TypeError Unsupported operand type bool for '&' (bitwise and) operator
false & '0' - TypeError Unsupported operand type bool for '&' (bitwise and) operator
false & '10' - TypeError Unsupported operand type bool for '&' (bitwise and) operator
false & '010' - TypeError Unsupported operand type bool for '&' (bitwise and) operator
false & '10 elephants' - TypeError Unsupported operand type bool for '&' (bitwise and) operator
false & 'foo' - TypeError Unsupported operand type bool for '&' (bitwise and) operator
false & array ( ) - TypeError Unsupported operand type bool for '&' (bitwise and) operator
false & array ( 0 => 1 ) - TypeError Unsupported operand type bool for '&' (bitwise and) operator
false & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for '&' (bitwise and) operator
false & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for '&' (bitwise and) operator
false & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for '&' (bitwise and) operator
false & (object) array ( ) - TypeError Unsupported operand type bool for '&' (bitwise and) operator
false & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for '&' (bitwise and) operator
false & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for '&' (bitwise and) operator
false & DateTime - TypeError Unsupported operand type bool for '&' (bitwise and) operator
false & resource - TypeError Unsupported operand type bool for '&' (bitwise and) operator
false & NULL - TypeError Unsupported operand type bool for '&' (bitwise and) operator
true & false - TypeError Unsupported operand type bool for '&' (bitwise and) operator
true & true - TypeError Unsupported operand type bool for '&' (bitwise and) operator
true & 0 - TypeError Unsupported operand type bool for '&' (bitwise and) operator
true & 10 - TypeError Unsupported operand type bool for '&' (bitwise and) operator
true & 0.0 - TypeError Unsupported operand type bool for '&' (bitwise and) operator
true & 10.0 - TypeError Unsupported operand type bool for '&' (bitwise and) operator
true & 3.14 - TypeError Unsupported operand type bool for '&' (bitwise and) operator
true & '0' - TypeError Unsupported operand type bool for '&' (bitwise and) operator
true & '10' - TypeError Unsupported operand type bool for '&' (bitwise and) operator
true & '010' - TypeError Unsupported operand type bool for '&' (bitwise and) operator
true & '10 elephants' - TypeError Unsupported operand type bool for '&' (bitwise and) operator
true & 'foo' - TypeError Unsupported operand type bool for '&' (bitwise and) operator
true & array ( ) - TypeError Unsupported operand type bool for '&' (bitwise and) operator
true & array ( 0 => 1 ) - TypeError Unsupported operand type bool for '&' (bitwise and) operator
true & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for '&' (bitwise and) operator
true & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for '&' (bitwise and) operator
true & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for '&' (bitwise and) operator
true & (object) array ( ) - TypeError Unsupported operand type bool for '&' (bitwise and) operator
true & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for '&' (bitwise and) operator
true & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for '&' (bitwise and) operator
true & DateTime - TypeError Unsupported operand type bool for '&' (bitwise and) operator
true & resource - TypeError Unsupported operand type bool for '&' (bitwise and) operator
true & NULL - TypeError Unsupported operand type bool for '&' (bitwise and) operator
0 & false - TypeError Unsupported operand type bool for '&' (bitwise and) operator
0 & true - TypeError Unsupported operand type bool for '&' (bitwise and) operator
0 & 0 = 0
0 & 10 = 0
0 & 0.0 - TypeError Unsupported operand type float for '&' (bitwise and) operator
0 & 10.0 - TypeError Unsupported operand type float for '&' (bitwise and) operator
0 & 3.14 - TypeError Unsupported operand type float for '&' (bitwise and) operator
0 & '0' - TypeError Operand type mismatch int and string for '&' (bitwise and) operator
0 & '10' - TypeError Operand type mismatch int and string for '&' (bitwise and) operator
0 & '010' - TypeError Operand type mismatch int and string for '&' (bitwise and) operator
0 & '10 elephants' - TypeError Operand type mismatch int and string for '&' (bitwise and) operator
0 & 'foo' - TypeError Operand type mismatch int and string for '&' (bitwise and) operator
0 & array ( ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
0 & array ( 0 => 1 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
0 & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
0 & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
0 & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
0 & (object) array ( ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
0 & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
0 & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
0 & DateTime - TypeError Unsupported operand type object for '&' (bitwise and) operator
0 & resource - TypeError Unsupported operand type resource for '&' (bitwise and) operator
0 & NULL - TypeError Unsupported operand type null for '&' (bitwise and) operator
10 & false - TypeError Unsupported operand type bool for '&' (bitwise and) operator
10 & true - TypeError Unsupported operand type bool for '&' (bitwise and) operator
10 & 0 = 0
10 & 10 = 10
10 & 0.0 - TypeError Unsupported operand type float for '&' (bitwise and) operator
10 & 10.0 - TypeError Unsupported operand type float for '&' (bitwise and) operator
10 & 3.14 - TypeError Unsupported operand type float for '&' (bitwise and) operator
10 & '0' - TypeError Operand type mismatch int and string for '&' (bitwise and) operator
10 & '10' - TypeError Operand type mismatch int and string for '&' (bitwise and) operator
10 & '010' - TypeError Operand type mismatch int and string for '&' (bitwise and) operator
10 & '10 elephants' - TypeError Operand type mismatch int and string for '&' (bitwise and) operator
10 & 'foo' - TypeError Operand type mismatch int and string for '&' (bitwise and) operator
10 & array ( ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
10 & array ( 0 => 1 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
10 & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
10 & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
10 & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
10 & (object) array ( ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
10 & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
10 & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
10 & DateTime - TypeError Unsupported operand type object for '&' (bitwise and) operator
10 & resource - TypeError Unsupported operand type resource for '&' (bitwise and) operator
10 & NULL - TypeError Unsupported operand type null for '&' (bitwise and) operator
0.0 & false - TypeError Unsupported operand type float for '&' (bitwise and) operator
0.0 & true - TypeError Unsupported operand type float for '&' (bitwise and) operator
0.0 & 0 - TypeError Unsupported operand type float for '&' (bitwise and) operator
0.0 & 10 - TypeError Unsupported operand type float for '&' (bitwise and) operator
0.0 & 0.0 - TypeError Unsupported operand type float for '&' (bitwise and) operator
0.0 & 10.0 - TypeError Unsupported operand type float for '&' (bitwise and) operator
0.0 & 3.14 - TypeError Unsupported operand type float for '&' (bitwise and) operator
0.0 & '0' - TypeError Unsupported operand type float for '&' (bitwise and) operator
0.0 & '10' - TypeError Unsupported operand type float for '&' (bitwise and) operator
0.0 & '010' - TypeError Unsupported operand type float for '&' (bitwise and) operator
0.0 & '10 elephants' - TypeError Unsupported operand type float for '&' (bitwise and) operator
0.0 & 'foo' - TypeError Unsupported operand type float for '&' (bitwise and) operator
0.0 & array ( ) - TypeError Unsupported operand type float for '&' (bitwise and) operator
0.0 & array ( 0 => 1 ) - TypeError Unsupported operand type float for '&' (bitwise and) operator
0.0 & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type float for '&' (bitwise and) operator
0.0 & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for '&' (bitwise and) operator
0.0 & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for '&' (bitwise and) operator
0.0 & (object) array ( ) - TypeError Unsupported operand type float for '&' (bitwise and) operator
0.0 & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for '&' (bitwise and) operator
0.0 & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for '&' (bitwise and) operator
0.0 & DateTime - TypeError Unsupported operand type float for '&' (bitwise and) operator
0.0 & resource - TypeError Unsupported operand type float for '&' (bitwise and) operator
0.0 & NULL - TypeError Unsupported operand type float for '&' (bitwise and) operator
10.0 & false - TypeError Unsupported operand type float for '&' (bitwise and) operator
10.0 & true - TypeError Unsupported operand type float for '&' (bitwise and) operator
10.0 & 0 - TypeError Unsupported operand type float for '&' (bitwise and) operator
10.0 & 10 - TypeError Unsupported operand type float for '&' (bitwise and) operator
10.0 & 0.0 - TypeError Unsupported operand type float for '&' (bitwise and) operator
10.0 & 10.0 - TypeError Unsupported operand type float for '&' (bitwise and) operator
10.0 & 3.14 - TypeError Unsupported operand type float for '&' (bitwise and) operator
10.0 & '0' - TypeError Unsupported operand type float for '&' (bitwise and) operator
10.0 & '10' - TypeError Unsupported operand type float for '&' (bitwise and) operator
10.0 & '010' - TypeError Unsupported operand type float for '&' (bitwise and) operator
10.0 & '10 elephants' - TypeError Unsupported operand type float for '&' (bitwise and) operator
10.0 & 'foo' - TypeError Unsupported operand type float for '&' (bitwise and) operator
10.0 & array ( ) - TypeError Unsupported operand type float for '&' (bitwise and) operator
10.0 & array ( 0 => 1 ) - TypeError Unsupported operand type float for '&' (bitwise and) operator
10.0 & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type float for '&' (bitwise and) operator
10.0 & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for '&' (bitwise and) operator
10.0 & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for '&' (bitwise and) operator
10.0 & (object) array ( ) - TypeError Unsupported operand type float for '&' (bitwise and) operator
10.0 & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for '&' (bitwise and) operator
10.0 & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for '&' (bitwise and) operator
10.0 & DateTime - TypeError Unsupported operand type float for '&' (bitwise and) operator
10.0 & resource - TypeError Unsupported operand type float for '&' (bitwise and) operator
10.0 & NULL - TypeError Unsupported operand type float for '&' (bitwise and) operator
3.14 & false - TypeError Unsupported operand type float for '&' (bitwise and) operator
3.14 & true - TypeError Unsupported operand type float for '&' (bitwise and) operator
3.14 & 0 - TypeError Unsupported operand type float for '&' (bitwise and) operator
3.14 & 10 - TypeError Unsupported operand type float for '&' (bitwise and) operator
3.14 & 0.0 - TypeError Unsupported operand type float for '&' (bitwise and) operator
3.14 & 10.0 - TypeError Unsupported operand type float for '&' (bitwise and) operator
3.14 & 3.14 - TypeError Unsupported operand type float for '&' (bitwise and) operator
3.14 & '0' - TypeError Unsupported operand type float for '&' (bitwise and) operator
3.14 & '10' - TypeError Unsupported operand type float for '&' (bitwise and) operator
3.14 & '010' - TypeError Unsupported operand type float for '&' (bitwise and) operator
3.14 & '10 elephants' - TypeError Unsupported operand type float for '&' (bitwise and) operator
3.14 & 'foo' - TypeError Unsupported operand type float for '&' (bitwise and) operator
3.14 & array ( ) - TypeError Unsupported operand type float for '&' (bitwise and) operator
3.14 & array ( 0 => 1 ) - TypeError Unsupported operand type float for '&' (bitwise and) operator
3.14 & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type float for '&' (bitwise and) operator
3.14 & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for '&' (bitwise and) operator
3.14 & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for '&' (bitwise and) operator
3.14 & (object) array ( ) - TypeError Unsupported operand type float for '&' (bitwise and) operator
3.14 & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for '&' (bitwise and) operator
3.14 & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for '&' (bitwise and) operator
3.14 & DateTime - TypeError Unsupported operand type float for '&' (bitwise and) operator
3.14 & resource - TypeError Unsupported operand type float for '&' (bitwise and) operator
3.14 & NULL - TypeError Unsupported operand type float for '&' (bitwise and) operator
'0' & false - TypeError Unsupported operand type bool for '&' (bitwise and) operator
'0' & true - TypeError Unsupported operand type bool for '&' (bitwise and) operator
'0' & 0 - TypeError Operand type mismatch string and int for '&' (bitwise and) operator
'0' & 10 - TypeError Operand type mismatch string and int for '&' (bitwise and) operator
'0' & 0.0 - TypeError Unsupported operand type float for '&' (bitwise and) operator
'0' & 10.0 - TypeError Unsupported operand type float for '&' (bitwise and) operator
'0' & 3.14 - TypeError Unsupported operand type float for '&' (bitwise and) operator
'0' & '0' = base64:MA==
'0' & '10' = base64:MA==
'0' & '010' = base64:MA==
'0' & '10 elephants' = base64:MA==
'0' & 'foo' = base64:IA==
'0' & array ( ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'0' & array ( 0 => 1 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'0' & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'0' & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'0' & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'0' & (object) array ( ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
'0' & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
'0' & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
'0' & DateTime - TypeError Unsupported operand type object for '&' (bitwise and) operator
'0' & resource - TypeError Unsupported operand type resource for '&' (bitwise and) operator
'0' & NULL - TypeError Unsupported operand type null for '&' (bitwise and) operator
'10' & false - TypeError Unsupported operand type bool for '&' (bitwise and) operator
'10' & true - TypeError Unsupported operand type bool for '&' (bitwise and) operator
'10' & 0 - TypeError Operand type mismatch string and int for '&' (bitwise and) operator
'10' & 10 - TypeError Operand type mismatch string and int for '&' (bitwise and) operator
'10' & 0.0 - TypeError Unsupported operand type float for '&' (bitwise and) operator
'10' & 10.0 - TypeError Unsupported operand type float for '&' (bitwise and) operator
'10' & 3.14 - TypeError Unsupported operand type float for '&' (bitwise and) operator
'10' & '0' = base64:MA==
'10' & '10' = base64:MTA=
'10' & '010' = base64:MDA=
'10' & '10 elephants' = base64:MTA=
'10' & 'foo' = base64:ICA=
'10' & array ( ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'10' & array ( 0 => 1 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'10' & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'10' & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'10' & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'10' & (object) array ( ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
'10' & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
'10' & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
'10' & DateTime - TypeError Unsupported operand type object for '&' (bitwise and) operator
'10' & resource - TypeError Unsupported operand type resource for '&' (bitwise and) operator
'10' & NULL - TypeError Unsupported operand type null for '&' (bitwise and) operator
'010' & false - TypeError Unsupported operand type bool for '&' (bitwise and) operator
'010' & true - TypeError Unsupported operand type bool for '&' (bitwise and) operator
'010' & 0 - TypeError Operand type mismatch string and int for '&' (bitwise and) operator
'010' & 10 - TypeError Operand type mismatch string and int for '&' (bitwise and) operator
'010' & 0.0 - TypeError Unsupported operand type float for '&' (bitwise and) operator
'010' & 10.0 - TypeError Unsupported operand type float for '&' (bitwise and) operator
'010' & 3.14 - TypeError Unsupported operand type float for '&' (bitwise and) operator
'010' & '0' = base64:MA==
'010' & '10' = base64:MDA=
'010' & '010' = base64:MDEw
'010' & '10 elephants' = base64:MDAg
'010' & 'foo' = base64:ICEg
'010' & array ( ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'010' & array ( 0 => 1 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'010' & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'010' & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'010' & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'010' & (object) array ( ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
'010' & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
'010' & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
'010' & DateTime - TypeError Unsupported operand type object for '&' (bitwise and) operator
'010' & resource - TypeError Unsupported operand type resource for '&' (bitwise and) operator
'010' & NULL - TypeError Unsupported operand type null for '&' (bitwise and) operator
'10 elephants' & false - TypeError Unsupported operand type bool for '&' (bitwise and) operator
'10 elephants' & true - TypeError Unsupported operand type bool for '&' (bitwise and) operator
'10 elephants' & 0 - TypeError Operand type mismatch string and int for '&' (bitwise and) operator
'10 elephants' & 10 - TypeError Operand type mismatch string and int for '&' (bitwise and) operator
'10 elephants' & 0.0 - TypeError Unsupported operand type float for '&' (bitwise and) operator
'10 elephants' & 10.0 - TypeError Unsupported operand type float for '&' (bitwise and) operator
'10 elephants' & 3.14 - TypeError Unsupported operand type float for '&' (bitwise and) operator
'10 elephants' & '0' = base64:MA==
'10 elephants' & '10' = base64:MTA=
'10 elephants' & '010' = base64:MDAg
'10 elephants' & '10 elephants' = base64:MTAgZWxlcGhhbnRz
'10 elephants' & 'foo' = base64:ICAg
'10 elephants' & array ( ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'10 elephants' & array ( 0 => 1 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'10 elephants' & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'10 elephants' & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'10 elephants' & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'10 elephants' & (object) array ( ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
'10 elephants' & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
'10 elephants' & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
'10 elephants' & DateTime - TypeError Unsupported operand type object for '&' (bitwise and) operator
'10 elephants' & resource - TypeError Unsupported operand type resource for '&' (bitwise and) operator
'10 elephants' & NULL - TypeError Unsupported operand type null for '&' (bitwise and) operator
'foo' & false - TypeError Unsupported operand type bool for '&' (bitwise and) operator
'foo' & true - TypeError Unsupported operand type bool for '&' (bitwise and) operator
'foo' & 0 - TypeError Operand type mismatch string and int for '&' (bitwise and) operator
'foo' & 10 - TypeError Operand type mismatch string and int for '&' (bitwise and) operator
'foo' & 0.0 - TypeError Unsupported operand type float for '&' (bitwise and) operator
'foo' & 10.0 - TypeError Unsupported operand type float for '&' (bitwise and) operator
'foo' & 3.14 - TypeError Unsupported operand type float for '&' (bitwise and) operator
'foo' & '0' = base64:IA==
'foo' & '10' = base64:ICA=
'foo' & '010' = base64:ICEg
'foo' & '10 elephants' = base64:ICAg
'foo' & 'foo' = base64:Zm9v
'foo' & array ( ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'foo' & array ( 0 => 1 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'foo' & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'foo' & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'foo' & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
'foo' & (object) array ( ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
'foo' & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
'foo' & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
'foo' & DateTime - TypeError Unsupported operand type object for '&' (bitwise and) operator
'foo' & resource - TypeError Unsupported operand type resource for '&' (bitwise and) operator
'foo' & NULL - TypeError Unsupported operand type null for '&' (bitwise and) operator
array ( ) & false - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( ) & true - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( ) & 0 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( ) & 10 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( ) & 0.0 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( ) & 10.0 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( ) & 3.14 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( ) & '0' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( ) & '10' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( ) & '010' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( ) & '10 elephants' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( ) & 'foo' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( ) & array ( ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( ) & array ( 0 => 1 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( ) & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( ) & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( ) & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( ) & (object) array ( ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( ) & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( ) & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( ) & DateTime - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( ) & resource - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( ) & NULL - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1 ) & false - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1 ) & true - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1 ) & 0 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1 ) & 10 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1 ) & 0.0 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1 ) & 10.0 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1 ) & 3.14 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1 ) & '0' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1 ) & '10' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1 ) & '010' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1 ) & '10 elephants' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1 ) & 'foo' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1 ) & array ( ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1 ) & array ( 0 => 1 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1 ) & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1 ) & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1 ) & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1 ) & (object) array ( ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1 ) & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1 ) & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1 ) & DateTime - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1 ) & resource - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1 ) & NULL - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1, 1 => 100 ) & false - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1, 1 => 100 ) & true - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1, 1 => 100 ) & 0 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1, 1 => 100 ) & 10 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1, 1 => 100 ) & 0.0 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1, 1 => 100 ) & 10.0 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1, 1 => 100 ) & 3.14 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1, 1 => 100 ) & '0' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1, 1 => 100 ) & '10' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1, 1 => 100 ) & '010' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1, 1 => 100 ) & '10 elephants' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1, 1 => 100 ) & 'foo' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1, 1 => 100 ) & array ( ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1, 1 => 100 ) & array ( 0 => 1 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1, 1 => 100 ) & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1, 1 => 100 ) & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1, 1 => 100 ) & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1, 1 => 100 ) & (object) array ( ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1, 1 => 100 ) & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1, 1 => 100 ) & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1, 1 => 100 ) & DateTime - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1, 1 => 100 ) & resource - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 0 => 1, 1 => 100 ) & NULL - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'foo' => 1, 'bar' => 2 ) & false - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'foo' => 1, 'bar' => 2 ) & true - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'foo' => 1, 'bar' => 2 ) & 0 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'foo' => 1, 'bar' => 2 ) & 10 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'foo' => 1, 'bar' => 2 ) & 0.0 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'foo' => 1, 'bar' => 2 ) & 10.0 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'foo' => 1, 'bar' => 2 ) & 3.14 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'foo' => 1, 'bar' => 2 ) & '0' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'foo' => 1, 'bar' => 2 ) & '10' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'foo' => 1, 'bar' => 2 ) & '010' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'foo' => 1, 'bar' => 2 ) & '10 elephants' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'foo' => 1, 'bar' => 2 ) & 'foo' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'foo' => 1, 'bar' => 2 ) & array ( ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'foo' => 1, 'bar' => 2 ) & array ( 0 => 1 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'foo' => 1, 'bar' => 2 ) & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'foo' => 1, 'bar' => 2 ) & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'foo' => 1, 'bar' => 2 ) & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'foo' => 1, 'bar' => 2 ) & (object) array ( ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'foo' => 1, 'bar' => 2 ) & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'foo' => 1, 'bar' => 2 ) & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'foo' => 1, 'bar' => 2 ) & DateTime - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'foo' => 1, 'bar' => 2 ) & resource - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'foo' => 1, 'bar' => 2 ) & NULL - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'bar' => 1, 'foo' => 2 ) & false - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'bar' => 1, 'foo' => 2 ) & true - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'bar' => 1, 'foo' => 2 ) & 0 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'bar' => 1, 'foo' => 2 ) & 10 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'bar' => 1, 'foo' => 2 ) & 0.0 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'bar' => 1, 'foo' => 2 ) & 10.0 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'bar' => 1, 'foo' => 2 ) & 3.14 - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'bar' => 1, 'foo' => 2 ) & '0' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'bar' => 1, 'foo' => 2 ) & '10' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'bar' => 1, 'foo' => 2 ) & '010' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'bar' => 1, 'foo' => 2 ) & '10 elephants' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'bar' => 1, 'foo' => 2 ) & 'foo' - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'bar' => 1, 'foo' => 2 ) & array ( ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'bar' => 1, 'foo' => 2 ) & array ( 0 => 1 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'bar' => 1, 'foo' => 2 ) & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'bar' => 1, 'foo' => 2 ) & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'bar' => 1, 'foo' => 2 ) & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'bar' => 1, 'foo' => 2 ) & (object) array ( ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'bar' => 1, 'foo' => 2 ) & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'bar' => 1, 'foo' => 2 ) & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'bar' => 1, 'foo' => 2 ) & DateTime - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'bar' => 1, 'foo' => 2 ) & resource - TypeError Unsupported operand type array for '&' (bitwise and) operator
array ( 'bar' => 1, 'foo' => 2 ) & NULL - TypeError Unsupported operand type array for '&' (bitwise and) operator
(object) array ( ) & false - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( ) & true - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( ) & 0 - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( ) & 10 - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( ) & 0.0 - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( ) & 10.0 - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( ) & 3.14 - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( ) & '0' - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( ) & '10' - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( ) & '010' - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( ) & '10 elephants' - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( ) & 'foo' - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( ) & array ( ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( ) & array ( 0 => 1 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( ) & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( ) & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( ) & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( ) & (object) array ( ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( ) & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( ) & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( ) & DateTime - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( ) & resource - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( ) & NULL - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) & false - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) & true - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) & 0 - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) & 10 - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) & 0.0 - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) & 10.0 - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) & 3.14 - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) & '0' - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) & '10' - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) & '010' - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) & '10 elephants' - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) & 'foo' - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) & array ( ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) & array ( 0 => 1 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) & (object) array ( ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) & DateTime - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) & resource - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'foo' => 1, 'bar' => 2 ) & NULL - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) & false - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) & true - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) & 0 - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) & 10 - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) & 0.0 - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) & 10.0 - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) & 3.14 - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) & '0' - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) & '10' - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) & '010' - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) & '10 elephants' - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) & 'foo' - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) & array ( ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) & array ( 0 => 1 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) & (object) array ( ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) & DateTime - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) & resource - TypeError Unsupported operand type object for '&' (bitwise and) operator
(object) array ( 'bar' => 1, 'foo' => 2 ) & NULL - TypeError Unsupported operand type object for '&' (bitwise and) operator
DateTime & false - TypeError Unsupported operand type object for '&' (bitwise and) operator
DateTime & true - TypeError Unsupported operand type object for '&' (bitwise and) operator
DateTime & 0 - TypeError Unsupported operand type object for '&' (bitwise and) operator
DateTime & 10 - TypeError Unsupported operand type object for '&' (bitwise and) operator
DateTime & 0.0 - TypeError Unsupported operand type object for '&' (bitwise and) operator
DateTime & 10.0 - TypeError Unsupported operand type object for '&' (bitwise and) operator
DateTime & 3.14 - TypeError Unsupported operand type object for '&' (bitwise and) operator
DateTime & '0' - TypeError Unsupported operand type object for '&' (bitwise and) operator
DateTime & '10' - TypeError Unsupported operand type object for '&' (bitwise and) operator
DateTime & '010' - TypeError Unsupported operand type object for '&' (bitwise and) operator
DateTime & '10 elephants' - TypeError Unsupported operand type object for '&' (bitwise and) operator
DateTime & 'foo' - TypeError Unsupported operand type object for '&' (bitwise and) operator
DateTime & array ( ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
DateTime & array ( 0 => 1 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
DateTime & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
DateTime & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
DateTime & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
DateTime & (object) array ( ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
DateTime & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
DateTime & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '&' (bitwise and) operator
DateTime & DateTime - TypeError Unsupported operand type object for '&' (bitwise and) operator
DateTime & resource - TypeError Unsupported operand type object for '&' (bitwise and) operator
DateTime & NULL - TypeError Unsupported operand type object for '&' (bitwise and) operator
resource & false - TypeError Unsupported operand type resource for '&' (bitwise and) operator
resource & true - TypeError Unsupported operand type resource for '&' (bitwise and) operator
resource & 0 - TypeError Unsupported operand type resource for '&' (bitwise and) operator
resource & 10 - TypeError Unsupported operand type resource for '&' (bitwise and) operator
resource & 0.0 - TypeError Unsupported operand type resource for '&' (bitwise and) operator
resource & 10.0 - TypeError Unsupported operand type resource for '&' (bitwise and) operator
resource & 3.14 - TypeError Unsupported operand type resource for '&' (bitwise and) operator
resource & '0' - TypeError Unsupported operand type resource for '&' (bitwise and) operator
resource & '10' - TypeError Unsupported operand type resource for '&' (bitwise and) operator
resource & '010' - TypeError Unsupported operand type resource for '&' (bitwise and) operator
resource & '10 elephants' - TypeError Unsupported operand type resource for '&' (bitwise and) operator
resource & 'foo' - TypeError Unsupported operand type resource for '&' (bitwise and) operator
resource & array ( ) - TypeError Unsupported operand type resource for '&' (bitwise and) operator
resource & array ( 0 => 1 ) - TypeError Unsupported operand type resource for '&' (bitwise and) operator
resource & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type resource for '&' (bitwise and) operator
resource & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for '&' (bitwise and) operator
resource & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for '&' (bitwise and) operator
resource & (object) array ( ) - TypeError Unsupported operand type resource for '&' (bitwise and) operator
resource & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for '&' (bitwise and) operator
resource & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for '&' (bitwise and) operator
resource & DateTime - TypeError Unsupported operand type resource for '&' (bitwise and) operator
resource & resource - TypeError Unsupported operand type resource for '&' (bitwise and) operator
resource & NULL - TypeError Unsupported operand type resource for '&' (bitwise and) operator
NULL & false - TypeError Unsupported operand type null for '&' (bitwise and) operator
NULL & true - TypeError Unsupported operand type null for '&' (bitwise and) operator
NULL & 0 - TypeError Unsupported operand type null for '&' (bitwise and) operator
NULL & 10 - TypeError Unsupported operand type null for '&' (bitwise and) operator
NULL & 0.0 - TypeError Unsupported operand type null for '&' (bitwise and) operator
NULL & 10.0 - TypeError Unsupported operand type null for '&' (bitwise and) operator
NULL & 3.14 - TypeError Unsupported operand type null for '&' (bitwise and) operator
NULL & '0' - TypeError Unsupported operand type null for '&' (bitwise and) operator
NULL & '10' - TypeError Unsupported operand type null for '&' (bitwise and) operator
NULL & '010' - TypeError Unsupported operand type null for '&' (bitwise and) operator
NULL & '10 elephants' - TypeError Unsupported operand type null for '&' (bitwise and) operator
NULL & 'foo' - TypeError Unsupported operand type null for '&' (bitwise and) operator
NULL & array ( ) - TypeError Unsupported operand type null for '&' (bitwise and) operator
NULL & array ( 0 => 1 ) - TypeError Unsupported operand type null for '&' (bitwise and) operator
NULL & array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type null for '&' (bitwise and) operator
NULL & array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for '&' (bitwise and) operator
NULL & array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for '&' (bitwise and) operator
NULL & (object) array ( ) - TypeError Unsupported operand type null for '&' (bitwise and) operator
NULL & (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for '&' (bitwise and) operator
NULL & (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for '&' (bitwise and) operator
NULL & DateTime - TypeError Unsupported operand type null for '&' (bitwise and) operator
NULL & resource - TypeError Unsupported operand type null for '&' (bitwise and) operator
NULL & NULL - TypeError Unsupported operand type null for '&' (bitwise and) operator