--TEST--
bitwise xor operator with strict operators
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a ^ $b', function($a, $b) { return $a ^ $b; }, 'var_out_base64');

--EXPECT--
false ^ false - TypeError Unsupported operand type bool for bitwise xor
false ^ true - TypeError Unsupported operand type bool for bitwise xor
false ^ 0 - TypeError Unsupported operand type bool for bitwise xor
false ^ 10 - TypeError Unsupported operand type bool for bitwise xor
false ^ 0.0 - TypeError Unsupported operand type bool for bitwise xor
false ^ 10.0 - TypeError Unsupported operand type bool for bitwise xor
false ^ 3.14 - TypeError Unsupported operand type bool for bitwise xor
false ^ '0' - TypeError Unsupported operand type bool for bitwise xor
false ^ '10' - TypeError Unsupported operand type bool for bitwise xor
false ^ '010' - TypeError Unsupported operand type bool for bitwise xor
false ^ '10 elephants' - TypeError Unsupported operand type bool for bitwise xor
false ^ 'foo' - TypeError Unsupported operand type bool for bitwise xor
false ^ array ( ) - TypeError Unsupported operand type bool for bitwise xor
false ^ array ( 0 => 1 ) - TypeError Unsupported operand type bool for bitwise xor
false ^ array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for bitwise xor
false ^ array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for bitwise xor
false ^ array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for bitwise xor
false ^ (object) array ( ) - TypeError Unsupported operand type bool for bitwise xor
false ^ (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for bitwise xor
false ^ (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for bitwise xor
false ^ DateTime - TypeError Unsupported operand type bool for bitwise xor
false ^ resource - TypeError Unsupported operand type bool for bitwise xor
false ^ NULL - TypeError Unsupported operand type bool for bitwise xor
true ^ false - TypeError Unsupported operand type bool for bitwise xor
true ^ true - TypeError Unsupported operand type bool for bitwise xor
true ^ 0 - TypeError Unsupported operand type bool for bitwise xor
true ^ 10 - TypeError Unsupported operand type bool for bitwise xor
true ^ 0.0 - TypeError Unsupported operand type bool for bitwise xor
true ^ 10.0 - TypeError Unsupported operand type bool for bitwise xor
true ^ 3.14 - TypeError Unsupported operand type bool for bitwise xor
true ^ '0' - TypeError Unsupported operand type bool for bitwise xor
true ^ '10' - TypeError Unsupported operand type bool for bitwise xor
true ^ '010' - TypeError Unsupported operand type bool for bitwise xor
true ^ '10 elephants' - TypeError Unsupported operand type bool for bitwise xor
true ^ 'foo' - TypeError Unsupported operand type bool for bitwise xor
true ^ array ( ) - TypeError Unsupported operand type bool for bitwise xor
true ^ array ( 0 => 1 ) - TypeError Unsupported operand type bool for bitwise xor
true ^ array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for bitwise xor
true ^ array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for bitwise xor
true ^ array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for bitwise xor
true ^ (object) array ( ) - TypeError Unsupported operand type bool for bitwise xor
true ^ (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for bitwise xor
true ^ (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for bitwise xor
true ^ DateTime - TypeError Unsupported operand type bool for bitwise xor
true ^ resource - TypeError Unsupported operand type bool for bitwise xor
true ^ NULL - TypeError Unsupported operand type bool for bitwise xor
0 ^ false - TypeError Unsupported operand type bool for bitwise xor
0 ^ true - TypeError Unsupported operand type bool for bitwise xor
0 ^ 0 = 0
0 ^ 10 = 10
0 ^ 0.0 - TypeError Unsupported operand type float for bitwise xor
0 ^ 10.0 - TypeError Unsupported operand type float for bitwise xor
0 ^ 3.14 - TypeError Unsupported operand type float for bitwise xor
0 ^ '0' - TypeError Operand type mismatch int and string for bitwise xor
0 ^ '10' - TypeError Operand type mismatch int and string for bitwise xor
0 ^ '010' - TypeError Operand type mismatch int and string for bitwise xor
0 ^ '10 elephants' - TypeError Operand type mismatch int and string for bitwise xor
0 ^ 'foo' - TypeError Operand type mismatch int and string for bitwise xor
0 ^ array ( ) - TypeError Unsupported operand type array for bitwise xor
0 ^ array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise xor
0 ^ array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise xor
0 ^ array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise xor
0 ^ array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise xor
0 ^ (object) array ( ) - TypeError Unsupported operand type object for bitwise xor
0 ^ (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise xor
0 ^ (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise xor
0 ^ DateTime - TypeError Unsupported operand type object for bitwise xor
0 ^ resource - TypeError Unsupported operand type resource for bitwise xor
0 ^ NULL - TypeError Unsupported operand type null for bitwise xor
10 ^ false - TypeError Unsupported operand type bool for bitwise xor
10 ^ true - TypeError Unsupported operand type bool for bitwise xor
10 ^ 0 = 10
10 ^ 10 = 0
10 ^ 0.0 - TypeError Unsupported operand type float for bitwise xor
10 ^ 10.0 - TypeError Unsupported operand type float for bitwise xor
10 ^ 3.14 - TypeError Unsupported operand type float for bitwise xor
10 ^ '0' - TypeError Operand type mismatch int and string for bitwise xor
10 ^ '10' - TypeError Operand type mismatch int and string for bitwise xor
10 ^ '010' - TypeError Operand type mismatch int and string for bitwise xor
10 ^ '10 elephants' - TypeError Operand type mismatch int and string for bitwise xor
10 ^ 'foo' - TypeError Operand type mismatch int and string for bitwise xor
10 ^ array ( ) - TypeError Unsupported operand type array for bitwise xor
10 ^ array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise xor
10 ^ array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise xor
10 ^ array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise xor
10 ^ array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise xor
10 ^ (object) array ( ) - TypeError Unsupported operand type object for bitwise xor
10 ^ (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise xor
10 ^ (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise xor
10 ^ DateTime - TypeError Unsupported operand type object for bitwise xor
10 ^ resource - TypeError Unsupported operand type resource for bitwise xor
10 ^ NULL - TypeError Unsupported operand type null for bitwise xor
0.0 ^ false - TypeError Unsupported operand type float for bitwise xor
0.0 ^ true - TypeError Unsupported operand type float for bitwise xor
0.0 ^ 0 - TypeError Unsupported operand type float for bitwise xor
0.0 ^ 10 - TypeError Unsupported operand type float for bitwise xor
0.0 ^ 0.0 - TypeError Unsupported operand type float for bitwise xor
0.0 ^ 10.0 - TypeError Unsupported operand type float for bitwise xor
0.0 ^ 3.14 - TypeError Unsupported operand type float for bitwise xor
0.0 ^ '0' - TypeError Unsupported operand type float for bitwise xor
0.0 ^ '10' - TypeError Unsupported operand type float for bitwise xor
0.0 ^ '010' - TypeError Unsupported operand type float for bitwise xor
0.0 ^ '10 elephants' - TypeError Unsupported operand type float for bitwise xor
0.0 ^ 'foo' - TypeError Unsupported operand type float for bitwise xor
0.0 ^ array ( ) - TypeError Unsupported operand type float for bitwise xor
0.0 ^ array ( 0 => 1 ) - TypeError Unsupported operand type float for bitwise xor
0.0 ^ array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type float for bitwise xor
0.0 ^ array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bitwise xor
0.0 ^ array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bitwise xor
0.0 ^ (object) array ( ) - TypeError Unsupported operand type float for bitwise xor
0.0 ^ (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bitwise xor
0.0 ^ (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bitwise xor
0.0 ^ DateTime - TypeError Unsupported operand type float for bitwise xor
0.0 ^ resource - TypeError Unsupported operand type float for bitwise xor
0.0 ^ NULL - TypeError Unsupported operand type float for bitwise xor
10.0 ^ false - TypeError Unsupported operand type float for bitwise xor
10.0 ^ true - TypeError Unsupported operand type float for bitwise xor
10.0 ^ 0 - TypeError Unsupported operand type float for bitwise xor
10.0 ^ 10 - TypeError Unsupported operand type float for bitwise xor
10.0 ^ 0.0 - TypeError Unsupported operand type float for bitwise xor
10.0 ^ 10.0 - TypeError Unsupported operand type float for bitwise xor
10.0 ^ 3.14 - TypeError Unsupported operand type float for bitwise xor
10.0 ^ '0' - TypeError Unsupported operand type float for bitwise xor
10.0 ^ '10' - TypeError Unsupported operand type float for bitwise xor
10.0 ^ '010' - TypeError Unsupported operand type float for bitwise xor
10.0 ^ '10 elephants' - TypeError Unsupported operand type float for bitwise xor
10.0 ^ 'foo' - TypeError Unsupported operand type float for bitwise xor
10.0 ^ array ( ) - TypeError Unsupported operand type float for bitwise xor
10.0 ^ array ( 0 => 1 ) - TypeError Unsupported operand type float for bitwise xor
10.0 ^ array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type float for bitwise xor
10.0 ^ array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bitwise xor
10.0 ^ array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bitwise xor
10.0 ^ (object) array ( ) - TypeError Unsupported operand type float for bitwise xor
10.0 ^ (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bitwise xor
10.0 ^ (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bitwise xor
10.0 ^ DateTime - TypeError Unsupported operand type float for bitwise xor
10.0 ^ resource - TypeError Unsupported operand type float for bitwise xor
10.0 ^ NULL - TypeError Unsupported operand type float for bitwise xor
3.14 ^ false - TypeError Unsupported operand type float for bitwise xor
3.14 ^ true - TypeError Unsupported operand type float for bitwise xor
3.14 ^ 0 - TypeError Unsupported operand type float for bitwise xor
3.14 ^ 10 - TypeError Unsupported operand type float for bitwise xor
3.14 ^ 0.0 - TypeError Unsupported operand type float for bitwise xor
3.14 ^ 10.0 - TypeError Unsupported operand type float for bitwise xor
3.14 ^ 3.14 - TypeError Unsupported operand type float for bitwise xor
3.14 ^ '0' - TypeError Unsupported operand type float for bitwise xor
3.14 ^ '10' - TypeError Unsupported operand type float for bitwise xor
3.14 ^ '010' - TypeError Unsupported operand type float for bitwise xor
3.14 ^ '10 elephants' - TypeError Unsupported operand type float for bitwise xor
3.14 ^ 'foo' - TypeError Unsupported operand type float for bitwise xor
3.14 ^ array ( ) - TypeError Unsupported operand type float for bitwise xor
3.14 ^ array ( 0 => 1 ) - TypeError Unsupported operand type float for bitwise xor
3.14 ^ array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type float for bitwise xor
3.14 ^ array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bitwise xor
3.14 ^ array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bitwise xor
3.14 ^ (object) array ( ) - TypeError Unsupported operand type float for bitwise xor
3.14 ^ (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type float for bitwise xor
3.14 ^ (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type float for bitwise xor
3.14 ^ DateTime - TypeError Unsupported operand type float for bitwise xor
3.14 ^ resource - TypeError Unsupported operand type float for bitwise xor
3.14 ^ NULL - TypeError Unsupported operand type float for bitwise xor
'0' ^ false - TypeError Unsupported operand type bool for bitwise xor
'0' ^ true - TypeError Unsupported operand type bool for bitwise xor
'0' ^ 0 - TypeError Operand type mismatch string and int for bitwise xor
'0' ^ 10 - TypeError Operand type mismatch string and int for bitwise xor
'0' ^ 0.0 - TypeError Unsupported operand type float for bitwise xor
'0' ^ 10.0 - TypeError Unsupported operand type float for bitwise xor
'0' ^ 3.14 - TypeError Unsupported operand type float for bitwise xor
'0' ^ '0' = base64:AA==
'0' ^ '10' = base64:AQ==
'0' ^ '010' = base64:AA==
'0' ^ '10 elephants' = base64:AQ==
'0' ^ 'foo' = base64:Vg==
'0' ^ array ( ) - TypeError Unsupported operand type array for bitwise xor
'0' ^ array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise xor
'0' ^ array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise xor
'0' ^ array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise xor
'0' ^ array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise xor
'0' ^ (object) array ( ) - TypeError Unsupported operand type object for bitwise xor
'0' ^ (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise xor
'0' ^ (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise xor
'0' ^ DateTime - TypeError Unsupported operand type object for bitwise xor
'0' ^ resource - TypeError Unsupported operand type resource for bitwise xor
'0' ^ NULL - TypeError Unsupported operand type null for bitwise xor
'10' ^ false - TypeError Unsupported operand type bool for bitwise xor
'10' ^ true - TypeError Unsupported operand type bool for bitwise xor
'10' ^ 0 - TypeError Operand type mismatch string and int for bitwise xor
'10' ^ 10 - TypeError Operand type mismatch string and int for bitwise xor
'10' ^ 0.0 - TypeError Unsupported operand type float for bitwise xor
'10' ^ 10.0 - TypeError Unsupported operand type float for bitwise xor
'10' ^ 3.14 - TypeError Unsupported operand type float for bitwise xor
'10' ^ '0' = base64:AQ==
'10' ^ '10' = base64:AAA=
'10' ^ '010' = base64:AQE=
'10' ^ '10 elephants' = base64:AAA=
'10' ^ 'foo' = base64:V18=
'10' ^ array ( ) - TypeError Unsupported operand type array for bitwise xor
'10' ^ array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise xor
'10' ^ array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise xor
'10' ^ array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise xor
'10' ^ array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise xor
'10' ^ (object) array ( ) - TypeError Unsupported operand type object for bitwise xor
'10' ^ (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise xor
'10' ^ (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise xor
'10' ^ DateTime - TypeError Unsupported operand type object for bitwise xor
'10' ^ resource - TypeError Unsupported operand type resource for bitwise xor
'10' ^ NULL - TypeError Unsupported operand type null for bitwise xor
'010' ^ false - TypeError Unsupported operand type bool for bitwise xor
'010' ^ true - TypeError Unsupported operand type bool for bitwise xor
'010' ^ 0 - TypeError Operand type mismatch string and int for bitwise xor
'010' ^ 10 - TypeError Operand type mismatch string and int for bitwise xor
'010' ^ 0.0 - TypeError Unsupported operand type float for bitwise xor
'010' ^ 10.0 - TypeError Unsupported operand type float for bitwise xor
'010' ^ 3.14 - TypeError Unsupported operand type float for bitwise xor
'010' ^ '0' = base64:AA==
'010' ^ '10' = base64:AQE=
'010' ^ '010' = base64:AAAA
'010' ^ '10 elephants' = base64:AQEQ
'010' ^ 'foo' = base64:Vl5f
'010' ^ array ( ) - TypeError Unsupported operand type array for bitwise xor
'010' ^ array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise xor
'010' ^ array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise xor
'010' ^ array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise xor
'010' ^ array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise xor
'010' ^ (object) array ( ) - TypeError Unsupported operand type object for bitwise xor
'010' ^ (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise xor
'010' ^ (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise xor
'010' ^ DateTime - TypeError Unsupported operand type object for bitwise xor
'010' ^ resource - TypeError Unsupported operand type resource for bitwise xor
'010' ^ NULL - TypeError Unsupported operand type null for bitwise xor
'10 elephants' ^ false - TypeError Unsupported operand type bool for bitwise xor
'10 elephants' ^ true - TypeError Unsupported operand type bool for bitwise xor
'10 elephants' ^ 0 - TypeError Operand type mismatch string and int for bitwise xor
'10 elephants' ^ 10 - TypeError Operand type mismatch string and int for bitwise xor
'10 elephants' ^ 0.0 - TypeError Unsupported operand type float for bitwise xor
'10 elephants' ^ 10.0 - TypeError Unsupported operand type float for bitwise xor
'10 elephants' ^ 3.14 - TypeError Unsupported operand type float for bitwise xor
'10 elephants' ^ '0' = base64:AQ==
'10 elephants' ^ '10' = base64:AAA=
'10 elephants' ^ '010' = base64:AQEQ
'10 elephants' ^ '10 elephants' = base64:AAAAAAAAAAAAAAAA
'10 elephants' ^ 'foo' = base64:V19P
'10 elephants' ^ array ( ) - TypeError Unsupported operand type array for bitwise xor
'10 elephants' ^ array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise xor
'10 elephants' ^ array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise xor
'10 elephants' ^ array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise xor
'10 elephants' ^ array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise xor
'10 elephants' ^ (object) array ( ) - TypeError Unsupported operand type object for bitwise xor
'10 elephants' ^ (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise xor
'10 elephants' ^ (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise xor
'10 elephants' ^ DateTime - TypeError Unsupported operand type object for bitwise xor
'10 elephants' ^ resource - TypeError Unsupported operand type resource for bitwise xor
'10 elephants' ^ NULL - TypeError Unsupported operand type null for bitwise xor
'foo' ^ false - TypeError Unsupported operand type bool for bitwise xor
'foo' ^ true - TypeError Unsupported operand type bool for bitwise xor
'foo' ^ 0 - TypeError Operand type mismatch string and int for bitwise xor
'foo' ^ 10 - TypeError Operand type mismatch string and int for bitwise xor
'foo' ^ 0.0 - TypeError Unsupported operand type float for bitwise xor
'foo' ^ 10.0 - TypeError Unsupported operand type float for bitwise xor
'foo' ^ 3.14 - TypeError Unsupported operand type float for bitwise xor
'foo' ^ '0' = base64:Vg==
'foo' ^ '10' = base64:V18=
'foo' ^ '010' = base64:Vl5f
'foo' ^ '10 elephants' = base64:V19P
'foo' ^ 'foo' = base64:AAAA
'foo' ^ array ( ) - TypeError Unsupported operand type array for bitwise xor
'foo' ^ array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise xor
'foo' ^ array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise xor
'foo' ^ array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise xor
'foo' ^ array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise xor
'foo' ^ (object) array ( ) - TypeError Unsupported operand type object for bitwise xor
'foo' ^ (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise xor
'foo' ^ (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise xor
'foo' ^ DateTime - TypeError Unsupported operand type object for bitwise xor
'foo' ^ resource - TypeError Unsupported operand type resource for bitwise xor
'foo' ^ NULL - TypeError Unsupported operand type null for bitwise xor
array ( ) ^ false - TypeError Unsupported operand type array for bitwise xor
array ( ) ^ true - TypeError Unsupported operand type array for bitwise xor
array ( ) ^ 0 - TypeError Unsupported operand type array for bitwise xor
array ( ) ^ 10 - TypeError Unsupported operand type array for bitwise xor
array ( ) ^ 0.0 - TypeError Unsupported operand type array for bitwise xor
array ( ) ^ 10.0 - TypeError Unsupported operand type array for bitwise xor
array ( ) ^ 3.14 - TypeError Unsupported operand type array for bitwise xor
array ( ) ^ '0' - TypeError Unsupported operand type array for bitwise xor
array ( ) ^ '10' - TypeError Unsupported operand type array for bitwise xor
array ( ) ^ '010' - TypeError Unsupported operand type array for bitwise xor
array ( ) ^ '10 elephants' - TypeError Unsupported operand type array for bitwise xor
array ( ) ^ 'foo' - TypeError Unsupported operand type array for bitwise xor
array ( ) ^ array ( ) - TypeError Unsupported operand type array for bitwise xor
array ( ) ^ array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise xor
array ( ) ^ array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise xor
array ( ) ^ array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise xor
array ( ) ^ array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise xor
array ( ) ^ (object) array ( ) - TypeError Unsupported operand type array for bitwise xor
array ( ) ^ (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise xor
array ( ) ^ (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise xor
array ( ) ^ DateTime - TypeError Unsupported operand type array for bitwise xor
array ( ) ^ resource - TypeError Unsupported operand type array for bitwise xor
array ( ) ^ NULL - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1 ) ^ false - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1 ) ^ true - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1 ) ^ 0 - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1 ) ^ 10 - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1 ) ^ 0.0 - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1 ) ^ 10.0 - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1 ) ^ 3.14 - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1 ) ^ '0' - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1 ) ^ '10' - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1 ) ^ '010' - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1 ) ^ '10 elephants' - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1 ) ^ 'foo' - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1 ) ^ array ( ) - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1 ) ^ array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1 ) ^ array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1 ) ^ array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1 ) ^ array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1 ) ^ (object) array ( ) - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1 ) ^ (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1 ) ^ (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1 ) ^ DateTime - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1 ) ^ resource - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1 ) ^ NULL - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1, 1 => 100 ) ^ false - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1, 1 => 100 ) ^ true - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1, 1 => 100 ) ^ 0 - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1, 1 => 100 ) ^ 10 - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1, 1 => 100 ) ^ 0.0 - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1, 1 => 100 ) ^ 10.0 - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1, 1 => 100 ) ^ 3.14 - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1, 1 => 100 ) ^ '0' - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1, 1 => 100 ) ^ '10' - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1, 1 => 100 ) ^ '010' - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1, 1 => 100 ) ^ '10 elephants' - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1, 1 => 100 ) ^ 'foo' - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1, 1 => 100 ) ^ array ( ) - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1, 1 => 100 ) ^ array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1, 1 => 100 ) ^ array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1, 1 => 100 ) ^ array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1, 1 => 100 ) ^ array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1, 1 => 100 ) ^ (object) array ( ) - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1, 1 => 100 ) ^ (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1, 1 => 100 ) ^ (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1, 1 => 100 ) ^ DateTime - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1, 1 => 100 ) ^ resource - TypeError Unsupported operand type array for bitwise xor
array ( 0 => 1, 1 => 100 ) ^ NULL - TypeError Unsupported operand type array for bitwise xor
array ( 'foo' => 1, 'bar' => 2 ) ^ false - TypeError Unsupported operand type array for bitwise xor
array ( 'foo' => 1, 'bar' => 2 ) ^ true - TypeError Unsupported operand type array for bitwise xor
array ( 'foo' => 1, 'bar' => 2 ) ^ 0 - TypeError Unsupported operand type array for bitwise xor
array ( 'foo' => 1, 'bar' => 2 ) ^ 10 - TypeError Unsupported operand type array for bitwise xor
array ( 'foo' => 1, 'bar' => 2 ) ^ 0.0 - TypeError Unsupported operand type array for bitwise xor
array ( 'foo' => 1, 'bar' => 2 ) ^ 10.0 - TypeError Unsupported operand type array for bitwise xor
array ( 'foo' => 1, 'bar' => 2 ) ^ 3.14 - TypeError Unsupported operand type array for bitwise xor
array ( 'foo' => 1, 'bar' => 2 ) ^ '0' - TypeError Unsupported operand type array for bitwise xor
array ( 'foo' => 1, 'bar' => 2 ) ^ '10' - TypeError Unsupported operand type array for bitwise xor
array ( 'foo' => 1, 'bar' => 2 ) ^ '010' - TypeError Unsupported operand type array for bitwise xor
array ( 'foo' => 1, 'bar' => 2 ) ^ '10 elephants' - TypeError Unsupported operand type array for bitwise xor
array ( 'foo' => 1, 'bar' => 2 ) ^ 'foo' - TypeError Unsupported operand type array for bitwise xor
array ( 'foo' => 1, 'bar' => 2 ) ^ array ( ) - TypeError Unsupported operand type array for bitwise xor
array ( 'foo' => 1, 'bar' => 2 ) ^ array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise xor
array ( 'foo' => 1, 'bar' => 2 ) ^ array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise xor
array ( 'foo' => 1, 'bar' => 2 ) ^ array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise xor
array ( 'foo' => 1, 'bar' => 2 ) ^ array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise xor
array ( 'foo' => 1, 'bar' => 2 ) ^ (object) array ( ) - TypeError Unsupported operand type array for bitwise xor
array ( 'foo' => 1, 'bar' => 2 ) ^ (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise xor
array ( 'foo' => 1, 'bar' => 2 ) ^ (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise xor
array ( 'foo' => 1, 'bar' => 2 ) ^ DateTime - TypeError Unsupported operand type array for bitwise xor
array ( 'foo' => 1, 'bar' => 2 ) ^ resource - TypeError Unsupported operand type array for bitwise xor
array ( 'foo' => 1, 'bar' => 2 ) ^ NULL - TypeError Unsupported operand type array for bitwise xor
array ( 'bar' => 1, 'foo' => 2 ) ^ false - TypeError Unsupported operand type array for bitwise xor
array ( 'bar' => 1, 'foo' => 2 ) ^ true - TypeError Unsupported operand type array for bitwise xor
array ( 'bar' => 1, 'foo' => 2 ) ^ 0 - TypeError Unsupported operand type array for bitwise xor
array ( 'bar' => 1, 'foo' => 2 ) ^ 10 - TypeError Unsupported operand type array for bitwise xor
array ( 'bar' => 1, 'foo' => 2 ) ^ 0.0 - TypeError Unsupported operand type array for bitwise xor
array ( 'bar' => 1, 'foo' => 2 ) ^ 10.0 - TypeError Unsupported operand type array for bitwise xor
array ( 'bar' => 1, 'foo' => 2 ) ^ 3.14 - TypeError Unsupported operand type array for bitwise xor
array ( 'bar' => 1, 'foo' => 2 ) ^ '0' - TypeError Unsupported operand type array for bitwise xor
array ( 'bar' => 1, 'foo' => 2 ) ^ '10' - TypeError Unsupported operand type array for bitwise xor
array ( 'bar' => 1, 'foo' => 2 ) ^ '010' - TypeError Unsupported operand type array for bitwise xor
array ( 'bar' => 1, 'foo' => 2 ) ^ '10 elephants' - TypeError Unsupported operand type array for bitwise xor
array ( 'bar' => 1, 'foo' => 2 ) ^ 'foo' - TypeError Unsupported operand type array for bitwise xor
array ( 'bar' => 1, 'foo' => 2 ) ^ array ( ) - TypeError Unsupported operand type array for bitwise xor
array ( 'bar' => 1, 'foo' => 2 ) ^ array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise xor
array ( 'bar' => 1, 'foo' => 2 ) ^ array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise xor
array ( 'bar' => 1, 'foo' => 2 ) ^ array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise xor
array ( 'bar' => 1, 'foo' => 2 ) ^ array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise xor
array ( 'bar' => 1, 'foo' => 2 ) ^ (object) array ( ) - TypeError Unsupported operand type array for bitwise xor
array ( 'bar' => 1, 'foo' => 2 ) ^ (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise xor
array ( 'bar' => 1, 'foo' => 2 ) ^ (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise xor
array ( 'bar' => 1, 'foo' => 2 ) ^ DateTime - TypeError Unsupported operand type array for bitwise xor
array ( 'bar' => 1, 'foo' => 2 ) ^ resource - TypeError Unsupported operand type array for bitwise xor
array ( 'bar' => 1, 'foo' => 2 ) ^ NULL - TypeError Unsupported operand type array for bitwise xor
(object) array ( ) ^ false - TypeError Unsupported operand type object for bitwise xor
(object) array ( ) ^ true - TypeError Unsupported operand type object for bitwise xor
(object) array ( ) ^ 0 - TypeError Unsupported operand type object for bitwise xor
(object) array ( ) ^ 10 - TypeError Unsupported operand type object for bitwise xor
(object) array ( ) ^ 0.0 - TypeError Unsupported operand type object for bitwise xor
(object) array ( ) ^ 10.0 - TypeError Unsupported operand type object for bitwise xor
(object) array ( ) ^ 3.14 - TypeError Unsupported operand type object for bitwise xor
(object) array ( ) ^ '0' - TypeError Unsupported operand type object for bitwise xor
(object) array ( ) ^ '10' - TypeError Unsupported operand type object for bitwise xor
(object) array ( ) ^ '010' - TypeError Unsupported operand type object for bitwise xor
(object) array ( ) ^ '10 elephants' - TypeError Unsupported operand type object for bitwise xor
(object) array ( ) ^ 'foo' - TypeError Unsupported operand type object for bitwise xor
(object) array ( ) ^ array ( ) - TypeError Unsupported operand type object for bitwise xor
(object) array ( ) ^ array ( 0 => 1 ) - TypeError Unsupported operand type object for bitwise xor
(object) array ( ) ^ array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for bitwise xor
(object) array ( ) ^ array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise xor
(object) array ( ) ^ array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise xor
(object) array ( ) ^ (object) array ( ) - TypeError Unsupported operand type object for bitwise xor
(object) array ( ) ^ (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise xor
(object) array ( ) ^ (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise xor
(object) array ( ) ^ DateTime - TypeError Unsupported operand type object for bitwise xor
(object) array ( ) ^ resource - TypeError Unsupported operand type object for bitwise xor
(object) array ( ) ^ NULL - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'foo' => 1, 'bar' => 2 ) ^ false - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'foo' => 1, 'bar' => 2 ) ^ true - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'foo' => 1, 'bar' => 2 ) ^ 0 - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'foo' => 1, 'bar' => 2 ) ^ 10 - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'foo' => 1, 'bar' => 2 ) ^ 0.0 - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'foo' => 1, 'bar' => 2 ) ^ 10.0 - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'foo' => 1, 'bar' => 2 ) ^ 3.14 - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'foo' => 1, 'bar' => 2 ) ^ '0' - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'foo' => 1, 'bar' => 2 ) ^ '10' - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'foo' => 1, 'bar' => 2 ) ^ '010' - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'foo' => 1, 'bar' => 2 ) ^ '10 elephants' - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'foo' => 1, 'bar' => 2 ) ^ 'foo' - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'foo' => 1, 'bar' => 2 ) ^ array ( ) - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'foo' => 1, 'bar' => 2 ) ^ array ( 0 => 1 ) - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'foo' => 1, 'bar' => 2 ) ^ array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'foo' => 1, 'bar' => 2 ) ^ array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'foo' => 1, 'bar' => 2 ) ^ array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'foo' => 1, 'bar' => 2 ) ^ (object) array ( ) - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'foo' => 1, 'bar' => 2 ) ^ (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'foo' => 1, 'bar' => 2 ) ^ (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'foo' => 1, 'bar' => 2 ) ^ DateTime - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'foo' => 1, 'bar' => 2 ) ^ resource - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'foo' => 1, 'bar' => 2 ) ^ NULL - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'bar' => 1, 'foo' => 2 ) ^ false - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'bar' => 1, 'foo' => 2 ) ^ true - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'bar' => 1, 'foo' => 2 ) ^ 0 - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'bar' => 1, 'foo' => 2 ) ^ 10 - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'bar' => 1, 'foo' => 2 ) ^ 0.0 - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'bar' => 1, 'foo' => 2 ) ^ 10.0 - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'bar' => 1, 'foo' => 2 ) ^ 3.14 - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'bar' => 1, 'foo' => 2 ) ^ '0' - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'bar' => 1, 'foo' => 2 ) ^ '10' - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'bar' => 1, 'foo' => 2 ) ^ '010' - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'bar' => 1, 'foo' => 2 ) ^ '10 elephants' - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'bar' => 1, 'foo' => 2 ) ^ 'foo' - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'bar' => 1, 'foo' => 2 ) ^ array ( ) - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'bar' => 1, 'foo' => 2 ) ^ array ( 0 => 1 ) - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'bar' => 1, 'foo' => 2 ) ^ array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'bar' => 1, 'foo' => 2 ) ^ array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'bar' => 1, 'foo' => 2 ) ^ array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'bar' => 1, 'foo' => 2 ) ^ (object) array ( ) - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'bar' => 1, 'foo' => 2 ) ^ (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'bar' => 1, 'foo' => 2 ) ^ (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'bar' => 1, 'foo' => 2 ) ^ DateTime - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'bar' => 1, 'foo' => 2 ) ^ resource - TypeError Unsupported operand type object for bitwise xor
(object) array ( 'bar' => 1, 'foo' => 2 ) ^ NULL - TypeError Unsupported operand type object for bitwise xor
DateTime ^ false - TypeError Unsupported operand type object for bitwise xor
DateTime ^ true - TypeError Unsupported operand type object for bitwise xor
DateTime ^ 0 - TypeError Unsupported operand type object for bitwise xor
DateTime ^ 10 - TypeError Unsupported operand type object for bitwise xor
DateTime ^ 0.0 - TypeError Unsupported operand type object for bitwise xor
DateTime ^ 10.0 - TypeError Unsupported operand type object for bitwise xor
DateTime ^ 3.14 - TypeError Unsupported operand type object for bitwise xor
DateTime ^ '0' - TypeError Unsupported operand type object for bitwise xor
DateTime ^ '10' - TypeError Unsupported operand type object for bitwise xor
DateTime ^ '010' - TypeError Unsupported operand type object for bitwise xor
DateTime ^ '10 elephants' - TypeError Unsupported operand type object for bitwise xor
DateTime ^ 'foo' - TypeError Unsupported operand type object for bitwise xor
DateTime ^ array ( ) - TypeError Unsupported operand type object for bitwise xor
DateTime ^ array ( 0 => 1 ) - TypeError Unsupported operand type object for bitwise xor
DateTime ^ array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type object for bitwise xor
DateTime ^ array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise xor
DateTime ^ array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise xor
DateTime ^ (object) array ( ) - TypeError Unsupported operand type object for bitwise xor
DateTime ^ (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise xor
DateTime ^ (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise xor
DateTime ^ DateTime - TypeError Unsupported operand type object for bitwise xor
DateTime ^ resource - TypeError Unsupported operand type object for bitwise xor
DateTime ^ NULL - TypeError Unsupported operand type object for bitwise xor
resource ^ false - TypeError Unsupported operand type resource for bitwise xor
resource ^ true - TypeError Unsupported operand type resource for bitwise xor
resource ^ 0 - TypeError Unsupported operand type resource for bitwise xor
resource ^ 10 - TypeError Unsupported operand type resource for bitwise xor
resource ^ 0.0 - TypeError Unsupported operand type resource for bitwise xor
resource ^ 10.0 - TypeError Unsupported operand type resource for bitwise xor
resource ^ 3.14 - TypeError Unsupported operand type resource for bitwise xor
resource ^ '0' - TypeError Unsupported operand type resource for bitwise xor
resource ^ '10' - TypeError Unsupported operand type resource for bitwise xor
resource ^ '010' - TypeError Unsupported operand type resource for bitwise xor
resource ^ '10 elephants' - TypeError Unsupported operand type resource for bitwise xor
resource ^ 'foo' - TypeError Unsupported operand type resource for bitwise xor
resource ^ array ( ) - TypeError Unsupported operand type resource for bitwise xor
resource ^ array ( 0 => 1 ) - TypeError Unsupported operand type resource for bitwise xor
resource ^ array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type resource for bitwise xor
resource ^ array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for bitwise xor
resource ^ array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for bitwise xor
resource ^ (object) array ( ) - TypeError Unsupported operand type resource for bitwise xor
resource ^ (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for bitwise xor
resource ^ (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for bitwise xor
resource ^ DateTime - TypeError Unsupported operand type resource for bitwise xor
resource ^ resource - TypeError Unsupported operand type resource for bitwise xor
resource ^ NULL - TypeError Unsupported operand type resource for bitwise xor
NULL ^ false - TypeError Unsupported operand type null for bitwise xor
NULL ^ true - TypeError Unsupported operand type null for bitwise xor
NULL ^ 0 - TypeError Unsupported operand type null for bitwise xor
NULL ^ 10 - TypeError Unsupported operand type null for bitwise xor
NULL ^ 0.0 - TypeError Unsupported operand type null for bitwise xor
NULL ^ 10.0 - TypeError Unsupported operand type null for bitwise xor
NULL ^ 3.14 - TypeError Unsupported operand type null for bitwise xor
NULL ^ '0' - TypeError Unsupported operand type null for bitwise xor
NULL ^ '10' - TypeError Unsupported operand type null for bitwise xor
NULL ^ '010' - TypeError Unsupported operand type null for bitwise xor
NULL ^ '10 elephants' - TypeError Unsupported operand type null for bitwise xor
NULL ^ 'foo' - TypeError Unsupported operand type null for bitwise xor
NULL ^ array ( ) - TypeError Unsupported operand type null for bitwise xor
NULL ^ array ( 0 => 1 ) - TypeError Unsupported operand type null for bitwise xor
NULL ^ array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type null for bitwise xor
NULL ^ array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for bitwise xor
NULL ^ array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for bitwise xor
NULL ^ (object) array ( ) - TypeError Unsupported operand type null for bitwise xor
NULL ^ (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for bitwise xor
NULL ^ (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for bitwise xor
NULL ^ DateTime - TypeError Unsupported operand type null for bitwise xor
NULL ^ resource - TypeError Unsupported operand type null for bitwise xor
NULL ^ NULL - TypeError Unsupported operand type null for bitwise xor