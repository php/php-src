--TEST--
bitwise or operator with strict operators
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a | $b', function($a, $b) { return $a | $b; }, 'var_out_base64');

--EXPECT--
false | false - TypeError Unsupported operand type bool for bitwise or
false | true - TypeError Unsupported operand type bool for bitwise or
false | 0 - TypeError Unsupported operand type bool for bitwise or
false | 10 - TypeError Unsupported operand type bool for bitwise or
false | 0.0 - TypeError Unsupported operand type bool for bitwise or
false | 10.0 - TypeError Unsupported operand type bool for bitwise or
false | 3.14 - TypeError Unsupported operand type bool for bitwise or
false | '0' - TypeError Unsupported operand type bool for bitwise or
false | '10' - TypeError Unsupported operand type bool for bitwise or
false | '010' - TypeError Unsupported operand type bool for bitwise or
false | '10 elephants' - TypeError Unsupported operand type bool for bitwise or
false | 'foo' - TypeError Unsupported operand type bool for bitwise or
false | array ( ) - TypeError Unsupported operand type bool for bitwise or
false | array ( 0 => 1 ) - TypeError Unsupported operand type bool for bitwise or
false | array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for bitwise or
false | array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for bitwise or
false | array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for bitwise or
false | (object) array ( ) - TypeError Unsupported operand type bool for bitwise or
false | (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for bitwise or
false | (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for bitwise or
false | DateTime - TypeError Unsupported operand type bool for bitwise or
false | resource - TypeError Unsupported operand type bool for bitwise or
false | NULL - TypeError Unsupported operand type bool for bitwise or
true | false - TypeError Unsupported operand type bool for bitwise or
true | true - TypeError Unsupported operand type bool for bitwise or
true | 0 - TypeError Unsupported operand type bool for bitwise or
true | 10 - TypeError Unsupported operand type bool for bitwise or
true | 0.0 - TypeError Unsupported operand type bool for bitwise or
true | 10.0 - TypeError Unsupported operand type bool for bitwise or
true | 3.14 - TypeError Unsupported operand type bool for bitwise or
true | '0' - TypeError Unsupported operand type bool for bitwise or
true | '10' - TypeError Unsupported operand type bool for bitwise or
true | '010' - TypeError Unsupported operand type bool for bitwise or
true | '10 elephants' - TypeError Unsupported operand type bool for bitwise or
true | 'foo' - TypeError Unsupported operand type bool for bitwise or
true | array ( ) - TypeError Unsupported operand type bool for bitwise or
true | array ( 0 => 1 ) - TypeError Unsupported operand type bool for bitwise or
true | array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for bitwise or
true | array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for bitwise or
true | array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for bitwise or
true | (object) array ( ) - TypeError Unsupported operand type bool for bitwise or
true | (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for bitwise or
true | (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for bitwise or
true | DateTime - TypeError Unsupported operand type bool for bitwise or
true | resource - TypeError Unsupported operand type bool for bitwise or
true | NULL - TypeError Unsupported operand type bool for bitwise or
0 | false - TypeError Unsupported operand type bool for bitwise or
0 | true - TypeError Unsupported operand type bool for bitwise or
0 | 0 = 0
0 | 10 = 10
0 | 0.0 - TypeError Unsupported operand type float for bitwise or
0 | 10.0 - TypeError Unsupported operand type float for bitwise or
0 | 3.14 - TypeError Unsupported operand type float for bitwise or
0 | '0' - TypeError Operand type mismatch int and string for bitwise or
0 | '10' - TypeError Operand type mismatch int and string for bitwise or
0 | '010' - TypeError Operand type mismatch int and string for bitwise or
0 | '10 elephants' - TypeError Operand type mismatch int and string for bitwise or
0 | 'foo' - TypeError Operand type mismatch int and string for bitwise or
0 | array ( ) - TypeError Unsupported operand type array for bitwise or
0 | array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise or
0 | array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise or
0 | array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise or
0 | array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise or
0 | (object) array ( ) - TypeError Unsupported operand type object for bitwise or
0 | (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise or
0 | (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise or
0 | DateTime - TypeError Unsupported operand type object for bitwise or
0 | resource - TypeError Unsupported operand type resource for bitwise or
0 | NULL - TypeError Unsupported operand type null for bitwise or
10 | false - TypeError Unsupported operand type bool for bitwise or
10 | true - TypeError Unsupported operand type bool for bitwise or
10 | 0 = 10
10 | 10 = 10
10 | 0.0 - TypeError Unsupported operand type float for bitwise or
10 | 10.0 - TypeError Unsupported operand type float for bitwise or
10 | 3.14 - TypeError Unsupported operand type float for bitwise or
10 | '0' - TypeError Operand type mismatch int and string for bitwise or
10 | '10' - TypeError Operand type mismatch int and string for bitwise or
10 | '010' - TypeError Operand type mismatch int and string for bitwise or
10 | '10 elephants' - TypeError Operand type mismatch int and string for bitwise or
10 | 'foo' - TypeError Operand type mismatch int and string for bitwise or
10 | array ( ) - TypeError Unsupported operand type array for bitwise or
10 | array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise or
10 | array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise or
10 | array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise or
10 | array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise or
10 | (object) array ( ) - TypeError Unsupported operand type object for bitwise or
10 | (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise or
10 | (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise or
10 | DateTime - TypeError Unsupported operand type object for bitwise or
10 | resource - TypeError Unsupported operand type resource for bitwise or
10 | NULL - TypeError Unsupported operand type null for bitwise or
0.0 | false - TypeError Unsupported operand type float for bitwise or
0.0 | true - TypeError Unsupported operand type float for bitwise or
0.0 | 0 - TypeError Unsupported operand type float for bitwise or
0.0 | 10 - TypeError Unsupported operand type float for bitwise or
0.0 | 0.0 - TypeError Unsupported operand type float for bitwise or
0.0 | 10.0 - TypeError Unsupported operand type float for bitwise or
0.0 | 3.14 - TypeError Unsupported operand type float for bitwise or
0.0 | '0' - TypeError Unsupported operand type float for bitwise or
0.0 | '10' - TypeError Unsupported operand type float for bitwise or
0.0 | '010' - TypeError Unsupported operand type float for bitwise or
0.0 | '10 elephants' - TypeError Unsupported operand type float for bitwise or
0.0 | 'foo' - TypeError Unsupported operand type float for bitwise or
0.0 | array ( ) - TypeError Unsupported operand type float for bitwise or
0.0 | array ( 0 => 1 ) - TypeError Unsupported operand type float for bitwise or
0.0 | array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type float for bitwise or
0.0 | array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bitwise or
0.0 | array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bitwise or
0.0 | (object) array ( ) - TypeError Unsupported operand type float for bitwise or
0.0 | (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bitwise or
0.0 | (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bitwise or
0.0 | DateTime - TypeError Unsupported operand type float for bitwise or
0.0 | resource - TypeError Unsupported operand type float for bitwise or
0.0 | NULL - TypeError Unsupported operand type float for bitwise or
10.0 | false - TypeError Unsupported operand type float for bitwise or
10.0 | true - TypeError Unsupported operand type float for bitwise or
10.0 | 0 - TypeError Unsupported operand type float for bitwise or
10.0 | 10 - TypeError Unsupported operand type float for bitwise or
10.0 | 0.0 - TypeError Unsupported operand type float for bitwise or
10.0 | 10.0 - TypeError Unsupported operand type float for bitwise or
10.0 | 3.14 - TypeError Unsupported operand type float for bitwise or
10.0 | '0' - TypeError Unsupported operand type float for bitwise or
10.0 | '10' - TypeError Unsupported operand type float for bitwise or
10.0 | '010' - TypeError Unsupported operand type float for bitwise or
10.0 | '10 elephants' - TypeError Unsupported operand type float for bitwise or
10.0 | 'foo' - TypeError Unsupported operand type float for bitwise or
10.0 | array ( ) - TypeError Unsupported operand type float for bitwise or
10.0 | array ( 0 => 1 ) - TypeError Unsupported operand type float for bitwise or
10.0 | array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type float for bitwise or
10.0 | array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bitwise or
10.0 | array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bitwise or
10.0 | (object) array ( ) - TypeError Unsupported operand type float for bitwise or
10.0 | (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bitwise or
10.0 | (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bitwise or
10.0 | DateTime - TypeError Unsupported operand type float for bitwise or
10.0 | resource - TypeError Unsupported operand type float for bitwise or
10.0 | NULL - TypeError Unsupported operand type float for bitwise or
3.14 | false - TypeError Unsupported operand type float for bitwise or
3.14 | true - TypeError Unsupported operand type float for bitwise or
3.14 | 0 - TypeError Unsupported operand type float for bitwise or
3.14 | 10 - TypeError Unsupported operand type float for bitwise or
3.14 | 0.0 - TypeError Unsupported operand type float for bitwise or
3.14 | 10.0 - TypeError Unsupported operand type float for bitwise or
3.14 | 3.14 - TypeError Unsupported operand type float for bitwise or
3.14 | '0' - TypeError Unsupported operand type float for bitwise or
3.14 | '10' - TypeError Unsupported operand type float for bitwise or
3.14 | '010' - TypeError Unsupported operand type float for bitwise or
3.14 | '10 elephants' - TypeError Unsupported operand type float for bitwise or
3.14 | 'foo' - TypeError Unsupported operand type float for bitwise or
3.14 | array ( ) - TypeError Unsupported operand type float for bitwise or
3.14 | array ( 0 => 1 ) - TypeError Unsupported operand type float for bitwise or
3.14 | array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type float for bitwise or
3.14 | array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bitwise or
3.14 | array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bitwise or
3.14 | (object) array ( ) - TypeError Unsupported operand type float for bitwise or
3.14 | (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bitwise or
3.14 | (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bitwise or
3.14 | DateTime - TypeError Unsupported operand type float for bitwise or
3.14 | resource - TypeError Unsupported operand type float for bitwise or
3.14 | NULL - TypeError Unsupported operand type float for bitwise or
'0' | false - TypeError Unsupported operand type bool for bitwise or
'0' | true - TypeError Unsupported operand type bool for bitwise or
'0' | 0 - TypeError Operand type mismatch string and int for bitwise or
'0' | 10 - TypeError Operand type mismatch string and int for bitwise or
'0' | 0.0 - TypeError Unsupported operand type float for bitwise or
'0' | 10.0 - TypeError Unsupported operand type float for bitwise or
'0' | 3.14 - TypeError Unsupported operand type float for bitwise or
'0' | '0' = base64:MA==
'0' | '10' = base64:MTA=
'0' | '010' = base64:MDEw
'0' | '10 elephants' = base64:MTAgZWxlcGhhbnRz
'0' | 'foo' = base64:dm9v
'0' | array ( ) - TypeError Unsupported operand type array for bitwise or
'0' | array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise or
'0' | array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise or
'0' | array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise or
'0' | array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise or
'0' | (object) array ( ) - TypeError Unsupported operand type object for bitwise or
'0' | (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise or
'0' | (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise or
'0' | DateTime - TypeError Unsupported operand type object for bitwise or
'0' | resource - TypeError Unsupported operand type resource for bitwise or
'0' | NULL - TypeError Unsupported operand type null for bitwise or
'10' | false - TypeError Unsupported operand type bool for bitwise or
'10' | true - TypeError Unsupported operand type bool for bitwise or
'10' | 0 - TypeError Operand type mismatch string and int for bitwise or
'10' | 10 - TypeError Operand type mismatch string and int for bitwise or
'10' | 0.0 - TypeError Unsupported operand type float for bitwise or
'10' | 10.0 - TypeError Unsupported operand type float for bitwise or
'10' | 3.14 - TypeError Unsupported operand type float for bitwise or
'10' | '0' = base64:MTA=
'10' | '10' = base64:MTA=
'10' | '010' = base64:MTEw
'10' | '10 elephants' = base64:MTAgZWxlcGhhbnRz
'10' | 'foo' = base64:d39v
'10' | array ( ) - TypeError Unsupported operand type array for bitwise or
'10' | array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise or
'10' | array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise or
'10' | array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise or
'10' | array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise or
'10' | (object) array ( ) - TypeError Unsupported operand type object for bitwise or
'10' | (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise or
'10' | (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise or
'10' | DateTime - TypeError Unsupported operand type object for bitwise or
'10' | resource - TypeError Unsupported operand type resource for bitwise or
'10' | NULL - TypeError Unsupported operand type null for bitwise or
'010' | false - TypeError Unsupported operand type bool for bitwise or
'010' | true - TypeError Unsupported operand type bool for bitwise or
'010' | 0 - TypeError Operand type mismatch string and int for bitwise or
'010' | 10 - TypeError Operand type mismatch string and int for bitwise or
'010' | 0.0 - TypeError Unsupported operand type float for bitwise or
'010' | 10.0 - TypeError Unsupported operand type float for bitwise or
'010' | 3.14 - TypeError Unsupported operand type float for bitwise or
'010' | '0' = base64:MDEw
'010' | '10' = base64:MTEw
'010' | '010' = base64:MDEw
'010' | '10 elephants' = base64:MTEwZWxlcGhhbnRz
'010' | 'foo' = base64:dn9/
'010' | array ( ) - TypeError Unsupported operand type array for bitwise or
'010' | array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise or
'010' | array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise or
'010' | array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise or
'010' | array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise or
'010' | (object) array ( ) - TypeError Unsupported operand type object for bitwise or
'010' | (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise or
'010' | (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise or
'010' | DateTime - TypeError Unsupported operand type object for bitwise or
'010' | resource - TypeError Unsupported operand type resource for bitwise or
'010' | NULL - TypeError Unsupported operand type null for bitwise or
'10 elephants' | false - TypeError Unsupported operand type bool for bitwise or
'10 elephants' | true - TypeError Unsupported operand type bool for bitwise or
'10 elephants' | 0 - TypeError Operand type mismatch string and int for bitwise or
'10 elephants' | 10 - TypeError Operand type mismatch string and int for bitwise or
'10 elephants' | 0.0 - TypeError Unsupported operand type float for bitwise or
'10 elephants' | 10.0 - TypeError Unsupported operand type float for bitwise or
'10 elephants' | 3.14 - TypeError Unsupported operand type float for bitwise or
'10 elephants' | '0' = base64:MTAgZWxlcGhhbnRz
'10 elephants' | '10' = base64:MTAgZWxlcGhhbnRz
'10 elephants' | '010' = base64:MTEwZWxlcGhhbnRz
'10 elephants' | '10 elephants' = base64:MTAgZWxlcGhhbnRz
'10 elephants' | 'foo' = base64:d39vZWxlcGhhbnRz
'10 elephants' | array ( ) - TypeError Unsupported operand type array for bitwise or
'10 elephants' | array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise or
'10 elephants' | array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise or
'10 elephants' | array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise or
'10 elephants' | array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise or
'10 elephants' | (object) array ( ) - TypeError Unsupported operand type object for bitwise or
'10 elephants' | (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise or
'10 elephants' | (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise or
'10 elephants' | DateTime - TypeError Unsupported operand type object for bitwise or
'10 elephants' | resource - TypeError Unsupported operand type resource for bitwise or
'10 elephants' | NULL - TypeError Unsupported operand type null for bitwise or
'foo' | false - TypeError Unsupported operand type bool for bitwise or
'foo' | true - TypeError Unsupported operand type bool for bitwise or
'foo' | 0 - TypeError Operand type mismatch string and int for bitwise or
'foo' | 10 - TypeError Operand type mismatch string and int for bitwise or
'foo' | 0.0 - TypeError Unsupported operand type float for bitwise or
'foo' | 10.0 - TypeError Unsupported operand type float for bitwise or
'foo' | 3.14 - TypeError Unsupported operand type float for bitwise or
'foo' | '0' = base64:dm9v
'foo' | '10' = base64:d39v
'foo' | '010' = base64:dn9/
'foo' | '10 elephants' = base64:d39vZWxlcGhhbnRz
'foo' | 'foo' = base64:Zm9v
'foo' | array ( ) - TypeError Unsupported operand type array for bitwise or
'foo' | array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise or
'foo' | array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise or
'foo' | array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise or
'foo' | array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise or
'foo' | (object) array ( ) - TypeError Unsupported operand type object for bitwise or
'foo' | (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise or
'foo' | (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise or
'foo' | DateTime - TypeError Unsupported operand type object for bitwise or
'foo' | resource - TypeError Unsupported operand type resource for bitwise or
'foo' | NULL - TypeError Unsupported operand type null for bitwise or
array ( ) | false - TypeError Unsupported operand type array for bitwise or
array ( ) | true - TypeError Unsupported operand type array for bitwise or
array ( ) | 0 - TypeError Unsupported operand type array for bitwise or
array ( ) | 10 - TypeError Unsupported operand type array for bitwise or
array ( ) | 0.0 - TypeError Unsupported operand type array for bitwise or
array ( ) | 10.0 - TypeError Unsupported operand type array for bitwise or
array ( ) | 3.14 - TypeError Unsupported operand type array for bitwise or
array ( ) | '0' - TypeError Unsupported operand type array for bitwise or
array ( ) | '10' - TypeError Unsupported operand type array for bitwise or
array ( ) | '010' - TypeError Unsupported operand type array for bitwise or
array ( ) | '10 elephants' - TypeError Unsupported operand type array for bitwise or
array ( ) | 'foo' - TypeError Unsupported operand type array for bitwise or
array ( ) | array ( ) - TypeError Unsupported operand type array for bitwise or
array ( ) | array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise or
array ( ) | array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise or
array ( ) | array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise or
array ( ) | array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise or
array ( ) | (object) array ( ) - TypeError Unsupported operand type array for bitwise or
array ( ) | (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise or
array ( ) | (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise or
array ( ) | DateTime - TypeError Unsupported operand type array for bitwise or
array ( ) | resource - TypeError Unsupported operand type array for bitwise or
array ( ) | NULL - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1 ) | false - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1 ) | true - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1 ) | 0 - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1 ) | 10 - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1 ) | 0.0 - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1 ) | 10.0 - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1 ) | 3.14 - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1 ) | '0' - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1 ) | '10' - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1 ) | '010' - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1 ) | '10 elephants' - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1 ) | 'foo' - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1 ) | array ( ) - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1 ) | array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1 ) | array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1 ) | array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1 ) | array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1 ) | (object) array ( ) - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1 ) | (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1 ) | (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1 ) | DateTime - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1 ) | resource - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1 ) | NULL - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1, 1 => 100 ) | false - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1, 1 => 100 ) | true - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1, 1 => 100 ) | 0 - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1, 1 => 100 ) | 10 - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1, 1 => 100 ) | 0.0 - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1, 1 => 100 ) | 10.0 - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1, 1 => 100 ) | 3.14 - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1, 1 => 100 ) | '0' - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1, 1 => 100 ) | '10' - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1, 1 => 100 ) | '010' - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1, 1 => 100 ) | '10 elephants' - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1, 1 => 100 ) | 'foo' - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1, 1 => 100 ) | array ( ) - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1, 1 => 100 ) | array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1, 1 => 100 ) | array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1, 1 => 100 ) | array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1, 1 => 100 ) | array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1, 1 => 100 ) | (object) array ( ) - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1, 1 => 100 ) | (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1, 1 => 100 ) | (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1, 1 => 100 ) | DateTime - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1, 1 => 100 ) | resource - TypeError Unsupported operand type array for bitwise or
array ( 0 => 1, 1 => 100 ) | NULL - TypeError Unsupported operand type array for bitwise or
array ( 'foo' => 1, 'bar' => 2 ) | false - TypeError Unsupported operand type array for bitwise or
array ( 'foo' => 1, 'bar' => 2 ) | true - TypeError Unsupported operand type array for bitwise or
array ( 'foo' => 1, 'bar' => 2 ) | 0 - TypeError Unsupported operand type array for bitwise or
array ( 'foo' => 1, 'bar' => 2 ) | 10 - TypeError Unsupported operand type array for bitwise or
array ( 'foo' => 1, 'bar' => 2 ) | 0.0 - TypeError Unsupported operand type array for bitwise or
array ( 'foo' => 1, 'bar' => 2 ) | 10.0 - TypeError Unsupported operand type array for bitwise or
array ( 'foo' => 1, 'bar' => 2 ) | 3.14 - TypeError Unsupported operand type array for bitwise or
array ( 'foo' => 1, 'bar' => 2 ) | '0' - TypeError Unsupported operand type array for bitwise or
array ( 'foo' => 1, 'bar' => 2 ) | '10' - TypeError Unsupported operand type array for bitwise or
array ( 'foo' => 1, 'bar' => 2 ) | '010' - TypeError Unsupported operand type array for bitwise or
array ( 'foo' => 1, 'bar' => 2 ) | '10 elephants' - TypeError Unsupported operand type array for bitwise or
array ( 'foo' => 1, 'bar' => 2 ) | 'foo' - TypeError Unsupported operand type array for bitwise or
array ( 'foo' => 1, 'bar' => 2 ) | array ( ) - TypeError Unsupported operand type array for bitwise or
array ( 'foo' => 1, 'bar' => 2 ) | array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise or
array ( 'foo' => 1, 'bar' => 2 ) | array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise or
array ( 'foo' => 1, 'bar' => 2 ) | array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise or
array ( 'foo' => 1, 'bar' => 2 ) | array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise or
array ( 'foo' => 1, 'bar' => 2 ) | (object) array ( ) - TypeError Unsupported operand type array for bitwise or
array ( 'foo' => 1, 'bar' => 2 ) | (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise or
array ( 'foo' => 1, 'bar' => 2 ) | (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise or
array ( 'foo' => 1, 'bar' => 2 ) | DateTime - TypeError Unsupported operand type array for bitwise or
array ( 'foo' => 1, 'bar' => 2 ) | resource - TypeError Unsupported operand type array for bitwise or
array ( 'foo' => 1, 'bar' => 2 ) | NULL - TypeError Unsupported operand type array for bitwise or
array ( 'bar' => 1, 'foo' => 2 ) | false - TypeError Unsupported operand type array for bitwise or
array ( 'bar' => 1, 'foo' => 2 ) | true - TypeError Unsupported operand type array for bitwise or
array ( 'bar' => 1, 'foo' => 2 ) | 0 - TypeError Unsupported operand type array for bitwise or
array ( 'bar' => 1, 'foo' => 2 ) | 10 - TypeError Unsupported operand type array for bitwise or
array ( 'bar' => 1, 'foo' => 2 ) | 0.0 - TypeError Unsupported operand type array for bitwise or
array ( 'bar' => 1, 'foo' => 2 ) | 10.0 - TypeError Unsupported operand type array for bitwise or
array ( 'bar' => 1, 'foo' => 2 ) | 3.14 - TypeError Unsupported operand type array for bitwise or
array ( 'bar' => 1, 'foo' => 2 ) | '0' - TypeError Unsupported operand type array for bitwise or
array ( 'bar' => 1, 'foo' => 2 ) | '10' - TypeError Unsupported operand type array for bitwise or
array ( 'bar' => 1, 'foo' => 2 ) | '010' - TypeError Unsupported operand type array for bitwise or
array ( 'bar' => 1, 'foo' => 2 ) | '10 elephants' - TypeError Unsupported operand type array for bitwise or
array ( 'bar' => 1, 'foo' => 2 ) | 'foo' - TypeError Unsupported operand type array for bitwise or
array ( 'bar' => 1, 'foo' => 2 ) | array ( ) - TypeError Unsupported operand type array for bitwise or
array ( 'bar' => 1, 'foo' => 2 ) | array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise or
array ( 'bar' => 1, 'foo' => 2 ) | array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise or
array ( 'bar' => 1, 'foo' => 2 ) | array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise or
array ( 'bar' => 1, 'foo' => 2 ) | array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise or
array ( 'bar' => 1, 'foo' => 2 ) | (object) array ( ) - TypeError Unsupported operand type array for bitwise or
array ( 'bar' => 1, 'foo' => 2 ) | (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise or
array ( 'bar' => 1, 'foo' => 2 ) | (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise or
array ( 'bar' => 1, 'foo' => 2 ) | DateTime - TypeError Unsupported operand type array for bitwise or
array ( 'bar' => 1, 'foo' => 2 ) | resource - TypeError Unsupported operand type array for bitwise or
array ( 'bar' => 1, 'foo' => 2 ) | NULL - TypeError Unsupported operand type array for bitwise or
(object) array ( ) | false - TypeError Unsupported operand type object for bitwise or
(object) array ( ) | true - TypeError Unsupported operand type object for bitwise or
(object) array ( ) | 0 - TypeError Unsupported operand type object for bitwise or
(object) array ( ) | 10 - TypeError Unsupported operand type object for bitwise or
(object) array ( ) | 0.0 - TypeError Unsupported operand type object for bitwise or
(object) array ( ) | 10.0 - TypeError Unsupported operand type object for bitwise or
(object) array ( ) | 3.14 - TypeError Unsupported operand type object for bitwise or
(object) array ( ) | '0' - TypeError Unsupported operand type object for bitwise or
(object) array ( ) | '10' - TypeError Unsupported operand type object for bitwise or
(object) array ( ) | '010' - TypeError Unsupported operand type object for bitwise or
(object) array ( ) | '10 elephants' - TypeError Unsupported operand type object for bitwise or
(object) array ( ) | 'foo' - TypeError Unsupported operand type object for bitwise or
(object) array ( ) | array ( ) - TypeError Unsupported operand type object for bitwise or
(object) array ( ) | array ( 0 => 1 ) - TypeError Unsupported operand type object for bitwise or
(object) array ( ) | array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for bitwise or
(object) array ( ) | array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise or
(object) array ( ) | array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise or
(object) array ( ) | (object) array ( ) - TypeError Unsupported operand type object for bitwise or
(object) array ( ) | (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise or
(object) array ( ) | (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise or
(object) array ( ) | DateTime - TypeError Unsupported operand type object for bitwise or
(object) array ( ) | resource - TypeError Unsupported operand type object for bitwise or
(object) array ( ) | NULL - TypeError Unsupported operand type object for bitwise or
(object) array ( 'foo' => 1, 'bar' => 2 ) | false - TypeError Unsupported operand type object for bitwise or
(object) array ( 'foo' => 1, 'bar' => 2 ) | true - TypeError Unsupported operand type object for bitwise or
(object) array ( 'foo' => 1, 'bar' => 2 ) | 0 - TypeError Unsupported operand type object for bitwise or
(object) array ( 'foo' => 1, 'bar' => 2 ) | 10 - TypeError Unsupported operand type object for bitwise or
(object) array ( 'foo' => 1, 'bar' => 2 ) | 0.0 - TypeError Unsupported operand type object for bitwise or
(object) array ( 'foo' => 1, 'bar' => 2 ) | 10.0 - TypeError Unsupported operand type object for bitwise or
(object) array ( 'foo' => 1, 'bar' => 2 ) | 3.14 - TypeError Unsupported operand type object for bitwise or
(object) array ( 'foo' => 1, 'bar' => 2 ) | '0' - TypeError Unsupported operand type object for bitwise or
(object) array ( 'foo' => 1, 'bar' => 2 ) | '10' - TypeError Unsupported operand type object for bitwise or
(object) array ( 'foo' => 1, 'bar' => 2 ) | '010' - TypeError Unsupported operand type object for bitwise or
(object) array ( 'foo' => 1, 'bar' => 2 ) | '10 elephants' - TypeError Unsupported operand type object for bitwise or
(object) array ( 'foo' => 1, 'bar' => 2 ) | 'foo' - TypeError Unsupported operand type object for bitwise or
(object) array ( 'foo' => 1, 'bar' => 2 ) | array ( ) - TypeError Unsupported operand type object for bitwise or
(object) array ( 'foo' => 1, 'bar' => 2 ) | array ( 0 => 1 ) - TypeError Unsupported operand type object for bitwise or
(object) array ( 'foo' => 1, 'bar' => 2 ) | array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for bitwise or
(object) array ( 'foo' => 1, 'bar' => 2 ) | array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise or
(object) array ( 'foo' => 1, 'bar' => 2 ) | array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise or
(object) array ( 'foo' => 1, 'bar' => 2 ) | (object) array ( ) - TypeError Unsupported operand type object for bitwise or
(object) array ( 'foo' => 1, 'bar' => 2 ) | (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise or
(object) array ( 'foo' => 1, 'bar' => 2 ) | (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise or
(object) array ( 'foo' => 1, 'bar' => 2 ) | DateTime - TypeError Unsupported operand type object for bitwise or
(object) array ( 'foo' => 1, 'bar' => 2 ) | resource - TypeError Unsupported operand type object for bitwise or
(object) array ( 'foo' => 1, 'bar' => 2 ) | NULL - TypeError Unsupported operand type object for bitwise or
(object) array ( 'bar' => 1, 'foo' => 2 ) | false - TypeError Unsupported operand type object for bitwise or
(object) array ( 'bar' => 1, 'foo' => 2 ) | true - TypeError Unsupported operand type object for bitwise or
(object) array ( 'bar' => 1, 'foo' => 2 ) | 0 - TypeError Unsupported operand type object for bitwise or
(object) array ( 'bar' => 1, 'foo' => 2 ) | 10 - TypeError Unsupported operand type object for bitwise or
(object) array ( 'bar' => 1, 'foo' => 2 ) | 0.0 - TypeError Unsupported operand type object for bitwise or
(object) array ( 'bar' => 1, 'foo' => 2 ) | 10.0 - TypeError Unsupported operand type object for bitwise or
(object) array ( 'bar' => 1, 'foo' => 2 ) | 3.14 - TypeError Unsupported operand type object for bitwise or
(object) array ( 'bar' => 1, 'foo' => 2 ) | '0' - TypeError Unsupported operand type object for bitwise or
(object) array ( 'bar' => 1, 'foo' => 2 ) | '10' - TypeError Unsupported operand type object for bitwise or
(object) array ( 'bar' => 1, 'foo' => 2 ) | '010' - TypeError Unsupported operand type object for bitwise or
(object) array ( 'bar' => 1, 'foo' => 2 ) | '10 elephants' - TypeError Unsupported operand type object for bitwise or
(object) array ( 'bar' => 1, 'foo' => 2 ) | 'foo' - TypeError Unsupported operand type object for bitwise or
(object) array ( 'bar' => 1, 'foo' => 2 ) | array ( ) - TypeError Unsupported operand type object for bitwise or
(object) array ( 'bar' => 1, 'foo' => 2 ) | array ( 0 => 1 ) - TypeError Unsupported operand type object for bitwise or
(object) array ( 'bar' => 1, 'foo' => 2 ) | array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for bitwise or
(object) array ( 'bar' => 1, 'foo' => 2 ) | array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise or
(object) array ( 'bar' => 1, 'foo' => 2 ) | array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise or
(object) array ( 'bar' => 1, 'foo' => 2 ) | (object) array ( ) - TypeError Unsupported operand type object for bitwise or
(object) array ( 'bar' => 1, 'foo' => 2 ) | (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise or
(object) array ( 'bar' => 1, 'foo' => 2 ) | (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise or
(object) array ( 'bar' => 1, 'foo' => 2 ) | DateTime - TypeError Unsupported operand type object for bitwise or
(object) array ( 'bar' => 1, 'foo' => 2 ) | resource - TypeError Unsupported operand type object for bitwise or
(object) array ( 'bar' => 1, 'foo' => 2 ) | NULL - TypeError Unsupported operand type object for bitwise or
DateTime | false - TypeError Unsupported operand type object for bitwise or
DateTime | true - TypeError Unsupported operand type object for bitwise or
DateTime | 0 - TypeError Unsupported operand type object for bitwise or
DateTime | 10 - TypeError Unsupported operand type object for bitwise or
DateTime | 0.0 - TypeError Unsupported operand type object for bitwise or
DateTime | 10.0 - TypeError Unsupported operand type object for bitwise or
DateTime | 3.14 - TypeError Unsupported operand type object for bitwise or
DateTime | '0' - TypeError Unsupported operand type object for bitwise or
DateTime | '10' - TypeError Unsupported operand type object for bitwise or
DateTime | '010' - TypeError Unsupported operand type object for bitwise or
DateTime | '10 elephants' - TypeError Unsupported operand type object for bitwise or
DateTime | 'foo' - TypeError Unsupported operand type object for bitwise or
DateTime | array ( ) - TypeError Unsupported operand type object for bitwise or
DateTime | array ( 0 => 1 ) - TypeError Unsupported operand type object for bitwise or
DateTime | array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for bitwise or
DateTime | array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise or
DateTime | array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise or
DateTime | (object) array ( ) - TypeError Unsupported operand type object for bitwise or
DateTime | (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise or
DateTime | (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise or
DateTime | DateTime - TypeError Unsupported operand type object for bitwise or
DateTime | resource - TypeError Unsupported operand type object for bitwise or
DateTime | NULL - TypeError Unsupported operand type object for bitwise or
resource | false - TypeError Unsupported operand type resource for bitwise or
resource | true - TypeError Unsupported operand type resource for bitwise or
resource | 0 - TypeError Unsupported operand type resource for bitwise or
resource | 10 - TypeError Unsupported operand type resource for bitwise or
resource | 0.0 - TypeError Unsupported operand type resource for bitwise or
resource | 10.0 - TypeError Unsupported operand type resource for bitwise or
resource | 3.14 - TypeError Unsupported operand type resource for bitwise or
resource | '0' - TypeError Unsupported operand type resource for bitwise or
resource | '10' - TypeError Unsupported operand type resource for bitwise or
resource | '010' - TypeError Unsupported operand type resource for bitwise or
resource | '10 elephants' - TypeError Unsupported operand type resource for bitwise or
resource | 'foo' - TypeError Unsupported operand type resource for bitwise or
resource | array ( ) - TypeError Unsupported operand type resource for bitwise or
resource | array ( 0 => 1 ) - TypeError Unsupported operand type resource for bitwise or
resource | array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type resource for bitwise or
resource | array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for bitwise or
resource | array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for bitwise or
resource | (object) array ( ) - TypeError Unsupported operand type resource for bitwise or
resource | (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for bitwise or
resource | (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for bitwise or
resource | DateTime - TypeError Unsupported operand type resource for bitwise or
resource | resource - TypeError Unsupported operand type resource for bitwise or
resource | NULL - TypeError Unsupported operand type resource for bitwise or
NULL | false - TypeError Unsupported operand type null for bitwise or
NULL | true - TypeError Unsupported operand type null for bitwise or
NULL | 0 - TypeError Unsupported operand type null for bitwise or
NULL | 10 - TypeError Unsupported operand type null for bitwise or
NULL | 0.0 - TypeError Unsupported operand type null for bitwise or
NULL | 10.0 - TypeError Unsupported operand type null for bitwise or
NULL | 3.14 - TypeError Unsupported operand type null for bitwise or
NULL | '0' - TypeError Unsupported operand type null for bitwise or
NULL | '10' - TypeError Unsupported operand type null for bitwise or
NULL | '010' - TypeError Unsupported operand type null for bitwise or
NULL | '10 elephants' - TypeError Unsupported operand type null for bitwise or
NULL | 'foo' - TypeError Unsupported operand type null for bitwise or
NULL | array ( ) - TypeError Unsupported operand type null for bitwise or
NULL | array ( 0 => 1 ) - TypeError Unsupported operand type null for bitwise or
NULL | array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type null for bitwise or
NULL | array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for bitwise or
NULL | array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for bitwise or
NULL | (object) array ( ) - TypeError Unsupported operand type null for bitwise or
NULL | (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for bitwise or
NULL | (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for bitwise or
NULL | DateTime - TypeError Unsupported operand type null for bitwise or
NULL | resource - TypeError Unsupported operand type null for bitwise or
NULL | NULL - TypeError Unsupported operand type null for bitwise or