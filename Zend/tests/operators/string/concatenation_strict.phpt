--TEST--
concatenation operator with strict_operators
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a . $b', function($a, $b) { return $a . $b; });

--EXPECT--
false . false - TypeError Unsupported operand type bool for concatenation
false . true - TypeError Unsupported operand type bool for concatenation
false . 0 - TypeError Unsupported operand type bool for concatenation
false . 10 - TypeError Unsupported operand type bool for concatenation
false . 0.0 - TypeError Unsupported operand type bool for concatenation
false . 10.0 - TypeError Unsupported operand type bool for concatenation
false . 3.14 - TypeError Unsupported operand type bool for concatenation
false . '0' - TypeError Unsupported operand type bool for concatenation
false . '10' - TypeError Unsupported operand type bool for concatenation
false . '010' - TypeError Unsupported operand type bool for concatenation
false . '10 elephants' - TypeError Unsupported operand type bool for concatenation
false . 'foo' - TypeError Unsupported operand type bool for concatenation
false . array ( ) - TypeError Unsupported operand type bool for concatenation
false . array ( 0 => 1 ) - TypeError Unsupported operand type bool for concatenation
false . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for concatenation
false . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for concatenation
false . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for concatenation
false . (object) array ( ) - TypeError Unsupported operand type bool for concatenation
false . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for concatenation
false . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for concatenation
false . DateTime - TypeError Unsupported operand type bool for concatenation
false . resource - TypeError Unsupported operand type bool for concatenation
false . NULL - TypeError Unsupported operand type bool for concatenation
true . false - TypeError Unsupported operand type bool for concatenation
true . true - TypeError Unsupported operand type bool for concatenation
true . 0 - TypeError Unsupported operand type bool for concatenation
true . 10 - TypeError Unsupported operand type bool for concatenation
true . 0.0 - TypeError Unsupported operand type bool for concatenation
true . 10.0 - TypeError Unsupported operand type bool for concatenation
true . 3.14 - TypeError Unsupported operand type bool for concatenation
true . '0' - TypeError Unsupported operand type bool for concatenation
true . '10' - TypeError Unsupported operand type bool for concatenation
true . '010' - TypeError Unsupported operand type bool for concatenation
true . '10 elephants' - TypeError Unsupported operand type bool for concatenation
true . 'foo' - TypeError Unsupported operand type bool for concatenation
true . array ( ) - TypeError Unsupported operand type bool for concatenation
true . array ( 0 => 1 ) - TypeError Unsupported operand type bool for concatenation
true . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type bool for concatenation
true . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for concatenation
true . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for concatenation
true . (object) array ( ) - TypeError Unsupported operand type bool for concatenation
true . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type bool for concatenation
true . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type bool for concatenation
true . DateTime - TypeError Unsupported operand type bool for concatenation
true . resource - TypeError Unsupported operand type bool for concatenation
true . NULL - TypeError Unsupported operand type bool for concatenation
0 . false - TypeError Unsupported operand type bool for concatenation
0 . true - TypeError Unsupported operand type bool for concatenation
0 . 0 = '00'
0 . 10 = '010'
0 . 0.0 = '00'
0 . 10.0 = '010'
0 . 3.14 = '03.14'
0 . '0' = '00'
0 . '10' = '010'
0 . '010' = '0010'
0 . '10 elephants' = '010 elephants'
0 . 'foo' = '0foo'
0 . array ( ) - TypeError Unsupported operand type array for concatenation
0 . array ( 0 => 1 ) - TypeError Unsupported operand type array for concatenation
0 . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for concatenation
0 . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for concatenation
0 . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for concatenation
0 . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
0 . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
0 . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
0 . DateTime - TypeError Object of class DateTime could not be converted to string
0 . resource - TypeError Unsupported operand type resource for concatenation
0 . NULL - TypeError Unsupported operand type null for concatenation
10 . false - TypeError Unsupported operand type bool for concatenation
10 . true - TypeError Unsupported operand type bool for concatenation
10 . 0 = '100'
10 . 10 = '1010'
10 . 0.0 = '100'
10 . 10.0 = '1010'
10 . 3.14 = '103.14'
10 . '0' = '100'
10 . '10' = '1010'
10 . '010' = '10010'
10 . '10 elephants' = '1010 elephants'
10 . 'foo' = '10foo'
10 . array ( ) - TypeError Unsupported operand type array for concatenation
10 . array ( 0 => 1 ) - TypeError Unsupported operand type array for concatenation
10 . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for concatenation
10 . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for concatenation
10 . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for concatenation
10 . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
10 . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
10 . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
10 . DateTime - TypeError Object of class DateTime could not be converted to string
10 . resource - TypeError Unsupported operand type resource for concatenation
10 . NULL - TypeError Unsupported operand type null for concatenation
0.0 . false - TypeError Unsupported operand type bool for concatenation
0.0 . true - TypeError Unsupported operand type bool for concatenation
0.0 . 0 = '00'
0.0 . 10 = '010'
0.0 . 0.0 = '00'
0.0 . 10.0 = '010'
0.0 . 3.14 = '03.14'
0.0 . '0' = '00'
0.0 . '10' = '010'
0.0 . '010' = '0010'
0.0 . '10 elephants' = '010 elephants'
0.0 . 'foo' = '0foo'
0.0 . array ( ) - TypeError Unsupported operand type array for concatenation
0.0 . array ( 0 => 1 ) - TypeError Unsupported operand type array for concatenation
0.0 . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for concatenation
0.0 . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for concatenation
0.0 . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for concatenation
0.0 . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
0.0 . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
0.0 . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
0.0 . DateTime - TypeError Object of class DateTime could not be converted to string
0.0 . resource - TypeError Unsupported operand type resource for concatenation
0.0 . NULL - TypeError Unsupported operand type null for concatenation
10.0 . false - TypeError Unsupported operand type bool for concatenation
10.0 . true - TypeError Unsupported operand type bool for concatenation
10.0 . 0 = '100'
10.0 . 10 = '1010'
10.0 . 0.0 = '100'
10.0 . 10.0 = '1010'
10.0 . 3.14 = '103.14'
10.0 . '0' = '100'
10.0 . '10' = '1010'
10.0 . '010' = '10010'
10.0 . '10 elephants' = '1010 elephants'
10.0 . 'foo' = '10foo'
10.0 . array ( ) - TypeError Unsupported operand type array for concatenation
10.0 . array ( 0 => 1 ) - TypeError Unsupported operand type array for concatenation
10.0 . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for concatenation
10.0 . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for concatenation
10.0 . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for concatenation
10.0 . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
10.0 . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
10.0 . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
10.0 . DateTime - TypeError Object of class DateTime could not be converted to string
10.0 . resource - TypeError Unsupported operand type resource for concatenation
10.0 . NULL - TypeError Unsupported operand type null for concatenation
3.14 . false - TypeError Unsupported operand type bool for concatenation
3.14 . true - TypeError Unsupported operand type bool for concatenation
3.14 . 0 = '3.140'
3.14 . 10 = '3.1410'
3.14 . 0.0 = '3.140'
3.14 . 10.0 = '3.1410'
3.14 . 3.14 = '3.143.14'
3.14 . '0' = '3.140'
3.14 . '10' = '3.1410'
3.14 . '010' = '3.14010'
3.14 . '10 elephants' = '3.1410 elephants'
3.14 . 'foo' = '3.14foo'
3.14 . array ( ) - TypeError Unsupported operand type array for concatenation
3.14 . array ( 0 => 1 ) - TypeError Unsupported operand type array for concatenation
3.14 . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for concatenation
3.14 . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for concatenation
3.14 . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for concatenation
3.14 . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
3.14 . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
3.14 . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
3.14 . DateTime - TypeError Object of class DateTime could not be converted to string
3.14 . resource - TypeError Unsupported operand type resource for concatenation
3.14 . NULL - TypeError Unsupported operand type null for concatenation
'0' . false - TypeError Unsupported operand type bool for concatenation
'0' . true - TypeError Unsupported operand type bool for concatenation
'0' . 0 = '00'
'0' . 10 = '010'
'0' . 0.0 = '00'
'0' . 10.0 = '010'
'0' . 3.14 = '03.14'
'0' . '0' = '00'
'0' . '10' = '010'
'0' . '010' = '0010'
'0' . '10 elephants' = '010 elephants'
'0' . 'foo' = '0foo'
'0' . array ( ) - TypeError Unsupported operand type array for concatenation
'0' . array ( 0 => 1 ) - TypeError Unsupported operand type array for concatenation
'0' . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for concatenation
'0' . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for concatenation
'0' . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for concatenation
'0' . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
'0' . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
'0' . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
'0' . DateTime - TypeError Object of class DateTime could not be converted to string
'0' . resource - TypeError Unsupported operand type resource for concatenation
'0' . NULL - TypeError Unsupported operand type null for concatenation
'10' . false - TypeError Unsupported operand type bool for concatenation
'10' . true - TypeError Unsupported operand type bool for concatenation
'10' . 0 = '100'
'10' . 10 = '1010'
'10' . 0.0 = '100'
'10' . 10.0 = '1010'
'10' . 3.14 = '103.14'
'10' . '0' = '100'
'10' . '10' = '1010'
'10' . '010' = '10010'
'10' . '10 elephants' = '1010 elephants'
'10' . 'foo' = '10foo'
'10' . array ( ) - TypeError Unsupported operand type array for concatenation
'10' . array ( 0 => 1 ) - TypeError Unsupported operand type array for concatenation
'10' . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for concatenation
'10' . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for concatenation
'10' . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for concatenation
'10' . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
'10' . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
'10' . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
'10' . DateTime - TypeError Object of class DateTime could not be converted to string
'10' . resource - TypeError Unsupported operand type resource for concatenation
'10' . NULL - TypeError Unsupported operand type null for concatenation
'010' . false - TypeError Unsupported operand type bool for concatenation
'010' . true - TypeError Unsupported operand type bool for concatenation
'010' . 0 = '0100'
'010' . 10 = '01010'
'010' . 0.0 = '0100'
'010' . 10.0 = '01010'
'010' . 3.14 = '0103.14'
'010' . '0' = '0100'
'010' . '10' = '01010'
'010' . '010' = '010010'
'010' . '10 elephants' = '01010 elephants'
'010' . 'foo' = '010foo'
'010' . array ( ) - TypeError Unsupported operand type array for concatenation
'010' . array ( 0 => 1 ) - TypeError Unsupported operand type array for concatenation
'010' . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for concatenation
'010' . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for concatenation
'010' . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for concatenation
'010' . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
'010' . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
'010' . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
'010' . DateTime - TypeError Object of class DateTime could not be converted to string
'010' . resource - TypeError Unsupported operand type resource for concatenation
'010' . NULL - TypeError Unsupported operand type null for concatenation
'10 elephants' . false - TypeError Unsupported operand type bool for concatenation
'10 elephants' . true - TypeError Unsupported operand type bool for concatenation
'10 elephants' . 0 = '10 elephants0'
'10 elephants' . 10 = '10 elephants10'
'10 elephants' . 0.0 = '10 elephants0'
'10 elephants' . 10.0 = '10 elephants10'
'10 elephants' . 3.14 = '10 elephants3.14'
'10 elephants' . '0' = '10 elephants0'
'10 elephants' . '10' = '10 elephants10'
'10 elephants' . '010' = '10 elephants010'
'10 elephants' . '10 elephants' = '10 elephants10 elephants'
'10 elephants' . 'foo' = '10 elephantsfoo'
'10 elephants' . array ( ) - TypeError Unsupported operand type array for concatenation
'10 elephants' . array ( 0 => 1 ) - TypeError Unsupported operand type array for concatenation
'10 elephants' . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for concatenation
'10 elephants' . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for concatenation
'10 elephants' . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for concatenation
'10 elephants' . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
'10 elephants' . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
'10 elephants' . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
'10 elephants' . DateTime - TypeError Object of class DateTime could not be converted to string
'10 elephants' . resource - TypeError Unsupported operand type resource for concatenation
'10 elephants' . NULL - TypeError Unsupported operand type null for concatenation
'foo' . false - TypeError Unsupported operand type bool for concatenation
'foo' . true - TypeError Unsupported operand type bool for concatenation
'foo' . 0 = 'foo0'
'foo' . 10 = 'foo10'
'foo' . 0.0 = 'foo0'
'foo' . 10.0 = 'foo10'
'foo' . 3.14 = 'foo3.14'
'foo' . '0' = 'foo0'
'foo' . '10' = 'foo10'
'foo' . '010' = 'foo010'
'foo' . '10 elephants' = 'foo10 elephants'
'foo' . 'foo' = 'foofoo'
'foo' . array ( ) - TypeError Unsupported operand type array for concatenation
'foo' . array ( 0 => 1 ) - TypeError Unsupported operand type array for concatenation
'foo' . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for concatenation
'foo' . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for concatenation
'foo' . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for concatenation
'foo' . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
'foo' . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
'foo' . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
'foo' . DateTime - TypeError Object of class DateTime could not be converted to string
'foo' . resource - TypeError Unsupported operand type resource for concatenation
'foo' . NULL - TypeError Unsupported operand type null for concatenation
array ( ) . false - TypeError Unsupported operand type array for concatenation
array ( ) . true - TypeError Unsupported operand type array for concatenation
array ( ) . 0 - TypeError Unsupported operand type array for concatenation
array ( ) . 10 - TypeError Unsupported operand type array for concatenation
array ( ) . 0.0 - TypeError Unsupported operand type array for concatenation
array ( ) . 10.0 - TypeError Unsupported operand type array for concatenation
array ( ) . 3.14 - TypeError Unsupported operand type array for concatenation
array ( ) . '0' - TypeError Unsupported operand type array for concatenation
array ( ) . '10' - TypeError Unsupported operand type array for concatenation
array ( ) . '010' - TypeError Unsupported operand type array for concatenation
array ( ) . '10 elephants' - TypeError Unsupported operand type array for concatenation
array ( ) . 'foo' - TypeError Unsupported operand type array for concatenation
array ( ) . array ( ) - TypeError Unsupported operand type array for concatenation
array ( ) . array ( 0 => 1 ) - TypeError Unsupported operand type array for concatenation
array ( ) . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for concatenation
array ( ) . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for concatenation
array ( ) . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for concatenation
array ( ) . (object) array ( ) - TypeError Unsupported operand type array for concatenation
array ( ) . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for concatenation
array ( ) . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for concatenation
array ( ) . DateTime - TypeError Unsupported operand type array for concatenation
array ( ) . resource - TypeError Unsupported operand type array for concatenation
array ( ) . NULL - TypeError Unsupported operand type array for concatenation
array ( 0 => 1 ) . false - TypeError Unsupported operand type array for concatenation
array ( 0 => 1 ) . true - TypeError Unsupported operand type array for concatenation
array ( 0 => 1 ) . 0 - TypeError Unsupported operand type array for concatenation
array ( 0 => 1 ) . 10 - TypeError Unsupported operand type array for concatenation
array ( 0 => 1 ) . 0.0 - TypeError Unsupported operand type array for concatenation
array ( 0 => 1 ) . 10.0 - TypeError Unsupported operand type array for concatenation
array ( 0 => 1 ) . 3.14 - TypeError Unsupported operand type array for concatenation
array ( 0 => 1 ) . '0' - TypeError Unsupported operand type array for concatenation
array ( 0 => 1 ) . '10' - TypeError Unsupported operand type array for concatenation
array ( 0 => 1 ) . '010' - TypeError Unsupported operand type array for concatenation
array ( 0 => 1 ) . '10 elephants' - TypeError Unsupported operand type array for concatenation
array ( 0 => 1 ) . 'foo' - TypeError Unsupported operand type array for concatenation
array ( 0 => 1 ) . array ( ) - TypeError Unsupported operand type array for concatenation
array ( 0 => 1 ) . array ( 0 => 1 ) - TypeError Unsupported operand type array for concatenation
array ( 0 => 1 ) . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for concatenation
array ( 0 => 1 ) . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for concatenation
array ( 0 => 1 ) . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for concatenation
array ( 0 => 1 ) . (object) array ( ) - TypeError Unsupported operand type array for concatenation
array ( 0 => 1 ) . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for concatenation
array ( 0 => 1 ) . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for concatenation
array ( 0 => 1 ) . DateTime - TypeError Unsupported operand type array for concatenation
array ( 0 => 1 ) . resource - TypeError Unsupported operand type array for concatenation
array ( 0 => 1 ) . NULL - TypeError Unsupported operand type array for concatenation
array ( 0 => 1, 1 => 100 ) . false - TypeError Unsupported operand type array for concatenation
array ( 0 => 1, 1 => 100 ) . true - TypeError Unsupported operand type array for concatenation
array ( 0 => 1, 1 => 100 ) . 0 - TypeError Unsupported operand type array for concatenation
array ( 0 => 1, 1 => 100 ) . 10 - TypeError Unsupported operand type array for concatenation
array ( 0 => 1, 1 => 100 ) . 0.0 - TypeError Unsupported operand type array for concatenation
array ( 0 => 1, 1 => 100 ) . 10.0 - TypeError Unsupported operand type array for concatenation
array ( 0 => 1, 1 => 100 ) . 3.14 - TypeError Unsupported operand type array for concatenation
array ( 0 => 1, 1 => 100 ) . '0' - TypeError Unsupported operand type array for concatenation
array ( 0 => 1, 1 => 100 ) . '10' - TypeError Unsupported operand type array for concatenation
array ( 0 => 1, 1 => 100 ) . '010' - TypeError Unsupported operand type array for concatenation
array ( 0 => 1, 1 => 100 ) . '10 elephants' - TypeError Unsupported operand type array for concatenation
array ( 0 => 1, 1 => 100 ) . 'foo' - TypeError Unsupported operand type array for concatenation
array ( 0 => 1, 1 => 100 ) . array ( ) - TypeError Unsupported operand type array for concatenation
array ( 0 => 1, 1 => 100 ) . array ( 0 => 1 ) - TypeError Unsupported operand type array for concatenation
array ( 0 => 1, 1 => 100 ) . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for concatenation
array ( 0 => 1, 1 => 100 ) . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for concatenation
array ( 0 => 1, 1 => 100 ) . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for concatenation
array ( 0 => 1, 1 => 100 ) . (object) array ( ) - TypeError Unsupported operand type array for concatenation
array ( 0 => 1, 1 => 100 ) . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for concatenation
array ( 0 => 1, 1 => 100 ) . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for concatenation
array ( 0 => 1, 1 => 100 ) . DateTime - TypeError Unsupported operand type array for concatenation
array ( 0 => 1, 1 => 100 ) . resource - TypeError Unsupported operand type array for concatenation
array ( 0 => 1, 1 => 100 ) . NULL - TypeError Unsupported operand type array for concatenation
array ( 'foo' => 1, 'bar' => 2 ) . false - TypeError Unsupported operand type array for concatenation
array ( 'foo' => 1, 'bar' => 2 ) . true - TypeError Unsupported operand type array for concatenation
array ( 'foo' => 1, 'bar' => 2 ) . 0 - TypeError Unsupported operand type array for concatenation
array ( 'foo' => 1, 'bar' => 2 ) . 10 - TypeError Unsupported operand type array for concatenation
array ( 'foo' => 1, 'bar' => 2 ) . 0.0 - TypeError Unsupported operand type array for concatenation
array ( 'foo' => 1, 'bar' => 2 ) . 10.0 - TypeError Unsupported operand type array for concatenation
array ( 'foo' => 1, 'bar' => 2 ) . 3.14 - TypeError Unsupported operand type array for concatenation
array ( 'foo' => 1, 'bar' => 2 ) . '0' - TypeError Unsupported operand type array for concatenation
array ( 'foo' => 1, 'bar' => 2 ) . '10' - TypeError Unsupported operand type array for concatenation
array ( 'foo' => 1, 'bar' => 2 ) . '010' - TypeError Unsupported operand type array for concatenation
array ( 'foo' => 1, 'bar' => 2 ) . '10 elephants' - TypeError Unsupported operand type array for concatenation
array ( 'foo' => 1, 'bar' => 2 ) . 'foo' - TypeError Unsupported operand type array for concatenation
array ( 'foo' => 1, 'bar' => 2 ) . array ( ) - TypeError Unsupported operand type array for concatenation
array ( 'foo' => 1, 'bar' => 2 ) . array ( 0 => 1 ) - TypeError Unsupported operand type array for concatenation
array ( 'foo' => 1, 'bar' => 2 ) . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for concatenation
array ( 'foo' => 1, 'bar' => 2 ) . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for concatenation
array ( 'foo' => 1, 'bar' => 2 ) . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for concatenation
array ( 'foo' => 1, 'bar' => 2 ) . (object) array ( ) - TypeError Unsupported operand type array for concatenation
array ( 'foo' => 1, 'bar' => 2 ) . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for concatenation
array ( 'foo' => 1, 'bar' => 2 ) . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for concatenation
array ( 'foo' => 1, 'bar' => 2 ) . DateTime - TypeError Unsupported operand type array for concatenation
array ( 'foo' => 1, 'bar' => 2 ) . resource - TypeError Unsupported operand type array for concatenation
array ( 'foo' => 1, 'bar' => 2 ) . NULL - TypeError Unsupported operand type array for concatenation
array ( 'bar' => 1, 'foo' => 2 ) . false - TypeError Unsupported operand type array for concatenation
array ( 'bar' => 1, 'foo' => 2 ) . true - TypeError Unsupported operand type array for concatenation
array ( 'bar' => 1, 'foo' => 2 ) . 0 - TypeError Unsupported operand type array for concatenation
array ( 'bar' => 1, 'foo' => 2 ) . 10 - TypeError Unsupported operand type array for concatenation
array ( 'bar' => 1, 'foo' => 2 ) . 0.0 - TypeError Unsupported operand type array for concatenation
array ( 'bar' => 1, 'foo' => 2 ) . 10.0 - TypeError Unsupported operand type array for concatenation
array ( 'bar' => 1, 'foo' => 2 ) . 3.14 - TypeError Unsupported operand type array for concatenation
array ( 'bar' => 1, 'foo' => 2 ) . '0' - TypeError Unsupported operand type array for concatenation
array ( 'bar' => 1, 'foo' => 2 ) . '10' - TypeError Unsupported operand type array for concatenation
array ( 'bar' => 1, 'foo' => 2 ) . '010' - TypeError Unsupported operand type array for concatenation
array ( 'bar' => 1, 'foo' => 2 ) . '10 elephants' - TypeError Unsupported operand type array for concatenation
array ( 'bar' => 1, 'foo' => 2 ) . 'foo' - TypeError Unsupported operand type array for concatenation
array ( 'bar' => 1, 'foo' => 2 ) . array ( ) - TypeError Unsupported operand type array for concatenation
array ( 'bar' => 1, 'foo' => 2 ) . array ( 0 => 1 ) - TypeError Unsupported operand type array for concatenation
array ( 'bar' => 1, 'foo' => 2 ) . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for concatenation
array ( 'bar' => 1, 'foo' => 2 ) . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for concatenation
array ( 'bar' => 1, 'foo' => 2 ) . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for concatenation
array ( 'bar' => 1, 'foo' => 2 ) . (object) array ( ) - TypeError Unsupported operand type array for concatenation
array ( 'bar' => 1, 'foo' => 2 ) . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for concatenation
array ( 'bar' => 1, 'foo' => 2 ) . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for concatenation
array ( 'bar' => 1, 'foo' => 2 ) . DateTime - TypeError Unsupported operand type array for concatenation
array ( 'bar' => 1, 'foo' => 2 ) . resource - TypeError Unsupported operand type array for concatenation
array ( 'bar' => 1, 'foo' => 2 ) . NULL - TypeError Unsupported operand type array for concatenation
(object) array ( ) . false - TypeError Unsupported operand type bool for concatenation
(object) array ( ) . true - TypeError Unsupported operand type bool for concatenation
(object) array ( ) . 0 - TypeError Object of class stdClass could not be converted to string
(object) array ( ) . 10 - TypeError Object of class stdClass could not be converted to string
(object) array ( ) . 0.0 - TypeError Object of class stdClass could not be converted to string
(object) array ( ) . 10.0 - TypeError Object of class stdClass could not be converted to string
(object) array ( ) . 3.14 - TypeError Object of class stdClass could not be converted to string
(object) array ( ) . '0' - TypeError Object of class stdClass could not be converted to string
(object) array ( ) . '10' - TypeError Object of class stdClass could not be converted to string
(object) array ( ) . '010' - TypeError Object of class stdClass could not be converted to string
(object) array ( ) . '10 elephants' - TypeError Object of class stdClass could not be converted to string
(object) array ( ) . 'foo' - TypeError Object of class stdClass could not be converted to string
(object) array ( ) . array ( ) - TypeError Unsupported operand type array for concatenation
(object) array ( ) . array ( 0 => 1 ) - TypeError Unsupported operand type array for concatenation
(object) array ( ) . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for concatenation
(object) array ( ) . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for concatenation
(object) array ( ) . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for concatenation
(object) array ( ) . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
(object) array ( ) . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
(object) array ( ) . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
(object) array ( ) . DateTime - TypeError Object of class stdClass could not be converted to string
(object) array ( ) . resource - TypeError Unsupported operand type resource for concatenation
(object) array ( ) . NULL - TypeError Unsupported operand type null for concatenation
(object) array ( 'foo' => 1, 'bar' => 2 ) . false - TypeError Unsupported operand type bool for concatenation
(object) array ( 'foo' => 1, 'bar' => 2 ) . true - TypeError Unsupported operand type bool for concatenation
(object) array ( 'foo' => 1, 'bar' => 2 ) . 0 - TypeError Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . 10 - TypeError Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . 0.0 - TypeError Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . 10.0 - TypeError Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . 3.14 - TypeError Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . '0' - TypeError Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . '10' - TypeError Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . '010' - TypeError Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . '10 elephants' - TypeError Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . 'foo' - TypeError Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . array ( ) - TypeError Unsupported operand type array for concatenation
(object) array ( 'foo' => 1, 'bar' => 2 ) . array ( 0 => 1 ) - TypeError Unsupported operand type array for concatenation
(object) array ( 'foo' => 1, 'bar' => 2 ) . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for concatenation
(object) array ( 'foo' => 1, 'bar' => 2 ) . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for concatenation
(object) array ( 'foo' => 1, 'bar' => 2 ) . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for concatenation
(object) array ( 'foo' => 1, 'bar' => 2 ) . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . DateTime - TypeError Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . resource - TypeError Unsupported operand type resource for concatenation
(object) array ( 'foo' => 1, 'bar' => 2 ) . NULL - TypeError Unsupported operand type null for concatenation
(object) array ( 'bar' => 1, 'foo' => 2 ) . false - TypeError Unsupported operand type bool for concatenation
(object) array ( 'bar' => 1, 'foo' => 2 ) . true - TypeError Unsupported operand type bool for concatenation
(object) array ( 'bar' => 1, 'foo' => 2 ) . 0 - TypeError Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . 10 - TypeError Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . 0.0 - TypeError Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . 10.0 - TypeError Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . 3.14 - TypeError Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . '0' - TypeError Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . '10' - TypeError Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . '010' - TypeError Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . '10 elephants' - TypeError Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . 'foo' - TypeError Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . array ( ) - TypeError Unsupported operand type array for concatenation
(object) array ( 'bar' => 1, 'foo' => 2 ) . array ( 0 => 1 ) - TypeError Unsupported operand type array for concatenation
(object) array ( 'bar' => 1, 'foo' => 2 ) . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for concatenation
(object) array ( 'bar' => 1, 'foo' => 2 ) . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for concatenation
(object) array ( 'bar' => 1, 'foo' => 2 ) . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for concatenation
(object) array ( 'bar' => 1, 'foo' => 2 ) . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . DateTime - TypeError Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . resource - TypeError Unsupported operand type resource for concatenation
(object) array ( 'bar' => 1, 'foo' => 2 ) . NULL - TypeError Unsupported operand type null for concatenation
DateTime . false - TypeError Unsupported operand type bool for concatenation
DateTime . true - TypeError Unsupported operand type bool for concatenation
DateTime . 0 - TypeError Object of class DateTime could not be converted to string
DateTime . 10 - TypeError Object of class DateTime could not be converted to string
DateTime . 0.0 - TypeError Object of class DateTime could not be converted to string
DateTime . 10.0 - TypeError Object of class DateTime could not be converted to string
DateTime . 3.14 - TypeError Object of class DateTime could not be converted to string
DateTime . '0' - TypeError Object of class DateTime could not be converted to string
DateTime . '10' - TypeError Object of class DateTime could not be converted to string
DateTime . '010' - TypeError Object of class DateTime could not be converted to string
DateTime . '10 elephants' - TypeError Object of class DateTime could not be converted to string
DateTime . 'foo' - TypeError Object of class DateTime could not be converted to string
DateTime . array ( ) - TypeError Unsupported operand type array for concatenation
DateTime . array ( 0 => 1 ) - TypeError Unsupported operand type array for concatenation
DateTime . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for concatenation
DateTime . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for concatenation
DateTime . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for concatenation
DateTime . (object) array ( ) - TypeError Object of class DateTime could not be converted to string
DateTime . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class DateTime could not be converted to string
DateTime . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class DateTime could not be converted to string
DateTime . DateTime - TypeError Object of class DateTime could not be converted to string
DateTime . resource - TypeError Unsupported operand type resource for concatenation
DateTime . NULL - TypeError Unsupported operand type null for concatenation
resource . false - TypeError Unsupported operand type resource for concatenation
resource . true - TypeError Unsupported operand type resource for concatenation
resource . 0 - TypeError Unsupported operand type resource for concatenation
resource . 10 - TypeError Unsupported operand type resource for concatenation
resource . 0.0 - TypeError Unsupported operand type resource for concatenation
resource . 10.0 - TypeError Unsupported operand type resource for concatenation
resource . 3.14 - TypeError Unsupported operand type resource for concatenation
resource . '0' - TypeError Unsupported operand type resource for concatenation
resource . '10' - TypeError Unsupported operand type resource for concatenation
resource . '010' - TypeError Unsupported operand type resource for concatenation
resource . '10 elephants' - TypeError Unsupported operand type resource for concatenation
resource . 'foo' - TypeError Unsupported operand type resource for concatenation
resource . array ( ) - TypeError Unsupported operand type resource for concatenation
resource . array ( 0 => 1 ) - TypeError Unsupported operand type resource for concatenation
resource . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type resource for concatenation
resource . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for concatenation
resource . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for concatenation
resource . (object) array ( ) - TypeError Unsupported operand type resource for concatenation
resource . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type resource for concatenation
resource . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type resource for concatenation
resource . DateTime - TypeError Unsupported operand type resource for concatenation
resource . resource - TypeError Unsupported operand type resource for concatenation
resource . NULL - TypeError Unsupported operand type resource for concatenation
NULL . false - TypeError Unsupported operand type null for concatenation
NULL . true - TypeError Unsupported operand type null for concatenation
NULL . 0 - TypeError Unsupported operand type null for concatenation
NULL . 10 - TypeError Unsupported operand type null for concatenation
NULL . 0.0 - TypeError Unsupported operand type null for concatenation
NULL . 10.0 - TypeError Unsupported operand type null for concatenation
NULL . 3.14 - TypeError Unsupported operand type null for concatenation
NULL . '0' - TypeError Unsupported operand type null for concatenation
NULL . '10' - TypeError Unsupported operand type null for concatenation
NULL . '010' - TypeError Unsupported operand type null for concatenation
NULL . '10 elephants' - TypeError Unsupported operand type null for concatenation
NULL . 'foo' - TypeError Unsupported operand type null for concatenation
NULL . array ( ) - TypeError Unsupported operand type null for concatenation
NULL . array ( 0 => 1 ) - TypeError Unsupported operand type null for concatenation
NULL . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type null for concatenation
NULL . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for concatenation
NULL . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for concatenation
NULL . (object) array ( ) - TypeError Unsupported operand type null for concatenation
NULL . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type null for concatenation
NULL . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type null for concatenation
NULL . DateTime - TypeError Unsupported operand type null for concatenation
NULL . resource - TypeError Unsupported operand type null for concatenation
NULL . NULL - TypeError Unsupported operand type null for concatenation