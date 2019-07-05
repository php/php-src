--TEST--
concatenation operator
--FILE--
<?php
require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a . $b', function($a, $b) { return $a . $b; });

--EXPECT--
false . false = ''
false . true = '1'
false . 0 = '0'
false . 10 = '10'
false . 0.0 = '0'
false . 10.0 = '10'
false . 3.14 = '3.14'
false . '0' = '0'
false . '10' = '10'
false . '010' = '010'
false . '10 elephants' = '10 elephants'
false . 'foo' = 'foo'
false . array ( ) = 'Array' - Notice Array to string conversion
false . array ( 0 => 1 ) = 'Array' - Notice Array to string conversion
false . array ( 0 => 1, 1 => 100 ) = 'Array' - Notice Array to string conversion
false . array ( 'foo' => 1, 'bar' => 2 ) = 'Array' - Notice Array to string conversion
false . array ( 'bar' => 1, 'foo' => 2 ) = 'Array' - Notice Array to string conversion
false . (object) array ( ) - Error Object of class stdClass could not be converted to string
false . (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
false . (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
false . DateTime - Error Object of class DateTime could not be converted to string
false . resource = 'Resource id #6'
false . NULL = ''
true . false = '1'
true . true = '11'
true . 0 = '10'
true . 10 = '110'
true . 0.0 = '10'
true . 10.0 = '110'
true . 3.14 = '13.14'
true . '0' = '10'
true . '10' = '110'
true . '010' = '1010'
true . '10 elephants' = '110 elephants'
true . 'foo' = '1foo'
true . array ( ) = '1Array' - Notice Array to string conversion
true . array ( 0 => 1 ) = '1Array' - Notice Array to string conversion
true . array ( 0 => 1, 1 => 100 ) = '1Array' - Notice Array to string conversion
true . array ( 'foo' => 1, 'bar' => 2 ) = '1Array' - Notice Array to string conversion
true . array ( 'bar' => 1, 'foo' => 2 ) = '1Array' - Notice Array to string conversion
true . (object) array ( ) - Error Object of class stdClass could not be converted to string
true . (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
true . (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
true . DateTime - Error Object of class DateTime could not be converted to string
true . resource = '1Resource id #6'
true . NULL = '1'
0 . false = '0'
0 . true = '01'
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
0 . array ( ) = '0Array' - Notice Array to string conversion
0 . array ( 0 => 1 ) = '0Array' - Notice Array to string conversion
0 . array ( 0 => 1, 1 => 100 ) = '0Array' - Notice Array to string conversion
0 . array ( 'foo' => 1, 'bar' => 2 ) = '0Array' - Notice Array to string conversion
0 . array ( 'bar' => 1, 'foo' => 2 ) = '0Array' - Notice Array to string conversion
0 . (object) array ( ) - Error Object of class stdClass could not be converted to string
0 . (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
0 . (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
0 . DateTime - Error Object of class DateTime could not be converted to string
0 . resource = '0Resource id #6'
0 . NULL = '0'
10 . false = '10'
10 . true = '101'
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
10 . array ( ) = '10Array' - Notice Array to string conversion
10 . array ( 0 => 1 ) = '10Array' - Notice Array to string conversion
10 . array ( 0 => 1, 1 => 100 ) = '10Array' - Notice Array to string conversion
10 . array ( 'foo' => 1, 'bar' => 2 ) = '10Array' - Notice Array to string conversion
10 . array ( 'bar' => 1, 'foo' => 2 ) = '10Array' - Notice Array to string conversion
10 . (object) array ( ) - Error Object of class stdClass could not be converted to string
10 . (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
10 . (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
10 . DateTime - Error Object of class DateTime could not be converted to string
10 . resource = '10Resource id #6'
10 . NULL = '10'
0.0 . false = '0'
0.0 . true = '01'
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
0.0 . array ( ) = '0Array' - Notice Array to string conversion
0.0 . array ( 0 => 1 ) = '0Array' - Notice Array to string conversion
0.0 . array ( 0 => 1, 1 => 100 ) = '0Array' - Notice Array to string conversion
0.0 . array ( 'foo' => 1, 'bar' => 2 ) = '0Array' - Notice Array to string conversion
0.0 . array ( 'bar' => 1, 'foo' => 2 ) = '0Array' - Notice Array to string conversion
0.0 . (object) array ( ) - Error Object of class stdClass could not be converted to string
0.0 . (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
0.0 . (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
0.0 . DateTime - Error Object of class DateTime could not be converted to string
0.0 . resource = '0Resource id #6'
0.0 . NULL = '0'
10.0 . false = '10'
10.0 . true = '101'
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
10.0 . array ( ) = '10Array' - Notice Array to string conversion
10.0 . array ( 0 => 1 ) = '10Array' - Notice Array to string conversion
10.0 . array ( 0 => 1, 1 => 100 ) = '10Array' - Notice Array to string conversion
10.0 . array ( 'foo' => 1, 'bar' => 2 ) = '10Array' - Notice Array to string conversion
10.0 . array ( 'bar' => 1, 'foo' => 2 ) = '10Array' - Notice Array to string conversion
10.0 . (object) array ( ) - Error Object of class stdClass could not be converted to string
10.0 . (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
10.0 . (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
10.0 . DateTime - Error Object of class DateTime could not be converted to string
10.0 . resource = '10Resource id #6'
10.0 . NULL = '10'
3.14 . false = '3.14'
3.14 . true = '3.141'
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
3.14 . array ( ) = '3.14Array' - Notice Array to string conversion
3.14 . array ( 0 => 1 ) = '3.14Array' - Notice Array to string conversion
3.14 . array ( 0 => 1, 1 => 100 ) = '3.14Array' - Notice Array to string conversion
3.14 . array ( 'foo' => 1, 'bar' => 2 ) = '3.14Array' - Notice Array to string conversion
3.14 . array ( 'bar' => 1, 'foo' => 2 ) = '3.14Array' - Notice Array to string conversion
3.14 . (object) array ( ) - Error Object of class stdClass could not be converted to string
3.14 . (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
3.14 . (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
3.14 . DateTime - Error Object of class DateTime could not be converted to string
3.14 . resource = '3.14Resource id #6'
3.14 . NULL = '3.14'
'0' . false = '0'
'0' . true = '01'
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
'0' . array ( ) = '0Array' - Notice Array to string conversion
'0' . array ( 0 => 1 ) = '0Array' - Notice Array to string conversion
'0' . array ( 0 => 1, 1 => 100 ) = '0Array' - Notice Array to string conversion
'0' . array ( 'foo' => 1, 'bar' => 2 ) = '0Array' - Notice Array to string conversion
'0' . array ( 'bar' => 1, 'foo' => 2 ) = '0Array' - Notice Array to string conversion
'0' . (object) array ( ) - Error Object of class stdClass could not be converted to string
'0' . (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
'0' . (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
'0' . DateTime - Error Object of class DateTime could not be converted to string
'0' . resource = '0Resource id #6'
'0' . NULL = '0'
'10' . false = '10'
'10' . true = '101'
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
'10' . array ( ) = '10Array' - Notice Array to string conversion
'10' . array ( 0 => 1 ) = '10Array' - Notice Array to string conversion
'10' . array ( 0 => 1, 1 => 100 ) = '10Array' - Notice Array to string conversion
'10' . array ( 'foo' => 1, 'bar' => 2 ) = '10Array' - Notice Array to string conversion
'10' . array ( 'bar' => 1, 'foo' => 2 ) = '10Array' - Notice Array to string conversion
'10' . (object) array ( ) - Error Object of class stdClass could not be converted to string
'10' . (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
'10' . (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
'10' . DateTime - Error Object of class DateTime could not be converted to string
'10' . resource = '10Resource id #6'
'10' . NULL = '10'
'010' . false = '010'
'010' . true = '0101'
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
'010' . array ( ) = '010Array' - Notice Array to string conversion
'010' . array ( 0 => 1 ) = '010Array' - Notice Array to string conversion
'010' . array ( 0 => 1, 1 => 100 ) = '010Array' - Notice Array to string conversion
'010' . array ( 'foo' => 1, 'bar' => 2 ) = '010Array' - Notice Array to string conversion
'010' . array ( 'bar' => 1, 'foo' => 2 ) = '010Array' - Notice Array to string conversion
'010' . (object) array ( ) - Error Object of class stdClass could not be converted to string
'010' . (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
'010' . (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
'010' . DateTime - Error Object of class DateTime could not be converted to string
'010' . resource = '010Resource id #6'
'010' . NULL = '010'
'10 elephants' . false = '10 elephants'
'10 elephants' . true = '10 elephants1'
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
'10 elephants' . array ( ) = '10 elephantsArray' - Notice Array to string conversion
'10 elephants' . array ( 0 => 1 ) = '10 elephantsArray' - Notice Array to string conversion
'10 elephants' . array ( 0 => 1, 1 => 100 ) = '10 elephantsArray' - Notice Array to string conversion
'10 elephants' . array ( 'foo' => 1, 'bar' => 2 ) = '10 elephantsArray' - Notice Array to string conversion
'10 elephants' . array ( 'bar' => 1, 'foo' => 2 ) = '10 elephantsArray' - Notice Array to string conversion
'10 elephants' . (object) array ( ) - Error Object of class stdClass could not be converted to string
'10 elephants' . (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
'10 elephants' . (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
'10 elephants' . DateTime - Error Object of class DateTime could not be converted to string
'10 elephants' . resource = '10 elephantsResource id #6'
'10 elephants' . NULL = '10 elephants'
'foo' . false = 'foo'
'foo' . true = 'foo1'
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
'foo' . array ( ) = 'fooArray' - Notice Array to string conversion
'foo' . array ( 0 => 1 ) = 'fooArray' - Notice Array to string conversion
'foo' . array ( 0 => 1, 1 => 100 ) = 'fooArray' - Notice Array to string conversion
'foo' . array ( 'foo' => 1, 'bar' => 2 ) = 'fooArray' - Notice Array to string conversion
'foo' . array ( 'bar' => 1, 'foo' => 2 ) = 'fooArray' - Notice Array to string conversion
'foo' . (object) array ( ) - Error Object of class stdClass could not be converted to string
'foo' . (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
'foo' . (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
'foo' . DateTime - Error Object of class DateTime could not be converted to string
'foo' . resource = 'fooResource id #6'
'foo' . NULL = 'foo'
array ( ) . false = 'Array' - Notice Array to string conversion
array ( ) . true = 'Array1' - Notice Array to string conversion
array ( ) . 0 = 'Array0' - Notice Array to string conversion
array ( ) . 10 = 'Array10' - Notice Array to string conversion
array ( ) . 0.0 = 'Array0' - Notice Array to string conversion
array ( ) . 10.0 = 'Array10' - Notice Array to string conversion
array ( ) . 3.14 = 'Array3.14' - Notice Array to string conversion
array ( ) . '0' = 'Array0' - Notice Array to string conversion
array ( ) . '10' = 'Array10' - Notice Array to string conversion
array ( ) . '010' = 'Array010' - Notice Array to string conversion
array ( ) . '10 elephants' = 'Array10 elephants' - Notice Array to string conversion
array ( ) . 'foo' = 'Arrayfoo' - Notice Array to string conversion
array ( ) . array ( ) = 'ArrayArray' - Notice Array to string conversion
array ( ) . array ( 0 => 1 ) = 'ArrayArray' - Notice Array to string conversion
array ( ) . array ( 0 => 1, 1 => 100 ) = 'ArrayArray' - Notice Array to string conversion
array ( ) . array ( 'foo' => 1, 'bar' => 2 ) = 'ArrayArray' - Notice Array to string conversion
array ( ) . array ( 'bar' => 1, 'foo' => 2 ) = 'ArrayArray' - Notice Array to string conversion
array ( ) . (object) array ( ) - Error Object of class stdClass could not be converted to string
array ( ) . (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
array ( ) . (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
array ( ) . DateTime - Error Object of class DateTime could not be converted to string
array ( ) . resource = 'ArrayResource id #6' - Notice Array to string conversion
array ( ) . NULL = 'Array' - Notice Array to string conversion
array ( 0 => 1 ) . false = 'Array' - Notice Array to string conversion
array ( 0 => 1 ) . true = 'Array1' - Notice Array to string conversion
array ( 0 => 1 ) . 0 = 'Array0' - Notice Array to string conversion
array ( 0 => 1 ) . 10 = 'Array10' - Notice Array to string conversion
array ( 0 => 1 ) . 0.0 = 'Array0' - Notice Array to string conversion
array ( 0 => 1 ) . 10.0 = 'Array10' - Notice Array to string conversion
array ( 0 => 1 ) . 3.14 = 'Array3.14' - Notice Array to string conversion
array ( 0 => 1 ) . '0' = 'Array0' - Notice Array to string conversion
array ( 0 => 1 ) . '10' = 'Array10' - Notice Array to string conversion
array ( 0 => 1 ) . '010' = 'Array010' - Notice Array to string conversion
array ( 0 => 1 ) . '10 elephants' = 'Array10 elephants' - Notice Array to string conversion
array ( 0 => 1 ) . 'foo' = 'Arrayfoo' - Notice Array to string conversion
array ( 0 => 1 ) . array ( ) = 'ArrayArray' - Notice Array to string conversion
array ( 0 => 1 ) . array ( 0 => 1 ) = 'ArrayArray' - Notice Array to string conversion
array ( 0 => 1 ) . array ( 0 => 1, 1 => 100 ) = 'ArrayArray' - Notice Array to string conversion
array ( 0 => 1 ) . array ( 'foo' => 1, 'bar' => 2 ) = 'ArrayArray' - Notice Array to string conversion
array ( 0 => 1 ) . array ( 'bar' => 1, 'foo' => 2 ) = 'ArrayArray' - Notice Array to string conversion
array ( 0 => 1 ) . (object) array ( ) - Error Object of class stdClass could not be converted to string
array ( 0 => 1 ) . (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
array ( 0 => 1 ) . (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
array ( 0 => 1 ) . DateTime - Error Object of class DateTime could not be converted to string
array ( 0 => 1 ) . resource = 'ArrayResource id #6' - Notice Array to string conversion
array ( 0 => 1 ) . NULL = 'Array' - Notice Array to string conversion
array ( 0 => 1, 1 => 100 ) . false = 'Array' - Notice Array to string conversion
array ( 0 => 1, 1 => 100 ) . true = 'Array1' - Notice Array to string conversion
array ( 0 => 1, 1 => 100 ) . 0 = 'Array0' - Notice Array to string conversion
array ( 0 => 1, 1 => 100 ) . 10 = 'Array10' - Notice Array to string conversion
array ( 0 => 1, 1 => 100 ) . 0.0 = 'Array0' - Notice Array to string conversion
array ( 0 => 1, 1 => 100 ) . 10.0 = 'Array10' - Notice Array to string conversion
array ( 0 => 1, 1 => 100 ) . 3.14 = 'Array3.14' - Notice Array to string conversion
array ( 0 => 1, 1 => 100 ) . '0' = 'Array0' - Notice Array to string conversion
array ( 0 => 1, 1 => 100 ) . '10' = 'Array10' - Notice Array to string conversion
array ( 0 => 1, 1 => 100 ) . '010' = 'Array010' - Notice Array to string conversion
array ( 0 => 1, 1 => 100 ) . '10 elephants' = 'Array10 elephants' - Notice Array to string conversion
array ( 0 => 1, 1 => 100 ) . 'foo' = 'Arrayfoo' - Notice Array to string conversion
array ( 0 => 1, 1 => 100 ) . array ( ) = 'ArrayArray' - Notice Array to string conversion
array ( 0 => 1, 1 => 100 ) . array ( 0 => 1 ) = 'ArrayArray' - Notice Array to string conversion
array ( 0 => 1, 1 => 100 ) . array ( 0 => 1, 1 => 100 ) = 'ArrayArray' - Notice Array to string conversion
array ( 0 => 1, 1 => 100 ) . array ( 'foo' => 1, 'bar' => 2 ) = 'ArrayArray' - Notice Array to string conversion
array ( 0 => 1, 1 => 100 ) . array ( 'bar' => 1, 'foo' => 2 ) = 'ArrayArray' - Notice Array to string conversion
array ( 0 => 1, 1 => 100 ) . (object) array ( ) - Error Object of class stdClass could not be converted to string
array ( 0 => 1, 1 => 100 ) . (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
array ( 0 => 1, 1 => 100 ) . (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
array ( 0 => 1, 1 => 100 ) . DateTime - Error Object of class DateTime could not be converted to string
array ( 0 => 1, 1 => 100 ) . resource = 'ArrayResource id #6' - Notice Array to string conversion
array ( 0 => 1, 1 => 100 ) . NULL = 'Array' - Notice Array to string conversion
array ( 'foo' => 1, 'bar' => 2 ) . false = 'Array' - Notice Array to string conversion
array ( 'foo' => 1, 'bar' => 2 ) . true = 'Array1' - Notice Array to string conversion
array ( 'foo' => 1, 'bar' => 2 ) . 0 = 'Array0' - Notice Array to string conversion
array ( 'foo' => 1, 'bar' => 2 ) . 10 = 'Array10' - Notice Array to string conversion
array ( 'foo' => 1, 'bar' => 2 ) . 0.0 = 'Array0' - Notice Array to string conversion
array ( 'foo' => 1, 'bar' => 2 ) . 10.0 = 'Array10' - Notice Array to string conversion
array ( 'foo' => 1, 'bar' => 2 ) . 3.14 = 'Array3.14' - Notice Array to string conversion
array ( 'foo' => 1, 'bar' => 2 ) . '0' = 'Array0' - Notice Array to string conversion
array ( 'foo' => 1, 'bar' => 2 ) . '10' = 'Array10' - Notice Array to string conversion
array ( 'foo' => 1, 'bar' => 2 ) . '010' = 'Array010' - Notice Array to string conversion
array ( 'foo' => 1, 'bar' => 2 ) . '10 elephants' = 'Array10 elephants' - Notice Array to string conversion
array ( 'foo' => 1, 'bar' => 2 ) . 'foo' = 'Arrayfoo' - Notice Array to string conversion
array ( 'foo' => 1, 'bar' => 2 ) . array ( ) = 'ArrayArray' - Notice Array to string conversion
array ( 'foo' => 1, 'bar' => 2 ) . array ( 0 => 1 ) = 'ArrayArray' - Notice Array to string conversion
array ( 'foo' => 1, 'bar' => 2 ) . array ( 0 => 1, 1 => 100 ) = 'ArrayArray' - Notice Array to string conversion
array ( 'foo' => 1, 'bar' => 2 ) . array ( 'foo' => 1, 'bar' => 2 ) = 'ArrayArray' - Notice Array to string conversion
array ( 'foo' => 1, 'bar' => 2 ) . array ( 'bar' => 1, 'foo' => 2 ) = 'ArrayArray' - Notice Array to string conversion
array ( 'foo' => 1, 'bar' => 2 ) . (object) array ( ) - Error Object of class stdClass could not be converted to string
array ( 'foo' => 1, 'bar' => 2 ) . (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
array ( 'foo' => 1, 'bar' => 2 ) . (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
array ( 'foo' => 1, 'bar' => 2 ) . DateTime - Error Object of class DateTime could not be converted to string
array ( 'foo' => 1, 'bar' => 2 ) . resource = 'ArrayResource id #6' - Notice Array to string conversion
array ( 'foo' => 1, 'bar' => 2 ) . NULL = 'Array' - Notice Array to string conversion
array ( 'bar' => 1, 'foo' => 2 ) . false = 'Array' - Notice Array to string conversion
array ( 'bar' => 1, 'foo' => 2 ) . true = 'Array1' - Notice Array to string conversion
array ( 'bar' => 1, 'foo' => 2 ) . 0 = 'Array0' - Notice Array to string conversion
array ( 'bar' => 1, 'foo' => 2 ) . 10 = 'Array10' - Notice Array to string conversion
array ( 'bar' => 1, 'foo' => 2 ) . 0.0 = 'Array0' - Notice Array to string conversion
array ( 'bar' => 1, 'foo' => 2 ) . 10.0 = 'Array10' - Notice Array to string conversion
array ( 'bar' => 1, 'foo' => 2 ) . 3.14 = 'Array3.14' - Notice Array to string conversion
array ( 'bar' => 1, 'foo' => 2 ) . '0' = 'Array0' - Notice Array to string conversion
array ( 'bar' => 1, 'foo' => 2 ) . '10' = 'Array10' - Notice Array to string conversion
array ( 'bar' => 1, 'foo' => 2 ) . '010' = 'Array010' - Notice Array to string conversion
array ( 'bar' => 1, 'foo' => 2 ) . '10 elephants' = 'Array10 elephants' - Notice Array to string conversion
array ( 'bar' => 1, 'foo' => 2 ) . 'foo' = 'Arrayfoo' - Notice Array to string conversion
array ( 'bar' => 1, 'foo' => 2 ) . array ( ) = 'ArrayArray' - Notice Array to string conversion
array ( 'bar' => 1, 'foo' => 2 ) . array ( 0 => 1 ) = 'ArrayArray' - Notice Array to string conversion
array ( 'bar' => 1, 'foo' => 2 ) . array ( 0 => 1, 1 => 100 ) = 'ArrayArray' - Notice Array to string conversion
array ( 'bar' => 1, 'foo' => 2 ) . array ( 'foo' => 1, 'bar' => 2 ) = 'ArrayArray' - Notice Array to string conversion
array ( 'bar' => 1, 'foo' => 2 ) . array ( 'bar' => 1, 'foo' => 2 ) = 'ArrayArray' - Notice Array to string conversion
array ( 'bar' => 1, 'foo' => 2 ) . (object) array ( ) - Error Object of class stdClass could not be converted to string
array ( 'bar' => 1, 'foo' => 2 ) . (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
array ( 'bar' => 1, 'foo' => 2 ) . (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
array ( 'bar' => 1, 'foo' => 2 ) . DateTime - Error Object of class DateTime could not be converted to string
array ( 'bar' => 1, 'foo' => 2 ) . resource = 'ArrayResource id #6' - Notice Array to string conversion
array ( 'bar' => 1, 'foo' => 2 ) . NULL = 'Array' - Notice Array to string conversion
(object) array ( ) . false - Error Object of class stdClass could not be converted to string
(object) array ( ) . true - Error Object of class stdClass could not be converted to string
(object) array ( ) . 0 - Error Object of class stdClass could not be converted to string
(object) array ( ) . 10 - Error Object of class stdClass could not be converted to string
(object) array ( ) . 0.0 - Error Object of class stdClass could not be converted to string
(object) array ( ) . 10.0 - Error Object of class stdClass could not be converted to string
(object) array ( ) . 3.14 - Error Object of class stdClass could not be converted to string
(object) array ( ) . '0' - Error Object of class stdClass could not be converted to string
(object) array ( ) . '10' - Error Object of class stdClass could not be converted to string
(object) array ( ) . '010' - Error Object of class stdClass could not be converted to string
(object) array ( ) . '10 elephants' - Error Object of class stdClass could not be converted to string
(object) array ( ) . 'foo' - Error Object of class stdClass could not be converted to string
(object) array ( ) . array ( ) - Error Object of class stdClass could not be converted to string
(object) array ( ) . array ( 0 => 1 ) - Error Object of class stdClass could not be converted to string
(object) array ( ) . array ( 0 => 1, 1 => 100 ) - Error Object of class stdClass could not be converted to string
(object) array ( ) . array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
(object) array ( ) . array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
(object) array ( ) . (object) array ( ) - Error Object of class stdClass could not be converted to string
(object) array ( ) . (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
(object) array ( ) . (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
(object) array ( ) . DateTime - Error Object of class stdClass could not be converted to string
(object) array ( ) . resource - Error Object of class stdClass could not be converted to string
(object) array ( ) . NULL - Error Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . false - Error Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . true - Error Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . 0 - Error Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . 10 - Error Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . 0.0 - Error Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . 10.0 - Error Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . 3.14 - Error Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . '0' - Error Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . '10' - Error Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . '010' - Error Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . '10 elephants' - Error Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . 'foo' - Error Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . array ( ) - Error Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . array ( 0 => 1 ) - Error Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . array ( 0 => 1, 1 => 100 ) - Error Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . (object) array ( ) - Error Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . DateTime - Error Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . resource - Error Object of class stdClass could not be converted to string
(object) array ( 'foo' => 1, 'bar' => 2 ) . NULL - Error Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . false - Error Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . true - Error Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . 0 - Error Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . 10 - Error Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . 0.0 - Error Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . 10.0 - Error Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . 3.14 - Error Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . '0' - Error Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . '10' - Error Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . '010' - Error Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . '10 elephants' - Error Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . 'foo' - Error Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . array ( ) - Error Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . array ( 0 => 1 ) - Error Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . array ( 0 => 1, 1 => 100 ) - Error Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . (object) array ( ) - Error Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . DateTime - Error Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . resource - Error Object of class stdClass could not be converted to string
(object) array ( 'bar' => 1, 'foo' => 2 ) . NULL - Error Object of class stdClass could not be converted to string
DateTime . false - Error Object of class DateTime could not be converted to string
DateTime . true - Error Object of class DateTime could not be converted to string
DateTime . 0 - Error Object of class DateTime could not be converted to string
DateTime . 10 - Error Object of class DateTime could not be converted to string
DateTime . 0.0 - Error Object of class DateTime could not be converted to string
DateTime . 10.0 - Error Object of class DateTime could not be converted to string
DateTime . 3.14 - Error Object of class DateTime could not be converted to string
DateTime . '0' - Error Object of class DateTime could not be converted to string
DateTime . '10' - Error Object of class DateTime could not be converted to string
DateTime . '010' - Error Object of class DateTime could not be converted to string
DateTime . '10 elephants' - Error Object of class DateTime could not be converted to string
DateTime . 'foo' - Error Object of class DateTime could not be converted to string
DateTime . array ( ) - Error Object of class DateTime could not be converted to string
DateTime . array ( 0 => 1 ) - Error Object of class DateTime could not be converted to string
DateTime . array ( 0 => 1, 1 => 100 ) - Error Object of class DateTime could not be converted to string
DateTime . array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class DateTime could not be converted to string
DateTime . array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class DateTime could not be converted to string
DateTime . (object) array ( ) - Error Object of class DateTime could not be converted to string
DateTime . (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class DateTime could not be converted to string
DateTime . (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class DateTime could not be converted to string
DateTime . DateTime - Error Object of class DateTime could not be converted to string
DateTime . resource - Error Object of class DateTime could not be converted to string
DateTime . NULL - Error Object of class DateTime could not be converted to string
resource . false = 'Resource id #6'
resource . true = 'Resource id #61'
resource . 0 = 'Resource id #60'
resource . 10 = 'Resource id #610'
resource . 0.0 = 'Resource id #60'
resource . 10.0 = 'Resource id #610'
resource . 3.14 = 'Resource id #63.14'
resource . '0' = 'Resource id #60'
resource . '10' = 'Resource id #610'
resource . '010' = 'Resource id #6010'
resource . '10 elephants' = 'Resource id #610 elephants'
resource . 'foo' = 'Resource id #6foo'
resource . array ( ) = 'Resource id #6Array' - Notice Array to string conversion
resource . array ( 0 => 1 ) = 'Resource id #6Array' - Notice Array to string conversion
resource . array ( 0 => 1, 1 => 100 ) = 'Resource id #6Array' - Notice Array to string conversion
resource . array ( 'foo' => 1, 'bar' => 2 ) = 'Resource id #6Array' - Notice Array to string conversion
resource . array ( 'bar' => 1, 'foo' => 2 ) = 'Resource id #6Array' - Notice Array to string conversion
resource . (object) array ( ) - Error Object of class stdClass could not be converted to string
resource . (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
resource . (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
resource . DateTime - Error Object of class DateTime could not be converted to string
resource . resource = 'Resource id #6Resource id #6'
resource . NULL = 'Resource id #6'
NULL . false = ''
NULL . true = '1'
NULL . 0 = '0'
NULL . 10 = '10'
NULL . 0.0 = '0'
NULL . 10.0 = '10'
NULL . 3.14 = '3.14'
NULL . '0' = '0'
NULL . '10' = '10'
NULL . '010' = '010'
NULL . '10 elephants' = '10 elephants'
NULL . 'foo' = 'foo'
NULL . array ( ) = 'Array' - Notice Array to string conversion
NULL . array ( 0 => 1 ) = 'Array' - Notice Array to string conversion
NULL . array ( 0 => 1, 1 => 100 ) = 'Array' - Notice Array to string conversion
NULL . array ( 'foo' => 1, 'bar' => 2 ) = 'Array' - Notice Array to string conversion
NULL . array ( 'bar' => 1, 'foo' => 2 ) = 'Array' - Notice Array to string conversion
NULL . (object) array ( ) - Error Object of class stdClass could not be converted to string
NULL . (object) array ( 'foo' => 1, 'bar' => 2 ) - Error Object of class stdClass could not be converted to string
NULL . (object) array ( 'bar' => 1, 'foo' => 2 ) - Error Object of class stdClass could not be converted to string
NULL . DateTime - Error Object of class DateTime could not be converted to string
NULL . resource = 'Resource id #6'
NULL . NULL = ''