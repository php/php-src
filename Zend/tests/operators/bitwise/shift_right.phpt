--TEST--
bitwise shift right operator
--FILE--
<?php
require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a >> $b', function($a, $b) { return $a >> $b; }, 'var_out_base64');

--EXPECT--
false >> false = 0
false >> true = 0
false >> 0 = 0
false >> 10 = 0
false >> 0.0 = 0
false >> 10.0 = 0
false >> 3.14 = 0
false >> '0' = 0
false >> '10' = 0
false >> '010' = 0
false >> '10 elephants' = 0 - Notice A non well formed numeric value encountered
false >> 'foo' = 0 - Warning A non-numeric value encountered
false >> array ( ) = 0
false >> array ( 0 => 1 ) = 0
false >> array ( 0 => 1, 1 => 100 ) = 0
false >> array ( 'foo' => 1, 'bar' => 2 ) = 0
false >> array ( 'bar' => 1, 'foo' => 2 ) = 0
false >> (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
false >> (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
false >> (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
false >> DateTime = 0 - Notice Object of class DateTime could not be converted to int
false >> resource = 0
false >> NULL = 0
true >> false = 1
true >> true = 0
true >> 0 = 1
true >> 10 = 0
true >> 0.0 = 1
true >> 10.0 = 0
true >> 3.14 = 0
true >> '0' = 1
true >> '10' = 0
true >> '010' = 0
true >> '10 elephants' = 0 - Notice A non well formed numeric value encountered
true >> 'foo' = 1 - Warning A non-numeric value encountered
true >> array ( ) = 1
true >> array ( 0 => 1 ) = 0
true >> array ( 0 => 1, 1 => 100 ) = 0
true >> array ( 'foo' => 1, 'bar' => 2 ) = 0
true >> array ( 'bar' => 1, 'foo' => 2 ) = 0
true >> (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
true >> (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
true >> (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
true >> DateTime = 0 - Notice Object of class DateTime could not be converted to int
true >> resource = 0
true >> NULL = 1
0 >> false = 0
0 >> true = 0
0 >> 0 = 0
0 >> 10 = 0
0 >> 0.0 = 0
0 >> 10.0 = 0
0 >> 3.14 = 0
0 >> '0' = 0
0 >> '10' = 0
0 >> '010' = 0
0 >> '10 elephants' = 0 - Notice A non well formed numeric value encountered
0 >> 'foo' = 0 - Warning A non-numeric value encountered
0 >> array ( ) = 0
0 >> array ( 0 => 1 ) = 0
0 >> array ( 0 => 1, 1 => 100 ) = 0
0 >> array ( 'foo' => 1, 'bar' => 2 ) = 0
0 >> array ( 'bar' => 1, 'foo' => 2 ) = 0
0 >> (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
0 >> (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
0 >> (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
0 >> DateTime = 0 - Notice Object of class DateTime could not be converted to int
0 >> resource = 0
0 >> NULL = 0
10 >> false = 10
10 >> true = 5
10 >> 0 = 10
10 >> 10 = 0
10 >> 0.0 = 10
10 >> 10.0 = 0
10 >> 3.14 = 1
10 >> '0' = 10
10 >> '10' = 0
10 >> '010' = 0
10 >> '10 elephants' = 0 - Notice A non well formed numeric value encountered
10 >> 'foo' = 10 - Warning A non-numeric value encountered
10 >> array ( ) = 10
10 >> array ( 0 => 1 ) = 5
10 >> array ( 0 => 1, 1 => 100 ) = 5
10 >> array ( 'foo' => 1, 'bar' => 2 ) = 5
10 >> array ( 'bar' => 1, 'foo' => 2 ) = 5
10 >> (object) array ( ) = 5 - Notice Object of class stdClass could not be converted to int
10 >> (object) array ( 'foo' => 1, 'bar' => 2 ) = 5 - Notice Object of class stdClass could not be converted to int
10 >> (object) array ( 'bar' => 1, 'foo' => 2 ) = 5 - Notice Object of class stdClass could not be converted to int
10 >> DateTime = 5 - Notice Object of class DateTime could not be converted to int
10 >> resource = 0
10 >> NULL = 10
0.0 >> false = 0
0.0 >> true = 0
0.0 >> 0 = 0
0.0 >> 10 = 0
0.0 >> 0.0 = 0
0.0 >> 10.0 = 0
0.0 >> 3.14 = 0
0.0 >> '0' = 0
0.0 >> '10' = 0
0.0 >> '010' = 0
0.0 >> '10 elephants' = 0 - Notice A non well formed numeric value encountered
0.0 >> 'foo' = 0 - Warning A non-numeric value encountered
0.0 >> array ( ) = 0
0.0 >> array ( 0 => 1 ) = 0
0.0 >> array ( 0 => 1, 1 => 100 ) = 0
0.0 >> array ( 'foo' => 1, 'bar' => 2 ) = 0
0.0 >> array ( 'bar' => 1, 'foo' => 2 ) = 0
0.0 >> (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
0.0 >> (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
0.0 >> (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
0.0 >> DateTime = 0 - Notice Object of class DateTime could not be converted to int
0.0 >> resource = 0
0.0 >> NULL = 0
10.0 >> false = 10
10.0 >> true = 5
10.0 >> 0 = 10
10.0 >> 10 = 0
10.0 >> 0.0 = 10
10.0 >> 10.0 = 0
10.0 >> 3.14 = 1
10.0 >> '0' = 10
10.0 >> '10' = 0
10.0 >> '010' = 0
10.0 >> '10 elephants' = 0 - Notice A non well formed numeric value encountered
10.0 >> 'foo' = 10 - Warning A non-numeric value encountered
10.0 >> array ( ) = 10
10.0 >> array ( 0 => 1 ) = 5
10.0 >> array ( 0 => 1, 1 => 100 ) = 5
10.0 >> array ( 'foo' => 1, 'bar' => 2 ) = 5
10.0 >> array ( 'bar' => 1, 'foo' => 2 ) = 5
10.0 >> (object) array ( ) = 5 - Notice Object of class stdClass could not be converted to int
10.0 >> (object) array ( 'foo' => 1, 'bar' => 2 ) = 5 - Notice Object of class stdClass could not be converted to int
10.0 >> (object) array ( 'bar' => 1, 'foo' => 2 ) = 5 - Notice Object of class stdClass could not be converted to int
10.0 >> DateTime = 5 - Notice Object of class DateTime could not be converted to int
10.0 >> resource = 0
10.0 >> NULL = 10
3.14 >> false = 3
3.14 >> true = 1
3.14 >> 0 = 3
3.14 >> 10 = 0
3.14 >> 0.0 = 3
3.14 >> 10.0 = 0
3.14 >> 3.14 = 0
3.14 >> '0' = 3
3.14 >> '10' = 0
3.14 >> '010' = 0
3.14 >> '10 elephants' = 0 - Notice A non well formed numeric value encountered
3.14 >> 'foo' = 3 - Warning A non-numeric value encountered
3.14 >> array ( ) = 3
3.14 >> array ( 0 => 1 ) = 1
3.14 >> array ( 0 => 1, 1 => 100 ) = 1
3.14 >> array ( 'foo' => 1, 'bar' => 2 ) = 1
3.14 >> array ( 'bar' => 1, 'foo' => 2 ) = 1
3.14 >> (object) array ( ) = 1 - Notice Object of class stdClass could not be converted to int
3.14 >> (object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
3.14 >> (object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
3.14 >> DateTime = 1 - Notice Object of class DateTime could not be converted to int
3.14 >> resource = 0
3.14 >> NULL = 3
'0' >> false = 0
'0' >> true = 0
'0' >> 0 = 0
'0' >> 10 = 0
'0' >> 0.0 = 0
'0' >> 10.0 = 0
'0' >> 3.14 = 0
'0' >> '0' = 0
'0' >> '10' = 0
'0' >> '010' = 0
'0' >> '10 elephants' = 0 - Notice A non well formed numeric value encountered
'0' >> 'foo' = 0 - Warning A non-numeric value encountered
'0' >> array ( ) = 0
'0' >> array ( 0 => 1 ) = 0
'0' >> array ( 0 => 1, 1 => 100 ) = 0
'0' >> array ( 'foo' => 1, 'bar' => 2 ) = 0
'0' >> array ( 'bar' => 1, 'foo' => 2 ) = 0
'0' >> (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
'0' >> (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
'0' >> (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
'0' >> DateTime = 0 - Notice Object of class DateTime could not be converted to int
'0' >> resource = 0
'0' >> NULL = 0
'10' >> false = 10
'10' >> true = 5
'10' >> 0 = 10
'10' >> 10 = 0
'10' >> 0.0 = 10
'10' >> 10.0 = 0
'10' >> 3.14 = 1
'10' >> '0' = 10
'10' >> '10' = 0
'10' >> '010' = 0
'10' >> '10 elephants' = 0 - Notice A non well formed numeric value encountered
'10' >> 'foo' = 10 - Warning A non-numeric value encountered
'10' >> array ( ) = 10
'10' >> array ( 0 => 1 ) = 5
'10' >> array ( 0 => 1, 1 => 100 ) = 5
'10' >> array ( 'foo' => 1, 'bar' => 2 ) = 5
'10' >> array ( 'bar' => 1, 'foo' => 2 ) = 5
'10' >> (object) array ( ) = 5 - Notice Object of class stdClass could not be converted to int
'10' >> (object) array ( 'foo' => 1, 'bar' => 2 ) = 5 - Notice Object of class stdClass could not be converted to int
'10' >> (object) array ( 'bar' => 1, 'foo' => 2 ) = 5 - Notice Object of class stdClass could not be converted to int
'10' >> DateTime = 5 - Notice Object of class DateTime could not be converted to int
'10' >> resource = 0
'10' >> NULL = 10
'010' >> false = 10
'010' >> true = 5
'010' >> 0 = 10
'010' >> 10 = 0
'010' >> 0.0 = 10
'010' >> 10.0 = 0
'010' >> 3.14 = 1
'010' >> '0' = 10
'010' >> '10' = 0
'010' >> '010' = 0
'010' >> '10 elephants' = 0 - Notice A non well formed numeric value encountered
'010' >> 'foo' = 10 - Warning A non-numeric value encountered
'010' >> array ( ) = 10
'010' >> array ( 0 => 1 ) = 5
'010' >> array ( 0 => 1, 1 => 100 ) = 5
'010' >> array ( 'foo' => 1, 'bar' => 2 ) = 5
'010' >> array ( 'bar' => 1, 'foo' => 2 ) = 5
'010' >> (object) array ( ) = 5 - Notice Object of class stdClass could not be converted to int
'010' >> (object) array ( 'foo' => 1, 'bar' => 2 ) = 5 - Notice Object of class stdClass could not be converted to int
'010' >> (object) array ( 'bar' => 1, 'foo' => 2 ) = 5 - Notice Object of class stdClass could not be converted to int
'010' >> DateTime = 5 - Notice Object of class DateTime could not be converted to int
'010' >> resource = 0
'010' >> NULL = 10
'10 elephants' >> false = 10 - Notice A non well formed numeric value encountered
'10 elephants' >> true = 5 - Notice A non well formed numeric value encountered
'10 elephants' >> 0 = 10 - Notice A non well formed numeric value encountered
'10 elephants' >> 10 = 0 - Notice A non well formed numeric value encountered
'10 elephants' >> 0.0 = 10 - Notice A non well formed numeric value encountered
'10 elephants' >> 10.0 = 0 - Notice A non well formed numeric value encountered
'10 elephants' >> 3.14 = 1 - Notice A non well formed numeric value encountered
'10 elephants' >> '0' = 10 - Notice A non well formed numeric value encountered
'10 elephants' >> '10' = 0 - Notice A non well formed numeric value encountered
'10 elephants' >> '010' = 0 - Notice A non well formed numeric value encountered
'10 elephants' >> '10 elephants' = 0 - Notice A non well formed numeric value encountered
'10 elephants' >> 'foo' = 10 - Warning A non-numeric value encountered
'10 elephants' >> array ( ) = 10 - Notice A non well formed numeric value encountered
'10 elephants' >> array ( 0 => 1 ) = 5 - Notice A non well formed numeric value encountered
'10 elephants' >> array ( 0 => 1, 1 => 100 ) = 5 - Notice A non well formed numeric value encountered
'10 elephants' >> array ( 'foo' => 1, 'bar' => 2 ) = 5 - Notice A non well formed numeric value encountered
'10 elephants' >> array ( 'bar' => 1, 'foo' => 2 ) = 5 - Notice A non well formed numeric value encountered
'10 elephants' >> (object) array ( ) = 5 - Notice Object of class stdClass could not be converted to int
'10 elephants' >> (object) array ( 'foo' => 1, 'bar' => 2 ) = 5 - Notice Object of class stdClass could not be converted to int
'10 elephants' >> (object) array ( 'bar' => 1, 'foo' => 2 ) = 5 - Notice Object of class stdClass could not be converted to int
'10 elephants' >> DateTime = 5 - Notice Object of class DateTime could not be converted to int
'10 elephants' >> resource = 0 - Notice A non well formed numeric value encountered
'10 elephants' >> NULL = 10 - Notice A non well formed numeric value encountered
'foo' >> false = 0 - Warning A non-numeric value encountered
'foo' >> true = 0 - Warning A non-numeric value encountered
'foo' >> 0 = 0 - Warning A non-numeric value encountered
'foo' >> 10 = 0 - Warning A non-numeric value encountered
'foo' >> 0.0 = 0 - Warning A non-numeric value encountered
'foo' >> 10.0 = 0 - Warning A non-numeric value encountered
'foo' >> 3.14 = 0 - Warning A non-numeric value encountered
'foo' >> '0' = 0 - Warning A non-numeric value encountered
'foo' >> '10' = 0 - Warning A non-numeric value encountered
'foo' >> '010' = 0 - Warning A non-numeric value encountered
'foo' >> '10 elephants' = 0 - Notice A non well formed numeric value encountered
'foo' >> 'foo' = 0 - Warning A non-numeric value encountered
'foo' >> array ( ) = 0 - Warning A non-numeric value encountered
'foo' >> array ( 0 => 1 ) = 0 - Warning A non-numeric value encountered
'foo' >> array ( 0 => 1, 1 => 100 ) = 0 - Warning A non-numeric value encountered
'foo' >> array ( 'foo' => 1, 'bar' => 2 ) = 0 - Warning A non-numeric value encountered
'foo' >> array ( 'bar' => 1, 'foo' => 2 ) = 0 - Warning A non-numeric value encountered
'foo' >> (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
'foo' >> (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
'foo' >> (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
'foo' >> DateTime = 0 - Notice Object of class DateTime could not be converted to int
'foo' >> resource = 0 - Warning A non-numeric value encountered
'foo' >> NULL = 0 - Warning A non-numeric value encountered
array ( ) >> false = 0
array ( ) >> true = 0
array ( ) >> 0 = 0
array ( ) >> 10 = 0
array ( ) >> 0.0 = 0
array ( ) >> 10.0 = 0
array ( ) >> 3.14 = 0
array ( ) >> '0' = 0
array ( ) >> '10' = 0
array ( ) >> '010' = 0
array ( ) >> '10 elephants' = 0 - Notice A non well formed numeric value encountered
array ( ) >> 'foo' = 0 - Warning A non-numeric value encountered
array ( ) >> array ( ) = 0
array ( ) >> array ( 0 => 1 ) = 0
array ( ) >> array ( 0 => 1, 1 => 100 ) = 0
array ( ) >> array ( 'foo' => 1, 'bar' => 2 ) = 0
array ( ) >> array ( 'bar' => 1, 'foo' => 2 ) = 0
array ( ) >> (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
array ( ) >> (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
array ( ) >> (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
array ( ) >> DateTime = 0 - Notice Object of class DateTime could not be converted to int
array ( ) >> resource = 0
array ( ) >> NULL = 0
array ( 0 => 1 ) >> false = 1
array ( 0 => 1 ) >> true = 0
array ( 0 => 1 ) >> 0 = 1
array ( 0 => 1 ) >> 10 = 0
array ( 0 => 1 ) >> 0.0 = 1
array ( 0 => 1 ) >> 10.0 = 0
array ( 0 => 1 ) >> 3.14 = 0
array ( 0 => 1 ) >> '0' = 1
array ( 0 => 1 ) >> '10' = 0
array ( 0 => 1 ) >> '010' = 0
array ( 0 => 1 ) >> '10 elephants' = 0 - Notice A non well formed numeric value encountered
array ( 0 => 1 ) >> 'foo' = 1 - Warning A non-numeric value encountered
array ( 0 => 1 ) >> array ( ) = 1
array ( 0 => 1 ) >> array ( 0 => 1 ) = 0
array ( 0 => 1 ) >> array ( 0 => 1, 1 => 100 ) = 0
array ( 0 => 1 ) >> array ( 'foo' => 1, 'bar' => 2 ) = 0
array ( 0 => 1 ) >> array ( 'bar' => 1, 'foo' => 2 ) = 0
array ( 0 => 1 ) >> (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
array ( 0 => 1 ) >> (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
array ( 0 => 1 ) >> (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
array ( 0 => 1 ) >> DateTime = 0 - Notice Object of class DateTime could not be converted to int
array ( 0 => 1 ) >> resource = 0
array ( 0 => 1 ) >> NULL = 1
array ( 0 => 1, 1 => 100 ) >> false = 1
array ( 0 => 1, 1 => 100 ) >> true = 0
array ( 0 => 1, 1 => 100 ) >> 0 = 1
array ( 0 => 1, 1 => 100 ) >> 10 = 0
array ( 0 => 1, 1 => 100 ) >> 0.0 = 1
array ( 0 => 1, 1 => 100 ) >> 10.0 = 0
array ( 0 => 1, 1 => 100 ) >> 3.14 = 0
array ( 0 => 1, 1 => 100 ) >> '0' = 1
array ( 0 => 1, 1 => 100 ) >> '10' = 0
array ( 0 => 1, 1 => 100 ) >> '010' = 0
array ( 0 => 1, 1 => 100 ) >> '10 elephants' = 0 - Notice A non well formed numeric value encountered
array ( 0 => 1, 1 => 100 ) >> 'foo' = 1 - Warning A non-numeric value encountered
array ( 0 => 1, 1 => 100 ) >> array ( ) = 1
array ( 0 => 1, 1 => 100 ) >> array ( 0 => 1 ) = 0
array ( 0 => 1, 1 => 100 ) >> array ( 0 => 1, 1 => 100 ) = 0
array ( 0 => 1, 1 => 100 ) >> array ( 'foo' => 1, 'bar' => 2 ) = 0
array ( 0 => 1, 1 => 100 ) >> array ( 'bar' => 1, 'foo' => 2 ) = 0
array ( 0 => 1, 1 => 100 ) >> (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
array ( 0 => 1, 1 => 100 ) >> (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
array ( 0 => 1, 1 => 100 ) >> (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
array ( 0 => 1, 1 => 100 ) >> DateTime = 0 - Notice Object of class DateTime could not be converted to int
array ( 0 => 1, 1 => 100 ) >> resource = 0
array ( 0 => 1, 1 => 100 ) >> NULL = 1
array ( 'foo' => 1, 'bar' => 2 ) >> false = 1
array ( 'foo' => 1, 'bar' => 2 ) >> true = 0
array ( 'foo' => 1, 'bar' => 2 ) >> 0 = 1
array ( 'foo' => 1, 'bar' => 2 ) >> 10 = 0
array ( 'foo' => 1, 'bar' => 2 ) >> 0.0 = 1
array ( 'foo' => 1, 'bar' => 2 ) >> 10.0 = 0
array ( 'foo' => 1, 'bar' => 2 ) >> 3.14 = 0
array ( 'foo' => 1, 'bar' => 2 ) >> '0' = 1
array ( 'foo' => 1, 'bar' => 2 ) >> '10' = 0
array ( 'foo' => 1, 'bar' => 2 ) >> '010' = 0
array ( 'foo' => 1, 'bar' => 2 ) >> '10 elephants' = 0 - Notice A non well formed numeric value encountered
array ( 'foo' => 1, 'bar' => 2 ) >> 'foo' = 1 - Warning A non-numeric value encountered
array ( 'foo' => 1, 'bar' => 2 ) >> array ( ) = 1
array ( 'foo' => 1, 'bar' => 2 ) >> array ( 0 => 1 ) = 0
array ( 'foo' => 1, 'bar' => 2 ) >> array ( 0 => 1, 1 => 100 ) = 0
array ( 'foo' => 1, 'bar' => 2 ) >> array ( 'foo' => 1, 'bar' => 2 ) = 0
array ( 'foo' => 1, 'bar' => 2 ) >> array ( 'bar' => 1, 'foo' => 2 ) = 0
array ( 'foo' => 1, 'bar' => 2 ) >> (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
array ( 'foo' => 1, 'bar' => 2 ) >> (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
array ( 'foo' => 1, 'bar' => 2 ) >> (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
array ( 'foo' => 1, 'bar' => 2 ) >> DateTime = 0 - Notice Object of class DateTime could not be converted to int
array ( 'foo' => 1, 'bar' => 2 ) >> resource = 0
array ( 'foo' => 1, 'bar' => 2 ) >> NULL = 1
array ( 'bar' => 1, 'foo' => 2 ) >> false = 1
array ( 'bar' => 1, 'foo' => 2 ) >> true = 0
array ( 'bar' => 1, 'foo' => 2 ) >> 0 = 1
array ( 'bar' => 1, 'foo' => 2 ) >> 10 = 0
array ( 'bar' => 1, 'foo' => 2 ) >> 0.0 = 1
array ( 'bar' => 1, 'foo' => 2 ) >> 10.0 = 0
array ( 'bar' => 1, 'foo' => 2 ) >> 3.14 = 0
array ( 'bar' => 1, 'foo' => 2 ) >> '0' = 1
array ( 'bar' => 1, 'foo' => 2 ) >> '10' = 0
array ( 'bar' => 1, 'foo' => 2 ) >> '010' = 0
array ( 'bar' => 1, 'foo' => 2 ) >> '10 elephants' = 0 - Notice A non well formed numeric value encountered
array ( 'bar' => 1, 'foo' => 2 ) >> 'foo' = 1 - Warning A non-numeric value encountered
array ( 'bar' => 1, 'foo' => 2 ) >> array ( ) = 1
array ( 'bar' => 1, 'foo' => 2 ) >> array ( 0 => 1 ) = 0
array ( 'bar' => 1, 'foo' => 2 ) >> array ( 0 => 1, 1 => 100 ) = 0
array ( 'bar' => 1, 'foo' => 2 ) >> array ( 'foo' => 1, 'bar' => 2 ) = 0
array ( 'bar' => 1, 'foo' => 2 ) >> array ( 'bar' => 1, 'foo' => 2 ) = 0
array ( 'bar' => 1, 'foo' => 2 ) >> (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
array ( 'bar' => 1, 'foo' => 2 ) >> (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
array ( 'bar' => 1, 'foo' => 2 ) >> (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
array ( 'bar' => 1, 'foo' => 2 ) >> DateTime = 0 - Notice Object of class DateTime could not be converted to int
array ( 'bar' => 1, 'foo' => 2 ) >> resource = 0
array ( 'bar' => 1, 'foo' => 2 ) >> NULL = 1
(object) array ( ) >> false = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( ) >> true = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) >> 0 = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( ) >> 10 = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) >> 0.0 = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( ) >> 10.0 = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) >> 3.14 = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) >> '0' = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( ) >> '10' = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) >> '010' = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) >> '10 elephants' = 0 - Notice A non well formed numeric value encountered
(object) array ( ) >> 'foo' = 1 - Warning A non-numeric value encountered
(object) array ( ) >> array ( ) = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( ) >> array ( 0 => 1 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) >> array ( 0 => 1, 1 => 100 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) >> array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) >> array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) >> (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) >> (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) >> (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) >> DateTime = 0 - Notice Object of class DateTime could not be converted to int
(object) array ( ) >> resource = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( ) >> NULL = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) >> false = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) >> true = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) >> 0 = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) >> 10 = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) >> 0.0 = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) >> 10.0 = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) >> 3.14 = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) >> '0' = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) >> '10' = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) >> '010' = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) >> '10 elephants' = 0 - Notice A non well formed numeric value encountered
(object) array ( 'foo' => 1, 'bar' => 2 ) >> 'foo' = 1 - Warning A non-numeric value encountered
(object) array ( 'foo' => 1, 'bar' => 2 ) >> array ( ) = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) >> array ( 0 => 1 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) >> array ( 0 => 1, 1 => 100 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) >> array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) >> array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) >> (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) >> (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) >> (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) >> DateTime = 0 - Notice Object of class DateTime could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) >> resource = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) >> NULL = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) >> false = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) >> true = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) >> 0 = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) >> 10 = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) >> 0.0 = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) >> 10.0 = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) >> 3.14 = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) >> '0' = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) >> '10' = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) >> '010' = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) >> '10 elephants' = 0 - Notice A non well formed numeric value encountered
(object) array ( 'bar' => 1, 'foo' => 2 ) >> 'foo' = 1 - Warning A non-numeric value encountered
(object) array ( 'bar' => 1, 'foo' => 2 ) >> array ( ) = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) >> array ( 0 => 1 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) >> array ( 0 => 1, 1 => 100 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) >> array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) >> array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) >> (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) >> (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) >> (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) >> DateTime = 0 - Notice Object of class DateTime could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) >> resource = 0 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) >> NULL = 1 - Notice Object of class stdClass could not be converted to int
DateTime >> false = 1 - Notice Object of class DateTime could not be converted to int
DateTime >> true = 0 - Notice Object of class DateTime could not be converted to int
DateTime >> 0 = 1 - Notice Object of class DateTime could not be converted to int
DateTime >> 10 = 0 - Notice Object of class DateTime could not be converted to int
DateTime >> 0.0 = 1 - Notice Object of class DateTime could not be converted to int
DateTime >> 10.0 = 0 - Notice Object of class DateTime could not be converted to int
DateTime >> 3.14 = 0 - Notice Object of class DateTime could not be converted to int
DateTime >> '0' = 1 - Notice Object of class DateTime could not be converted to int
DateTime >> '10' = 0 - Notice Object of class DateTime could not be converted to int
DateTime >> '010' = 0 - Notice Object of class DateTime could not be converted to int
DateTime >> '10 elephants' = 0 - Notice A non well formed numeric value encountered
DateTime >> 'foo' = 1 - Warning A non-numeric value encountered
DateTime >> array ( ) = 1 - Notice Object of class DateTime could not be converted to int
DateTime >> array ( 0 => 1 ) = 0 - Notice Object of class DateTime could not be converted to int
DateTime >> array ( 0 => 1, 1 => 100 ) = 0 - Notice Object of class DateTime could not be converted to int
DateTime >> array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class DateTime could not be converted to int
DateTime >> array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class DateTime could not be converted to int
DateTime >> (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
DateTime >> (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
DateTime >> (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
DateTime >> DateTime = 0 - Notice Object of class DateTime could not be converted to int
DateTime >> resource = 0 - Notice Object of class DateTime could not be converted to int
DateTime >> NULL = 1 - Notice Object of class DateTime could not be converted to int
resource >> false = 6
resource >> true = 3
resource >> 0 = 6
resource >> 10 = 0
resource >> 0.0 = 6
resource >> 10.0 = 0
resource >> 3.14 = 0
resource >> '0' = 6
resource >> '10' = 0
resource >> '010' = 0
resource >> '10 elephants' = 0 - Notice A non well formed numeric value encountered
resource >> 'foo' = 6 - Warning A non-numeric value encountered
resource >> array ( ) = 6
resource >> array ( 0 => 1 ) = 3
resource >> array ( 0 => 1, 1 => 100 ) = 3
resource >> array ( 'foo' => 1, 'bar' => 2 ) = 3
resource >> array ( 'bar' => 1, 'foo' => 2 ) = 3
resource >> (object) array ( ) = 3 - Notice Object of class stdClass could not be converted to int
resource >> (object) array ( 'foo' => 1, 'bar' => 2 ) = 3 - Notice Object of class stdClass could not be converted to int
resource >> (object) array ( 'bar' => 1, 'foo' => 2 ) = 3 - Notice Object of class stdClass could not be converted to int
resource >> DateTime = 3 - Notice Object of class DateTime could not be converted to int
resource >> resource = 0
resource >> NULL = 6
NULL >> false = 0
NULL >> true = 0
NULL >> 0 = 0
NULL >> 10 = 0
NULL >> 0.0 = 0
NULL >> 10.0 = 0
NULL >> 3.14 = 0
NULL >> '0' = 0
NULL >> '10' = 0
NULL >> '010' = 0
NULL >> '10 elephants' = 0 - Notice A non well formed numeric value encountered
NULL >> 'foo' = 0 - Warning A non-numeric value encountered
NULL >> array ( ) = 0
NULL >> array ( 0 => 1 ) = 0
NULL >> array ( 0 => 1, 1 => 100 ) = 0
NULL >> array ( 'foo' => 1, 'bar' => 2 ) = 0
NULL >> array ( 'bar' => 1, 'foo' => 2 ) = 0
NULL >> (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to int
NULL >> (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
NULL >> (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to int
NULL >> DateTime = 0 - Notice Object of class DateTime could not be converted to int
NULL >> resource = 0
NULL >> NULL = 0