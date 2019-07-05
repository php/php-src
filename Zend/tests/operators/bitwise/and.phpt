--TEST--
bitwise and operator
--FILE--
<?php
require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a & $b', function($a, $b) { return $a & $b; }, 'var_out_base64');

--EXPECT--
false & false = 0
false & true = 0
false & 0 = 0
false & 10 = 0
false & 0.0 = 0
false & 10.0 = 0
false & 3.14 = 0
false & '0' = 0
false & '10' = 0
false & '010' = 0
false & '10 elephants' = 0 - Notice A non well formed numeric value encountered
false & 'foo' = 0 - Warning A non-numeric value encountered
false & array ( ) = 0
false & array ( 0 => 1 ) = 0
false & array ( 0 => 1, 1 => 100 ) = 0
false & array ( 'foo' => 1, 'bar' => 2 ) = 0
false & array ( 'bar' => 1, 'foo' => 2 ) = 0
false & (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
false & (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
false & (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
false & DateTime = 0 - Notice Object of class DateTime could not be converted to int
false & resource = 0
false & NULL = 0
true & false = 0
true & true = 1
true & 0 = 0
true & 10 = 0
true & 0.0 = 0
true & 10.0 = 0
true & 3.14 = 1
true & '0' = 0
true & '10' = 0
true & '010' = 0
true & '10 elephants' = 0 - Notice A non well formed numeric value encountered
true & 'foo' = 0 - Warning A non-numeric value encountered
true & array ( ) = 0
true & array ( 0 => 1 ) = 1
true & array ( 0 => 1, 1 => 100 ) = 1
true & array ( 'foo' => 1, 'bar' => 2 ) = 1
true & array ( 'bar' => 1, 'foo' => 2 ) = 1
true & (object) array ( ) = 1 - Notice Object of class stdClass could not be converted to int
true & (object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
true & (object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
true & DateTime = 1 - Notice Object of class DateTime could not be converted to int
true & resource = 0
true & NULL = 0
0 & false = 0
0 & true = 0
0 & 0 = 0
0 & 10 = 0
0 & 0.0 = 0
0 & 10.0 = 0
0 & 3.14 = 0
0 & '0' = 0
0 & '10' = 0
0 & '010' = 0
0 & '10 elephants' = 0 - Notice A non well formed numeric value encountered
0 & 'foo' = 0 - Warning A non-numeric value encountered
0 & array ( ) = 0
0 & array ( 0 => 1 ) = 0
0 & array ( 0 => 1, 1 => 100 ) = 0
0 & array ( 'foo' => 1, 'bar' => 2 ) = 0
0 & array ( 'bar' => 1, 'foo' => 2 ) = 0
0 & (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
0 & (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
0 & (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
0 & DateTime = 0 - Notice Object of class DateTime could not be converted to int
0 & resource = 0
0 & NULL = 0
10 & false = 0
10 & true = 0
10 & 0 = 0
10 & 10 = 10
10 & 0.0 = 0
10 & 10.0 = 10
10 & 3.14 = 2
10 & '0' = 0
10 & '10' = 10
10 & '010' = 10
10 & '10 elephants' = 10 - Notice A non well formed numeric value encountered
10 & 'foo' = 0 - Warning A non-numeric value encountered
10 & array ( ) = 0
10 & array ( 0 => 1 ) = 0
10 & array ( 0 => 1, 1 => 100 ) = 0
10 & array ( 'foo' => 1, 'bar' => 2 ) = 0
10 & array ( 'bar' => 1, 'foo' => 2 ) = 0
10 & (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
10 & (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
10 & (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
10 & DateTime = 0 - Notice Object of class DateTime could not be converted to int
10 & resource = 2
10 & NULL = 0
0.0 & false = 0
0.0 & true = 0
0.0 & 0 = 0
0.0 & 10 = 0
0.0 & 0.0 = 0
0.0 & 10.0 = 0
0.0 & 3.14 = 0
0.0 & '0' = 0
0.0 & '10' = 0
0.0 & '010' = 0
0.0 & '10 elephants' = 0 - Notice A non well formed numeric value encountered
0.0 & 'foo' = 0 - Warning A non-numeric value encountered
0.0 & array ( ) = 0
0.0 & array ( 0 => 1 ) = 0
0.0 & array ( 0 => 1, 1 => 100 ) = 0
0.0 & array ( 'foo' => 1, 'bar' => 2 ) = 0
0.0 & array ( 'bar' => 1, 'foo' => 2 ) = 0
0.0 & (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
0.0 & (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
0.0 & (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
0.0 & DateTime = 0 - Notice Object of class DateTime could not be converted to int
0.0 & resource = 0
0.0 & NULL = 0
10.0 & false = 0
10.0 & true = 0
10.0 & 0 = 0
10.0 & 10 = 10
10.0 & 0.0 = 0
10.0 & 10.0 = 10
10.0 & 3.14 = 2
10.0 & '0' = 0
10.0 & '10' = 10
10.0 & '010' = 10
10.0 & '10 elephants' = 10 - Notice A non well formed numeric value encountered
10.0 & 'foo' = 0 - Warning A non-numeric value encountered
10.0 & array ( ) = 0
10.0 & array ( 0 => 1 ) = 0
10.0 & array ( 0 => 1, 1 => 100 ) = 0
10.0 & array ( 'foo' => 1, 'bar' => 2 ) = 0
10.0 & array ( 'bar' => 1, 'foo' => 2 ) = 0
10.0 & (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
10.0 & (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
10.0 & (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
10.0 & DateTime = 0 - Notice Object of class DateTime could not be converted to int
10.0 & resource = 2
10.0 & NULL = 0
3.14 & false = 0
3.14 & true = 1
3.14 & 0 = 0
3.14 & 10 = 2
3.14 & 0.0 = 0
3.14 & 10.0 = 2
3.14 & 3.14 = 3
3.14 & '0' = 0
3.14 & '10' = 2
3.14 & '010' = 2
3.14 & '10 elephants' = 2 - Notice A non well formed numeric value encountered
3.14 & 'foo' = 0 - Warning A non-numeric value encountered
3.14 & array ( ) = 0
3.14 & array ( 0 => 1 ) = 1
3.14 & array ( 0 => 1, 1 => 100 ) = 1
3.14 & array ( 'foo' => 1, 'bar' => 2 ) = 1
3.14 & array ( 'bar' => 1, 'foo' => 2 ) = 1
3.14 & (object) array ( ) = 1 - Notice Object of class stdClass could not be converted to int
3.14 & (object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
3.14 & (object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
3.14 & DateTime = 1 - Notice Object of class DateTime could not be converted to int
3.14 & resource = 2
3.14 & NULL = 0
'0' & false = 0
'0' & true = 0
'0' & 0 = 0
'0' & 10 = 0
'0' & 0.0 = 0
'0' & 10.0 = 0
'0' & 3.14 = 0
'0' & '0' = base64:MA==
'0' & '10' = base64:MA==
'0' & '010' = base64:MA==
'0' & '10 elephants' = base64:MA==
'0' & 'foo' = base64:IA==
'0' & array ( ) = 0
'0' & array ( 0 => 1 ) = 0
'0' & array ( 0 => 1, 1 => 100 ) = 0
'0' & array ( 'foo' => 1, 'bar' => 2 ) = 0
'0' & array ( 'bar' => 1, 'foo' => 2 ) = 0
'0' & (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
'0' & (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
'0' & (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
'0' & DateTime = 0 - Notice Object of class DateTime could not be converted to int
'0' & resource = 0
'0' & NULL = 0
'10' & false = 0
'10' & true = 0
'10' & 0 = 0
'10' & 10 = 10
'10' & 0.0 = 0
'10' & 10.0 = 10
'10' & 3.14 = 2
'10' & '0' = base64:MA==
'10' & '10' = base64:MTA=
'10' & '010' = base64:MDA=
'10' & '10 elephants' = base64:MTA=
'10' & 'foo' = base64:ICA=
'10' & array ( ) = 0
'10' & array ( 0 => 1 ) = 0
'10' & array ( 0 => 1, 1 => 100 ) = 0
'10' & array ( 'foo' => 1, 'bar' => 2 ) = 0
'10' & array ( 'bar' => 1, 'foo' => 2 ) = 0
'10' & (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
'10' & (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
'10' & (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
'10' & DateTime = 0 - Notice Object of class DateTime could not be converted to int
'10' & resource = 2
'10' & NULL = 0
'010' & false = 0
'010' & true = 0
'010' & 0 = 0
'010' & 10 = 10
'010' & 0.0 = 0
'010' & 10.0 = 10
'010' & 3.14 = 2
'010' & '0' = base64:MA==
'010' & '10' = base64:MDA=
'010' & '010' = base64:MDEw
'010' & '10 elephants' = base64:MDAg
'010' & 'foo' = base64:ICEg
'010' & array ( ) = 0
'010' & array ( 0 => 1 ) = 0
'010' & array ( 0 => 1, 1 => 100 ) = 0
'010' & array ( 'foo' => 1, 'bar' => 2 ) = 0
'010' & array ( 'bar' => 1, 'foo' => 2 ) = 0
'010' & (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
'010' & (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
'010' & (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
'010' & DateTime = 0 - Notice Object of class DateTime could not be converted to int
'010' & resource = 2
'010' & NULL = 0
'10 elephants' & false = 0 - Notice A non well formed numeric value encountered
'10 elephants' & true = 0 - Notice A non well formed numeric value encountered
'10 elephants' & 0 = 0 - Notice A non well formed numeric value encountered
'10 elephants' & 10 = 10 - Notice A non well formed numeric value encountered
'10 elephants' & 0.0 = 0 - Notice A non well formed numeric value encountered
'10 elephants' & 10.0 = 10 - Notice A non well formed numeric value encountered
'10 elephants' & 3.14 = 2 - Notice A non well formed numeric value encountered
'10 elephants' & '0' = base64:MA==
'10 elephants' & '10' = base64:MTA=
'10 elephants' & '010' = base64:MDAg
'10 elephants' & '10 elephants' = base64:MTAgZWxlcGhhbnRz
'10 elephants' & 'foo' = base64:ICAg
'10 elephants' & array ( ) = 0 - Notice A non well formed numeric value encountered
'10 elephants' & array ( 0 => 1 ) = 0 - Notice A non well formed numeric value encountered
'10 elephants' & array ( 0 => 1, 1 => 100 ) = 0 - Notice A non well formed numeric value encountered
'10 elephants' & array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice A non well formed numeric value encountered
'10 elephants' & array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice A non well formed numeric value encountered
'10 elephants' & (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
'10 elephants' & (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
'10 elephants' & (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
'10 elephants' & DateTime = 0 - Notice Object of class DateTime could not be converted to int
'10 elephants' & resource = 2 - Notice A non well formed numeric value encountered
'10 elephants' & NULL = 0 - Notice A non well formed numeric value encountered
'foo' & false = 0 - Warning A non-numeric value encountered
'foo' & true = 0 - Warning A non-numeric value encountered
'foo' & 0 = 0 - Warning A non-numeric value encountered
'foo' & 10 = 0 - Warning A non-numeric value encountered
'foo' & 0.0 = 0 - Warning A non-numeric value encountered
'foo' & 10.0 = 0 - Warning A non-numeric value encountered
'foo' & 3.14 = 0 - Warning A non-numeric value encountered
'foo' & '0' = base64:IA==
'foo' & '10' = base64:ICA=
'foo' & '010' = base64:ICEg
'foo' & '10 elephants' = base64:ICAg
'foo' & 'foo' = base64:Zm9v
'foo' & array ( ) = 0 - Warning A non-numeric value encountered
'foo' & array ( 0 => 1 ) = 0 - Warning A non-numeric value encountered
'foo' & array ( 0 => 1, 1 => 100 ) = 0 - Warning A non-numeric value encountered
'foo' & array ( 'foo' => 1, 'bar' => 2 ) = 0 - Warning A non-numeric value encountered
'foo' & array ( 'bar' => 1, 'foo' => 2 ) = 0 - Warning A non-numeric value encountered
'foo' & (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
'foo' & (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
'foo' & (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
'foo' & DateTime = 0 - Notice Object of class DateTime could not be converted to int
'foo' & resource = 0 - Warning A non-numeric value encountered
'foo' & NULL = 0 - Warning A non-numeric value encountered
array ( ) & false = 0
array ( ) & true = 0
array ( ) & 0 = 0
array ( ) & 10 = 0
array ( ) & 0.0 = 0
array ( ) & 10.0 = 0
array ( ) & 3.14 = 0
array ( ) & '0' = 0
array ( ) & '10' = 0
array ( ) & '010' = 0
array ( ) & '10 elephants' = 0 - Notice A non well formed numeric value encountered
array ( ) & 'foo' = 0 - Warning A non-numeric value encountered
array ( ) & array ( ) = 0
array ( ) & array ( 0 => 1 ) = 0
array ( ) & array ( 0 => 1, 1 => 100 ) = 0
array ( ) & array ( 'foo' => 1, 'bar' => 2 ) = 0
array ( ) & array ( 'bar' => 1, 'foo' => 2 ) = 0
array ( ) & (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
array ( ) & (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
array ( ) & (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
array ( ) & DateTime = 0 - Notice Object of class DateTime could not be converted to int
array ( ) & resource = 0
array ( ) & NULL = 0
array ( 0 => 1 ) & false = 0
array ( 0 => 1 ) & true = 1
array ( 0 => 1 ) & 0 = 0
array ( 0 => 1 ) & 10 = 0
array ( 0 => 1 ) & 0.0 = 0
array ( 0 => 1 ) & 10.0 = 0
array ( 0 => 1 ) & 3.14 = 1
array ( 0 => 1 ) & '0' = 0
array ( 0 => 1 ) & '10' = 0
array ( 0 => 1 ) & '010' = 0
array ( 0 => 1 ) & '10 elephants' = 0 - Notice A non well formed numeric value encountered
array ( 0 => 1 ) & 'foo' = 0 - Warning A non-numeric value encountered
array ( 0 => 1 ) & array ( ) = 0
array ( 0 => 1 ) & array ( 0 => 1 ) = 1
array ( 0 => 1 ) & array ( 0 => 1, 1 => 100 ) = 1
array ( 0 => 1 ) & array ( 'foo' => 1, 'bar' => 2 ) = 1
array ( 0 => 1 ) & array ( 'bar' => 1, 'foo' => 2 ) = 1
array ( 0 => 1 ) & (object) array ( ) = 1 - Notice Object of class stdClass could not be converted to int
array ( 0 => 1 ) & (object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
array ( 0 => 1 ) & (object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
array ( 0 => 1 ) & DateTime = 1 - Notice Object of class DateTime could not be converted to int
array ( 0 => 1 ) & resource = 0
array ( 0 => 1 ) & NULL = 0
array ( 0 => 1, 1 => 100 ) & false = 0
array ( 0 => 1, 1 => 100 ) & true = 1
array ( 0 => 1, 1 => 100 ) & 0 = 0
array ( 0 => 1, 1 => 100 ) & 10 = 0
array ( 0 => 1, 1 => 100 ) & 0.0 = 0
array ( 0 => 1, 1 => 100 ) & 10.0 = 0
array ( 0 => 1, 1 => 100 ) & 3.14 = 1
array ( 0 => 1, 1 => 100 ) & '0' = 0
array ( 0 => 1, 1 => 100 ) & '10' = 0
array ( 0 => 1, 1 => 100 ) & '010' = 0
array ( 0 => 1, 1 => 100 ) & '10 elephants' = 0 - Notice A non well formed numeric value encountered
array ( 0 => 1, 1 => 100 ) & 'foo' = 0 - Warning A non-numeric value encountered
array ( 0 => 1, 1 => 100 ) & array ( ) = 0
array ( 0 => 1, 1 => 100 ) & array ( 0 => 1 ) = 1
array ( 0 => 1, 1 => 100 ) & array ( 0 => 1, 1 => 100 ) = 1
array ( 0 => 1, 1 => 100 ) & array ( 'foo' => 1, 'bar' => 2 ) = 1
array ( 0 => 1, 1 => 100 ) & array ( 'bar' => 1, 'foo' => 2 ) = 1
array ( 0 => 1, 1 => 100 ) & (object) array ( ) = 1 - Notice Object of class stdClass could not be converted to int
array ( 0 => 1, 1 => 100 ) & (object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
array ( 0 => 1, 1 => 100 ) & (object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
array ( 0 => 1, 1 => 100 ) & DateTime = 1 - Notice Object of class DateTime could not be converted to int
array ( 0 => 1, 1 => 100 ) & resource = 0
array ( 0 => 1, 1 => 100 ) & NULL = 0
array ( 'foo' => 1, 'bar' => 2 ) & false = 0
array ( 'foo' => 1, 'bar' => 2 ) & true = 1
array ( 'foo' => 1, 'bar' => 2 ) & 0 = 0
array ( 'foo' => 1, 'bar' => 2 ) & 10 = 0
array ( 'foo' => 1, 'bar' => 2 ) & 0.0 = 0
array ( 'foo' => 1, 'bar' => 2 ) & 10.0 = 0
array ( 'foo' => 1, 'bar' => 2 ) & 3.14 = 1
array ( 'foo' => 1, 'bar' => 2 ) & '0' = 0
array ( 'foo' => 1, 'bar' => 2 ) & '10' = 0
array ( 'foo' => 1, 'bar' => 2 ) & '010' = 0
array ( 'foo' => 1, 'bar' => 2 ) & '10 elephants' = 0 - Notice A non well formed numeric value encountered
array ( 'foo' => 1, 'bar' => 2 ) & 'foo' = 0 - Warning A non-numeric value encountered
array ( 'foo' => 1, 'bar' => 2 ) & array ( ) = 0
array ( 'foo' => 1, 'bar' => 2 ) & array ( 0 => 1 ) = 1
array ( 'foo' => 1, 'bar' => 2 ) & array ( 0 => 1, 1 => 100 ) = 1
array ( 'foo' => 1, 'bar' => 2 ) & array ( 'foo' => 1, 'bar' => 2 ) = 1
array ( 'foo' => 1, 'bar' => 2 ) & array ( 'bar' => 1, 'foo' => 2 ) = 1
array ( 'foo' => 1, 'bar' => 2 ) & (object) array ( ) = 1 - Notice Object of class stdClass could not be converted to int
array ( 'foo' => 1, 'bar' => 2 ) & (object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
array ( 'foo' => 1, 'bar' => 2 ) & (object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
array ( 'foo' => 1, 'bar' => 2 ) & DateTime = 1 - Notice Object of class DateTime could not be converted to int
array ( 'foo' => 1, 'bar' => 2 ) & resource = 0
array ( 'foo' => 1, 'bar' => 2 ) & NULL = 0
array ( 'bar' => 1, 'foo' => 2 ) & false = 0
array ( 'bar' => 1, 'foo' => 2 ) & true = 1
array ( 'bar' => 1, 'foo' => 2 ) & 0 = 0
array ( 'bar' => 1, 'foo' => 2 ) & 10 = 0
array ( 'bar' => 1, 'foo' => 2 ) & 0.0 = 0
array ( 'bar' => 1, 'foo' => 2 ) & 10.0 = 0
array ( 'bar' => 1, 'foo' => 2 ) & 3.14 = 1
array ( 'bar' => 1, 'foo' => 2 ) & '0' = 0
array ( 'bar' => 1, 'foo' => 2 ) & '10' = 0
array ( 'bar' => 1, 'foo' => 2 ) & '010' = 0
array ( 'bar' => 1, 'foo' => 2 ) & '10 elephants' = 0 - Notice A non well formed numeric value encountered
array ( 'bar' => 1, 'foo' => 2 ) & 'foo' = 0 - Warning A non-numeric value encountered
array ( 'bar' => 1, 'foo' => 2 ) & array ( ) = 0
array ( 'bar' => 1, 'foo' => 2 ) & array ( 0 => 1 ) = 1
array ( 'bar' => 1, 'foo' => 2 ) & array ( 0 => 1, 1 => 100 ) = 1
array ( 'bar' => 1, 'foo' => 2 ) & array ( 'foo' => 1, 'bar' => 2 ) = 1
array ( 'bar' => 1, 'foo' => 2 ) & array ( 'bar' => 1, 'foo' => 2 ) = 1
array ( 'bar' => 1, 'foo' => 2 ) & (object) array ( ) = 1 - Notice Object of class stdClass could not be converted to int
array ( 'bar' => 1, 'foo' => 2 ) & (object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
array ( 'bar' => 1, 'foo' => 2 ) & (object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
array ( 'bar' => 1, 'foo' => 2 ) & DateTime = 1 - Notice Object of class DateTime could not be converted to int
array ( 'bar' => 1, 'foo' => 2 ) & resource = 0
array ( 'bar' => 1, 'foo' => 2 ) & NULL = 0
(object) array ( ) & false = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) & true = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( ) & 0 = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) & 10 = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) & 0.0 = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) & 10.0 = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) & 3.14 = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( ) & '0' = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) & '10' = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) & '010' = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) & '10 elephants' = 0 - Notice A non well formed numeric value encountered
(object) array ( ) & 'foo' = 0 - Warning A non-numeric value encountered
(object) array ( ) & array ( ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) & array ( 0 => 1 ) = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( ) & array ( 0 => 1, 1 => 100 ) = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( ) & array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( ) & array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( ) & (object) array ( ) = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( ) & (object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( ) & (object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( ) & DateTime = 1 - Notice Object of class DateTime could not be converted to int
(object) array ( ) & resource = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) & NULL = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) & false = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) & true = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) & 0 = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) & 10 = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) & 0.0 = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) & 10.0 = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) & 3.14 = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) & '0' = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) & '10' = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) & '010' = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) & '10 elephants' = 0 - Notice A non well formed numeric value encountered
(object) array ( 'foo' => 1, 'bar' => 2 ) & 'foo' = 0 - Warning A non-numeric value encountered
(object) array ( 'foo' => 1, 'bar' => 2 ) & array ( ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) & array ( 0 => 1 ) = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) & array ( 0 => 1, 1 => 100 ) = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) & array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) & array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) & (object) array ( ) = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) & (object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) & (object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) & DateTime = 1 - Notice Object of class DateTime could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) & resource = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) & NULL = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) & false = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) & true = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) & 0 = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) & 10 = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) & 0.0 = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) & 10.0 = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) & 3.14 = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) & '0' = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) & '10' = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) & '010' = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) & '10 elephants' = 0 - Notice A non well formed numeric value encountered
(object) array ( 'bar' => 1, 'foo' => 2 ) & 'foo' = 0 - Warning A non-numeric value encountered
(object) array ( 'bar' => 1, 'foo' => 2 ) & array ( ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) & array ( 0 => 1 ) = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) & array ( 0 => 1, 1 => 100 ) = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) & array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) & array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) & (object) array ( ) = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) & (object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) & (object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) & DateTime = 1 - Notice Object of class DateTime could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) & resource = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) & NULL = 0 - Notice Object of class stdClass could not be converted to int
DateTime & false = 0 - Notice Object of class DateTime could not be converted to int
DateTime & true = 1 - Notice Object of class DateTime could not be converted to int
DateTime & 0 = 0 - Notice Object of class DateTime could not be converted to int
DateTime & 10 = 0 - Notice Object of class DateTime could not be converted to int
DateTime & 0.0 = 0 - Notice Object of class DateTime could not be converted to int
DateTime & 10.0 = 0 - Notice Object of class DateTime could not be converted to int
DateTime & 3.14 = 1 - Notice Object of class DateTime could not be converted to int
DateTime & '0' = 0 - Notice Object of class DateTime could not be converted to int
DateTime & '10' = 0 - Notice Object of class DateTime could not be converted to int
DateTime & '010' = 0 - Notice Object of class DateTime could not be converted to int
DateTime & '10 elephants' = 0 - Notice A non well formed numeric value encountered
DateTime & 'foo' = 0 - Warning A non-numeric value encountered
DateTime & array ( ) = 0 - Notice Object of class DateTime could not be converted to int
DateTime & array ( 0 => 1 ) = 1 - Notice Object of class DateTime could not be converted to int
DateTime & array ( 0 => 1, 1 => 100 ) = 1 - Notice Object of class DateTime could not be converted to int
DateTime & array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class DateTime could not be converted to int
DateTime & array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class DateTime could not be converted to int
DateTime & (object) array ( ) = 1 - Notice Object of class stdClass could not be converted to int
DateTime & (object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
DateTime & (object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
DateTime & DateTime = 1 - Notice Object of class DateTime could not be converted to int
DateTime & resource = 0 - Notice Object of class DateTime could not be converted to int
DateTime & NULL = 0 - Notice Object of class DateTime could not be converted to int
resource & false = 0
resource & true = 0
resource & 0 = 0
resource & 10 = 2
resource & 0.0 = 0
resource & 10.0 = 2
resource & 3.14 = 2
resource & '0' = 0
resource & '10' = 2
resource & '010' = 2
resource & '10 elephants' = 2 - Notice A non well formed numeric value encountered
resource & 'foo' = 0 - Warning A non-numeric value encountered
resource & array ( ) = 0
resource & array ( 0 => 1 ) = 0
resource & array ( 0 => 1, 1 => 100 ) = 0
resource & array ( 'foo' => 1, 'bar' => 2 ) = 0
resource & array ( 'bar' => 1, 'foo' => 2 ) = 0
resource & (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
resource & (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
resource & (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
resource & DateTime = 0 - Notice Object of class DateTime could not be converted to int
resource & resource = 6
resource & NULL = 0
NULL & false = 0
NULL & true = 0
NULL & 0 = 0
NULL & 10 = 0
NULL & 0.0 = 0
NULL & 10.0 = 0
NULL & 3.14 = 0
NULL & '0' = 0
NULL & '10' = 0
NULL & '010' = 0
NULL & '10 elephants' = 0 - Notice A non well formed numeric value encountered
NULL & 'foo' = 0 - Warning A non-numeric value encountered
NULL & array ( ) = 0
NULL & array ( 0 => 1 ) = 0
NULL & array ( 0 => 1, 1 => 100 ) = 0
NULL & array ( 'foo' => 1, 'bar' => 2 ) = 0
NULL & array ( 'bar' => 1, 'foo' => 2 ) = 0
NULL & (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
NULL & (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
NULL & (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
NULL & DateTime = 0 - Notice Object of class DateTime could not be converted to int
NULL & resource = 0
NULL & NULL = 0