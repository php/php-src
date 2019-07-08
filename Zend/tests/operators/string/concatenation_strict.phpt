--TEST--
concatenation operator with strict_operators
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a . $b', function($a, $b) { return $a . $b; });

--EXPECT--
false . false - TypeError Unsupported operands
false . true - TypeError Unsupported operands
false . 0 - TypeError Unsupported operands
false . 10 - TypeError Unsupported operands
false . 0.0 - TypeError Unsupported operands
false . 10.0 - TypeError Unsupported operands
false . 3.14 - TypeError Unsupported operands
false . '0' - TypeError Unsupported operands
false . '10' - TypeError Unsupported operands
false . '010' - TypeError Unsupported operands
false . '10 elephants' - TypeError Unsupported operands
false . 'foo' - TypeError Unsupported operands
false . array ( ) - TypeError Unsupported operands
false . array ( 0 => 1 ) - TypeError Unsupported operands
false . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operands
false . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
false . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
false . (object) array ( ) - TypeError Unsupported operands
false . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
false . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
false . DateTime - TypeError Unsupported operands
false . resource - TypeError Unsupported operands
false . NULL - TypeError Unsupported operands
true . false - TypeError Unsupported operands
true . true - TypeError Unsupported operands
true . 0 - TypeError Unsupported operands
true . 10 - TypeError Unsupported operands
true . 0.0 - TypeError Unsupported operands
true . 10.0 - TypeError Unsupported operands
true . 3.14 - TypeError Unsupported operands
true . '0' - TypeError Unsupported operands
true . '10' - TypeError Unsupported operands
true . '010' - TypeError Unsupported operands
true . '10 elephants' - TypeError Unsupported operands
true . 'foo' - TypeError Unsupported operands
true . array ( ) - TypeError Unsupported operands
true . array ( 0 => 1 ) - TypeError Unsupported operands
true . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operands
true . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
true . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
true . (object) array ( ) - TypeError Unsupported operands
true . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
true . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
true . DateTime - TypeError Unsupported operands
true . resource - TypeError Unsupported operands
true . NULL - TypeError Unsupported operands
0 . false - TypeError Unsupported operands
0 . true - TypeError Unsupported operands
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
0 . array ( ) - TypeError Unsupported operands
0 . array ( 0 => 1 ) - TypeError Unsupported operands
0 . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operands
0 . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
0 . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
0 . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
0 . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
0 . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
0 . DateTime - TypeError Object of class DateTime could not be converted to string
0 . resource - TypeError Unsupported operands
0 . NULL - TypeError Unsupported operands
10 . false - TypeError Unsupported operands
10 . true - TypeError Unsupported operands
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
10 . array ( ) - TypeError Unsupported operands
10 . array ( 0 => 1 ) - TypeError Unsupported operands
10 . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operands
10 . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
10 . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
10 . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
10 . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
10 . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
10 . DateTime - TypeError Object of class DateTime could not be converted to string
10 . resource - TypeError Unsupported operands
10 . NULL - TypeError Unsupported operands
0.0 . false - TypeError Unsupported operands
0.0 . true - TypeError Unsupported operands
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
0.0 . array ( ) - TypeError Unsupported operands
0.0 . array ( 0 => 1 ) - TypeError Unsupported operands
0.0 . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operands
0.0 . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
0.0 . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
0.0 . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
0.0 . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
0.0 . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
0.0 . DateTime - TypeError Object of class DateTime could not be converted to string
0.0 . resource - TypeError Unsupported operands
0.0 . NULL - TypeError Unsupported operands
10.0 . false - TypeError Unsupported operands
10.0 . true - TypeError Unsupported operands
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
10.0 . array ( ) - TypeError Unsupported operands
10.0 . array ( 0 => 1 ) - TypeError Unsupported operands
10.0 . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operands
10.0 . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
10.0 . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
10.0 . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
10.0 . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
10.0 . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
10.0 . DateTime - TypeError Object of class DateTime could not be converted to string
10.0 . resource - TypeError Unsupported operands
10.0 . NULL - TypeError Unsupported operands
3.14 . false - TypeError Unsupported operands
3.14 . true - TypeError Unsupported operands
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
3.14 . array ( ) - TypeError Unsupported operands
3.14 . array ( 0 => 1 ) - TypeError Unsupported operands
3.14 . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operands
3.14 . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
3.14 . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
3.14 . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
3.14 . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
3.14 . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
3.14 . DateTime - TypeError Object of class DateTime could not be converted to string
3.14 . resource - TypeError Unsupported operands
3.14 . NULL - TypeError Unsupported operands
'0' . false - TypeError Unsupported operands
'0' . true - TypeError Unsupported operands
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
'0' . array ( ) - TypeError Unsupported operands
'0' . array ( 0 => 1 ) - TypeError Unsupported operands
'0' . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operands
'0' . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
'0' . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
'0' . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
'0' . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
'0' . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
'0' . DateTime - TypeError Object of class DateTime could not be converted to string
'0' . resource - TypeError Unsupported operands
'0' . NULL - TypeError Unsupported operands
'10' . false - TypeError Unsupported operands
'10' . true - TypeError Unsupported operands
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
'10' . array ( ) - TypeError Unsupported operands
'10' . array ( 0 => 1 ) - TypeError Unsupported operands
'10' . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operands
'10' . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
'10' . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
'10' . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
'10' . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
'10' . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
'10' . DateTime - TypeError Object of class DateTime could not be converted to string
'10' . resource - TypeError Unsupported operands
'10' . NULL - TypeError Unsupported operands
'010' . false - TypeError Unsupported operands
'010' . true - TypeError Unsupported operands
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
'010' . array ( ) - TypeError Unsupported operands
'010' . array ( 0 => 1 ) - TypeError Unsupported operands
'010' . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operands
'010' . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
'010' . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
'010' . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
'010' . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
'010' . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
'010' . DateTime - TypeError Object of class DateTime could not be converted to string
'010' . resource - TypeError Unsupported operands
'010' . NULL - TypeError Unsupported operands
'10 elephants' . false - TypeError Unsupported operands
'10 elephants' . true - TypeError Unsupported operands
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
'10 elephants' . array ( ) - TypeError Unsupported operands
'10 elephants' . array ( 0 => 1 ) - TypeError Unsupported operands
'10 elephants' . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operands
'10 elephants' . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
'10 elephants' . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
'10 elephants' . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
'10 elephants' . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
'10 elephants' . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
'10 elephants' . DateTime - TypeError Object of class DateTime could not be converted to string
'10 elephants' . resource - TypeError Unsupported operands
'10 elephants' . NULL - TypeError Unsupported operands
'foo' . false - TypeError Unsupported operands
'foo' . true - TypeError Unsupported operands
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
'foo' . array ( ) - TypeError Unsupported operands
'foo' . array ( 0 => 1 ) - TypeError Unsupported operands
'foo' . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operands
'foo' . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
'foo' . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
'foo' . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
'foo' . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
'foo' . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
'foo' . DateTime - TypeError Object of class DateTime could not be converted to string
'foo' . resource - TypeError Unsupported operands
'foo' . NULL - TypeError Unsupported operands
array ( ) . false - TypeError Unsupported operands
array ( ) . true - TypeError Unsupported operands
array ( ) . 0 - TypeError Unsupported operands
array ( ) . 10 - TypeError Unsupported operands
array ( ) . 0.0 - TypeError Unsupported operands
array ( ) . 10.0 - TypeError Unsupported operands
array ( ) . 3.14 - TypeError Unsupported operands
array ( ) . '0' - TypeError Unsupported operands
array ( ) . '10' - TypeError Unsupported operands
array ( ) . '010' - TypeError Unsupported operands
array ( ) . '10 elephants' - TypeError Unsupported operands
array ( ) . 'foo' - TypeError Unsupported operands
array ( ) . array ( ) - TypeError Unsupported operands
array ( ) . array ( 0 => 1 ) - TypeError Unsupported operands
array ( ) . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operands
array ( ) . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
array ( ) . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
array ( ) . (object) array ( ) - TypeError Unsupported operands
array ( ) . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
array ( ) . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
array ( ) . DateTime - TypeError Unsupported operands
array ( ) . resource - TypeError Unsupported operands
array ( ) . NULL - TypeError Unsupported operands
array ( 0 => 1 ) . false - TypeError Unsupported operands
array ( 0 => 1 ) . true - TypeError Unsupported operands
array ( 0 => 1 ) . 0 - TypeError Unsupported operands
array ( 0 => 1 ) . 10 - TypeError Unsupported operands
array ( 0 => 1 ) . 0.0 - TypeError Unsupported operands
array ( 0 => 1 ) . 10.0 - TypeError Unsupported operands
array ( 0 => 1 ) . 3.14 - TypeError Unsupported operands
array ( 0 => 1 ) . '0' - TypeError Unsupported operands
array ( 0 => 1 ) . '10' - TypeError Unsupported operands
array ( 0 => 1 ) . '010' - TypeError Unsupported operands
array ( 0 => 1 ) . '10 elephants' - TypeError Unsupported operands
array ( 0 => 1 ) . 'foo' - TypeError Unsupported operands
array ( 0 => 1 ) . array ( ) - TypeError Unsupported operands
array ( 0 => 1 ) . array ( 0 => 1 ) - TypeError Unsupported operands
array ( 0 => 1 ) . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operands
array ( 0 => 1 ) . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
array ( 0 => 1 ) . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
array ( 0 => 1 ) . (object) array ( ) - TypeError Unsupported operands
array ( 0 => 1 ) . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
array ( 0 => 1 ) . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
array ( 0 => 1 ) . DateTime - TypeError Unsupported operands
array ( 0 => 1 ) . resource - TypeError Unsupported operands
array ( 0 => 1 ) . NULL - TypeError Unsupported operands
array ( 0 => 1, 1 => 100 ) . false - TypeError Unsupported operands
array ( 0 => 1, 1 => 100 ) . true - TypeError Unsupported operands
array ( 0 => 1, 1 => 100 ) . 0 - TypeError Unsupported operands
array ( 0 => 1, 1 => 100 ) . 10 - TypeError Unsupported operands
array ( 0 => 1, 1 => 100 ) . 0.0 - TypeError Unsupported operands
array ( 0 => 1, 1 => 100 ) . 10.0 - TypeError Unsupported operands
array ( 0 => 1, 1 => 100 ) . 3.14 - TypeError Unsupported operands
array ( 0 => 1, 1 => 100 ) . '0' - TypeError Unsupported operands
array ( 0 => 1, 1 => 100 ) . '10' - TypeError Unsupported operands
array ( 0 => 1, 1 => 100 ) . '010' - TypeError Unsupported operands
array ( 0 => 1, 1 => 100 ) . '10 elephants' - TypeError Unsupported operands
array ( 0 => 1, 1 => 100 ) . 'foo' - TypeError Unsupported operands
array ( 0 => 1, 1 => 100 ) . array ( ) - TypeError Unsupported operands
array ( 0 => 1, 1 => 100 ) . array ( 0 => 1 ) - TypeError Unsupported operands
array ( 0 => 1, 1 => 100 ) . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operands
array ( 0 => 1, 1 => 100 ) . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
array ( 0 => 1, 1 => 100 ) . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
array ( 0 => 1, 1 => 100 ) . (object) array ( ) - TypeError Unsupported operands
array ( 0 => 1, 1 => 100 ) . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
array ( 0 => 1, 1 => 100 ) . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
array ( 0 => 1, 1 => 100 ) . DateTime - TypeError Unsupported operands
array ( 0 => 1, 1 => 100 ) . resource - TypeError Unsupported operands
array ( 0 => 1, 1 => 100 ) . NULL - TypeError Unsupported operands
array ( 'foo' => 1, 'bar' => 2 ) . false - TypeError Unsupported operands
array ( 'foo' => 1, 'bar' => 2 ) . true - TypeError Unsupported operands
array ( 'foo' => 1, 'bar' => 2 ) . 0 - TypeError Unsupported operands
array ( 'foo' => 1, 'bar' => 2 ) . 10 - TypeError Unsupported operands
array ( 'foo' => 1, 'bar' => 2 ) . 0.0 - TypeError Unsupported operands
array ( 'foo' => 1, 'bar' => 2 ) . 10.0 - TypeError Unsupported operands
array ( 'foo' => 1, 'bar' => 2 ) . 3.14 - TypeError Unsupported operands
array ( 'foo' => 1, 'bar' => 2 ) . '0' - TypeError Unsupported operands
array ( 'foo' => 1, 'bar' => 2 ) . '10' - TypeError Unsupported operands
array ( 'foo' => 1, 'bar' => 2 ) . '010' - TypeError Unsupported operands
array ( 'foo' => 1, 'bar' => 2 ) . '10 elephants' - TypeError Unsupported operands
array ( 'foo' => 1, 'bar' => 2 ) . 'foo' - TypeError Unsupported operands
array ( 'foo' => 1, 'bar' => 2 ) . array ( ) - TypeError Unsupported operands
array ( 'foo' => 1, 'bar' => 2 ) . array ( 0 => 1 ) - TypeError Unsupported operands
array ( 'foo' => 1, 'bar' => 2 ) . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operands
array ( 'foo' => 1, 'bar' => 2 ) . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
array ( 'foo' => 1, 'bar' => 2 ) . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
array ( 'foo' => 1, 'bar' => 2 ) . (object) array ( ) - TypeError Unsupported operands
array ( 'foo' => 1, 'bar' => 2 ) . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
array ( 'foo' => 1, 'bar' => 2 ) . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
array ( 'foo' => 1, 'bar' => 2 ) . DateTime - TypeError Unsupported operands
array ( 'foo' => 1, 'bar' => 2 ) . resource - TypeError Unsupported operands
array ( 'foo' => 1, 'bar' => 2 ) . NULL - TypeError Unsupported operands
array ( 'bar' => 1, 'foo' => 2 ) . false - TypeError Unsupported operands
array ( 'bar' => 1, 'foo' => 2 ) . true - TypeError Unsupported operands
array ( 'bar' => 1, 'foo' => 2 ) . 0 - TypeError Unsupported operands
array ( 'bar' => 1, 'foo' => 2 ) . 10 - TypeError Unsupported operands
array ( 'bar' => 1, 'foo' => 2 ) . 0.0 - TypeError Unsupported operands
array ( 'bar' => 1, 'foo' => 2 ) . 10.0 - TypeError Unsupported operands
array ( 'bar' => 1, 'foo' => 2 ) . 3.14 - TypeError Unsupported operands
array ( 'bar' => 1, 'foo' => 2 ) . '0' - TypeError Unsupported operands
array ( 'bar' => 1, 'foo' => 2 ) . '10' - TypeError Unsupported operands
array ( 'bar' => 1, 'foo' => 2 ) . '010' - TypeError Unsupported operands
array ( 'bar' => 1, 'foo' => 2 ) . '10 elephants' - TypeError Unsupported operands
array ( 'bar' => 1, 'foo' => 2 ) . 'foo' - TypeError Unsupported operands
array ( 'bar' => 1, 'foo' => 2 ) . array ( ) - TypeError Unsupported operands
array ( 'bar' => 1, 'foo' => 2 ) . array ( 0 => 1 ) - TypeError Unsupported operands
array ( 'bar' => 1, 'foo' => 2 ) . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operands
array ( 'bar' => 1, 'foo' => 2 ) . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
array ( 'bar' => 1, 'foo' => 2 ) . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
array ( 'bar' => 1, 'foo' => 2 ) . (object) array ( ) - TypeError Unsupported operands
array ( 'bar' => 1, 'foo' => 2 ) . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
array ( 'bar' => 1, 'foo' => 2 ) . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
array ( 'bar' => 1, 'foo' => 2 ) . DateTime - TypeError Unsupported operands
array ( 'bar' => 1, 'foo' => 2 ) . resource - TypeError Unsupported operands
array ( 'bar' => 1, 'foo' => 2 ) . NULL - TypeError Unsupported operands
(object) array ( ) . false - TypeError Unsupported operands
(object) array ( ) . true - TypeError Unsupported operands
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
(object) array ( ) . array ( ) - TypeError Unsupported operands
(object) array ( ) . array ( 0 => 1 ) - TypeError Unsupported operands
(object) array ( ) . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operands
(object) array ( ) . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
(object) array ( ) . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
(object) array ( ) . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
(object) array ( ) . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
(object) array ( ) . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
(object) array ( ) . DateTime - TypeError Object of class stdClass could not be converted to string
(object) array ( ) . resource - TypeError Unsupported operands
(object) array ( ) . NULL - TypeError Unsupported operands
(object) array ( 'foo' => 1, 'bar' => 2 ) . false - TypeError Unsupported operands
(object) array ( 'foo' => 1, 'bar' => 2 ) . true - TypeError Unsupported operands
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
(object) array ( 'foo' => 1, 'bar' => 2 ) . array ( ) - TypeError Unsupported operands
(object) array ( 'foo' => 1, 'bar' => 2 ) . array ( 0 => 1 ) - TypeError Unsupported operands
(object) array ( 'foo' => 1, 'bar' => 2 ) . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operands
(object) array ( 'foo' => 1, 'bar' => 2 ) . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
(object) array ( 'foo' => 1, 'bar' => 2 ) . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
(object) array ( 'foo' => 1, 'bar' => 2 ) . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . DateTime - TypeError Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . resource - TypeError Unsupported operands
(object) array ( 'foo' => 1, 'bar' => 2 ) . NULL - TypeError Unsupported operands
(object) array ( 'bar' => 1, 'foo' => 2 ) . false - TypeError Unsupported operands
(object) array ( 'bar' => 1, 'foo' => 2 ) . true - TypeError Unsupported operands
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
(object) array ( 'bar' => 1, 'foo' => 2 ) . array ( ) - TypeError Unsupported operands
(object) array ( 'bar' => 1, 'foo' => 2 ) . array ( 0 => 1 ) - TypeError Unsupported operands
(object) array ( 'bar' => 1, 'foo' => 2 ) . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operands
(object) array ( 'bar' => 1, 'foo' => 2 ) . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
(object) array ( 'bar' => 1, 'foo' => 2 ) . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
(object) array ( 'bar' => 1, 'foo' => 2 ) . (object) array ( ) - TypeError Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . DateTime - TypeError Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . resource - TypeError Unsupported operands
(object) array ( 'bar' => 1, 'foo' => 2 ) . NULL - TypeError Unsupported operands
DateTime . false - TypeError Unsupported operands
DateTime . true - TypeError Unsupported operands
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
DateTime . array ( ) - TypeError Unsupported operands
DateTime . array ( 0 => 1 ) - TypeError Unsupported operands
DateTime . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operands
DateTime . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
DateTime . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
DateTime . (object) array ( ) - TypeError Object of class DateTime could not be converted to string
DateTime . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Object of class DateTime could not be converted to string
DateTime . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Object of class DateTime could not be converted to string
DateTime . DateTime - TypeError Object of class DateTime could not be converted to string
DateTime . resource - TypeError Unsupported operands
DateTime . NULL - TypeError Unsupported operands
resource . false - TypeError Unsupported operands
resource . true - TypeError Unsupported operands
resource . 0 - TypeError Unsupported operands
resource . 10 - TypeError Unsupported operands
resource . 0.0 - TypeError Unsupported operands
resource . 10.0 - TypeError Unsupported operands
resource . 3.14 - TypeError Unsupported operands
resource . '0' - TypeError Unsupported operands
resource . '10' - TypeError Unsupported operands
resource . '010' - TypeError Unsupported operands
resource . '10 elephants' - TypeError Unsupported operands
resource . 'foo' - TypeError Unsupported operands
resource . array ( ) - TypeError Unsupported operands
resource . array ( 0 => 1 ) - TypeError Unsupported operands
resource . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operands
resource . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
resource . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
resource . (object) array ( ) - TypeError Unsupported operands
resource . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
resource . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
resource . DateTime - TypeError Unsupported operands
resource . resource - TypeError Unsupported operands
resource . NULL - TypeError Unsupported operands
NULL . false - TypeError Unsupported operands
NULL . true - TypeError Unsupported operands
NULL . 0 - TypeError Unsupported operands
NULL . 10 - TypeError Unsupported operands
NULL . 0.0 - TypeError Unsupported operands
NULL . 10.0 - TypeError Unsupported operands
NULL . 3.14 - TypeError Unsupported operands
NULL . '0' - TypeError Unsupported operands
NULL . '10' - TypeError Unsupported operands
NULL . '010' - TypeError Unsupported operands
NULL . '10 elephants' - TypeError Unsupported operands
NULL . 'foo' - TypeError Unsupported operands
NULL . array ( ) - TypeError Unsupported operands
NULL . array ( 0 => 1 ) - TypeError Unsupported operands
NULL . array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operands
NULL . array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
NULL . array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
NULL . (object) array ( ) - TypeError Unsupported operands
NULL . (object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operands
NULL . (object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operands
NULL . DateTime - TypeError Unsupported operands
NULL . resource - TypeError Unsupported operands
NULL . NULL - TypeError Unsupported operands