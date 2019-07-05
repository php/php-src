--TEST--
exponentiation '**' operator
--FILE--
<?php
require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a ** $b', function($a, $b) { return $a ** $b; });

--EXPECT--
false ** false = 1
false ** true = 0
false ** 0 = 1
false ** 10 = 0
false ** 0.0 = 1.0
false ** 10.0 = 0.0
false ** 3.14 = 0.0
false ** '0' = 1
false ** '10' = 0
false ** '010' = 0
false ** '10 elephants' = 0 - Notice A non well formed numeric value encountered
false ** 'foo' = 1 - Warning A non-numeric value encountered
false ** array ( ) = 1
false ** array ( 0 => 1 ) = 1
false ** array ( 0 => 1, 1 => 100 ) = 1
false ** array ( 'foo' => 1, 'bar' => 2 ) = 1
false ** array ( 'bar' => 1, 'foo' => 2 ) = 1
false ** (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to number
false ** (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to number
false ** (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to number
false ** DateTime = 0 - Notice Object of class DateTime could not be converted to number
false ** resource = 0
false ** NULL = 1
true ** false = 1
true ** true = 1
true ** 0 = 1
true ** 10 = 1
true ** 0.0 = 1.0
true ** 10.0 = 1.0
true ** 3.14 = 1.0
true ** '0' = 1
true ** '10' = 1
true ** '010' = 1
true ** '10 elephants' = 1 - Notice A non well formed numeric value encountered
true ** 'foo' = 1 - Warning A non-numeric value encountered
true ** array ( ) = 1
true ** array ( 0 => 1 ) = 1
true ** array ( 0 => 1, 1 => 100 ) = 1
true ** array ( 'foo' => 1, 'bar' => 2 ) = 1
true ** array ( 'bar' => 1, 'foo' => 2 ) = 1
true ** (object) array ( ) = 1 - Notice Object of class stdClass could not be converted to number
true ** (object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
true ** (object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
true ** DateTime = 1 - Notice Object of class DateTime could not be converted to number
true ** resource = 1
true ** NULL = 1
0 ** false = 1
0 ** true = 0
0 ** 0 = 1
0 ** 10 = 0
0 ** 0.0 = 1.0
0 ** 10.0 = 0.0
0 ** 3.14 = 0.0
0 ** '0' = 1
0 ** '10' = 0
0 ** '010' = 0
0 ** '10 elephants' = 0 - Notice A non well formed numeric value encountered
0 ** 'foo' = 1 - Warning A non-numeric value encountered
0 ** array ( ) = 1
0 ** array ( 0 => 1 ) = 1
0 ** array ( 0 => 1, 1 => 100 ) = 1
0 ** array ( 'foo' => 1, 'bar' => 2 ) = 1
0 ** array ( 'bar' => 1, 'foo' => 2 ) = 1
0 ** (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to number
0 ** (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to number
0 ** (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to number
0 ** DateTime = 0 - Notice Object of class DateTime could not be converted to number
0 ** resource = 0
0 ** NULL = 1
10 ** false = 1
10 ** true = 10
10 ** 0 = 1
10 ** 10 = 10000000000
10 ** 0.0 = 1.0
10 ** 10.0 = 10000000000.0
10 ** 3.14 = 1380.3842646028852
10 ** '0' = 1
10 ** '10' = 10000000000
10 ** '010' = 10000000000
10 ** '10 elephants' = 10000000000 - Notice A non well formed numeric value encountered
10 ** 'foo' = 1 - Warning A non-numeric value encountered
10 ** array ( ) = 1
10 ** array ( 0 => 1 ) = 1
10 ** array ( 0 => 1, 1 => 100 ) = 1
10 ** array ( 'foo' => 1, 'bar' => 2 ) = 1
10 ** array ( 'bar' => 1, 'foo' => 2 ) = 1
10 ** (object) array ( ) = 10 - Notice Object of class stdClass could not be converted to number
10 ** (object) array ( 'foo' => 1, 'bar' => 2 ) = 10 - Notice Object of class stdClass could not be converted to number
10 ** (object) array ( 'bar' => 1, 'foo' => 2 ) = 10 - Notice Object of class stdClass could not be converted to number
10 ** DateTime = 10 - Notice Object of class DateTime could not be converted to number
10 ** resource = 1000000
10 ** NULL = 1
0.0 ** false = 1.0
0.0 ** true = 0.0
0.0 ** 0 = 1.0
0.0 ** 10 = 0.0
0.0 ** 0.0 = 1.0
0.0 ** 10.0 = 0.0
0.0 ** 3.14 = 0.0
0.0 ** '0' = 1.0
0.0 ** '10' = 0.0
0.0 ** '010' = 0.0
0.0 ** '10 elephants' = 0.0 - Notice A non well formed numeric value encountered
0.0 ** 'foo' = 1.0 - Warning A non-numeric value encountered
0.0 ** array ( ) = 1
0.0 ** array ( 0 => 1 ) = 1
0.0 ** array ( 0 => 1, 1 => 100 ) = 1
0.0 ** array ( 'foo' => 1, 'bar' => 2 ) = 1
0.0 ** array ( 'bar' => 1, 'foo' => 2 ) = 1
0.0 ** (object) array ( ) = 0.0 - Notice Object of class stdClass could not be converted to number
0.0 ** (object) array ( 'foo' => 1, 'bar' => 2 ) = 0.0 - Notice Object of class stdClass could not be converted to number
0.0 ** (object) array ( 'bar' => 1, 'foo' => 2 ) = 0.0 - Notice Object of class stdClass could not be converted to number
0.0 ** DateTime = 0.0 - Notice Object of class DateTime could not be converted to number
0.0 ** resource = 0.0
0.0 ** NULL = 1.0
10.0 ** false = 1.0
10.0 ** true = 10.0
10.0 ** 0 = 1.0
10.0 ** 10 = 10000000000.0
10.0 ** 0.0 = 1.0
10.0 ** 10.0 = 10000000000.0
10.0 ** 3.14 = 1380.3842646028852
10.0 ** '0' = 1.0
10.0 ** '10' = 10000000000.0
10.0 ** '010' = 10000000000.0
10.0 ** '10 elephants' = 10000000000.0 - Notice A non well formed numeric value encountered
10.0 ** 'foo' = 1.0 - Warning A non-numeric value encountered
10.0 ** array ( ) = 1
10.0 ** array ( 0 => 1 ) = 1
10.0 ** array ( 0 => 1, 1 => 100 ) = 1
10.0 ** array ( 'foo' => 1, 'bar' => 2 ) = 1
10.0 ** array ( 'bar' => 1, 'foo' => 2 ) = 1
10.0 ** (object) array ( ) = 10.0 - Notice Object of class stdClass could not be converted to number
10.0 ** (object) array ( 'foo' => 1, 'bar' => 2 ) = 10.0 - Notice Object of class stdClass could not be converted to number
10.0 ** (object) array ( 'bar' => 1, 'foo' => 2 ) = 10.0 - Notice Object of class stdClass could not be converted to number
10.0 ** DateTime = 10.0 - Notice Object of class DateTime could not be converted to number
10.0 ** resource = 1000000.0
10.0 ** NULL = 1.0
3.14 ** false = 1.0
3.14 ** true = 3.14
3.14 ** 0 = 1.0
3.14 ** 10 = 93174.3733866435
3.14 ** 0.0 = 1.0
3.14 ** 10.0 = 93174.3733866435
3.14 ** 3.14 = 36.33783888017471
3.14 ** '0' = 1.0
3.14 ** '10' = 93174.3733866435
3.14 ** '010' = 93174.3733866435
3.14 ** '10 elephants' = 93174.3733866435 - Notice A non well formed numeric value encountered
3.14 ** 'foo' = 1.0 - Warning A non-numeric value encountered
3.14 ** array ( ) = 1
3.14 ** array ( 0 => 1 ) = 1
3.14 ** array ( 0 => 1, 1 => 100 ) = 1
3.14 ** array ( 'foo' => 1, 'bar' => 2 ) = 1
3.14 ** array ( 'bar' => 1, 'foo' => 2 ) = 1
3.14 ** (object) array ( ) = 3.14 - Notice Object of class stdClass could not be converted to number
3.14 ** (object) array ( 'foo' => 1, 'bar' => 2 ) = 3.14 - Notice Object of class stdClass could not be converted to number
3.14 ** (object) array ( 'bar' => 1, 'foo' => 2 ) = 3.14 - Notice Object of class stdClass could not be converted to number
3.14 ** DateTime = 3.14 - Notice Object of class DateTime could not be converted to number
3.14 ** resource = 958.4685972127362
3.14 ** NULL = 1.0
'0' ** false = 1
'0' ** true = 0
'0' ** 0 = 1
'0' ** 10 = 0
'0' ** 0.0 = 1.0
'0' ** 10.0 = 0.0
'0' ** 3.14 = 0.0
'0' ** '0' = 1
'0' ** '10' = 0
'0' ** '010' = 0
'0' ** '10 elephants' = 0 - Notice A non well formed numeric value encountered
'0' ** 'foo' = 1 - Warning A non-numeric value encountered
'0' ** array ( ) = 1
'0' ** array ( 0 => 1 ) = 1
'0' ** array ( 0 => 1, 1 => 100 ) = 1
'0' ** array ( 'foo' => 1, 'bar' => 2 ) = 1
'0' ** array ( 'bar' => 1, 'foo' => 2 ) = 1
'0' ** (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to number
'0' ** (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to number
'0' ** (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to number
'0' ** DateTime = 0 - Notice Object of class DateTime could not be converted to number
'0' ** resource = 0
'0' ** NULL = 1
'10' ** false = 1
'10' ** true = 10
'10' ** 0 = 1
'10' ** 10 = 10000000000
'10' ** 0.0 = 1.0
'10' ** 10.0 = 10000000000.0
'10' ** 3.14 = 1380.3842646028852
'10' ** '0' = 1
'10' ** '10' = 10000000000
'10' ** '010' = 10000000000
'10' ** '10 elephants' = 10000000000 - Notice A non well formed numeric value encountered
'10' ** 'foo' = 1 - Warning A non-numeric value encountered
'10' ** array ( ) = 1
'10' ** array ( 0 => 1 ) = 1
'10' ** array ( 0 => 1, 1 => 100 ) = 1
'10' ** array ( 'foo' => 1, 'bar' => 2 ) = 1
'10' ** array ( 'bar' => 1, 'foo' => 2 ) = 1
'10' ** (object) array ( ) = 10 - Notice Object of class stdClass could not be converted to number
'10' ** (object) array ( 'foo' => 1, 'bar' => 2 ) = 10 - Notice Object of class stdClass could not be converted to number
'10' ** (object) array ( 'bar' => 1, 'foo' => 2 ) = 10 - Notice Object of class stdClass could not be converted to number
'10' ** DateTime = 10 - Notice Object of class DateTime could not be converted to number
'10' ** resource = 1000000
'10' ** NULL = 1
'010' ** false = 1
'010' ** true = 10
'010' ** 0 = 1
'010' ** 10 = 10000000000
'010' ** 0.0 = 1.0
'010' ** 10.0 = 10000000000.0
'010' ** 3.14 = 1380.3842646028852
'010' ** '0' = 1
'010' ** '10' = 10000000000
'010' ** '010' = 10000000000
'010' ** '10 elephants' = 10000000000 - Notice A non well formed numeric value encountered
'010' ** 'foo' = 1 - Warning A non-numeric value encountered
'010' ** array ( ) = 1
'010' ** array ( 0 => 1 ) = 1
'010' ** array ( 0 => 1, 1 => 100 ) = 1
'010' ** array ( 'foo' => 1, 'bar' => 2 ) = 1
'010' ** array ( 'bar' => 1, 'foo' => 2 ) = 1
'010' ** (object) array ( ) = 10 - Notice Object of class stdClass could not be converted to number
'010' ** (object) array ( 'foo' => 1, 'bar' => 2 ) = 10 - Notice Object of class stdClass could not be converted to number
'010' ** (object) array ( 'bar' => 1, 'foo' => 2 ) = 10 - Notice Object of class stdClass could not be converted to number
'010' ** DateTime = 10 - Notice Object of class DateTime could not be converted to number
'010' ** resource = 1000000
'010' ** NULL = 1
'10 elephants' ** false = 1 - Notice A non well formed numeric value encountered
'10 elephants' ** true = 10 - Notice A non well formed numeric value encountered
'10 elephants' ** 0 = 1 - Notice A non well formed numeric value encountered
'10 elephants' ** 10 = 10000000000 - Notice A non well formed numeric value encountered
'10 elephants' ** 0.0 = 1.0 - Notice A non well formed numeric value encountered
'10 elephants' ** 10.0 = 10000000000.0 - Notice A non well formed numeric value encountered
'10 elephants' ** 3.14 = 1380.3842646028852 - Notice A non well formed numeric value encountered
'10 elephants' ** '0' = 1 - Notice A non well formed numeric value encountered
'10 elephants' ** '10' = 10000000000 - Notice A non well formed numeric value encountered
'10 elephants' ** '010' = 10000000000 - Notice A non well formed numeric value encountered
'10 elephants' ** '10 elephants' = 10000000000 - Notice A non well formed numeric value encountered
'10 elephants' ** 'foo' = 1 - Warning A non-numeric value encountered
'10 elephants' ** array ( ) = 1 - Notice A non well formed numeric value encountered
'10 elephants' ** array ( 0 => 1 ) = 1 - Notice A non well formed numeric value encountered
'10 elephants' ** array ( 0 => 1, 1 => 100 ) = 1 - Notice A non well formed numeric value encountered
'10 elephants' ** array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice A non well formed numeric value encountered
'10 elephants' ** array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice A non well formed numeric value encountered
'10 elephants' ** (object) array ( ) = 10 - Notice Object of class stdClass could not be converted to number
'10 elephants' ** (object) array ( 'foo' => 1, 'bar' => 2 ) = 10 - Notice Object of class stdClass could not be converted to number
'10 elephants' ** (object) array ( 'bar' => 1, 'foo' => 2 ) = 10 - Notice Object of class stdClass could not be converted to number
'10 elephants' ** DateTime = 10 - Notice Object of class DateTime could not be converted to number
'10 elephants' ** resource = 1000000 - Notice A non well formed numeric value encountered
'10 elephants' ** NULL = 1 - Notice A non well formed numeric value encountered
'foo' ** false = 1 - Warning A non-numeric value encountered
'foo' ** true = 0 - Warning A non-numeric value encountered
'foo' ** 0 = 1 - Warning A non-numeric value encountered
'foo' ** 10 = 0 - Warning A non-numeric value encountered
'foo' ** 0.0 = 1.0 - Warning A non-numeric value encountered
'foo' ** 10.0 = 0.0 - Warning A non-numeric value encountered
'foo' ** 3.14 = 0.0 - Warning A non-numeric value encountered
'foo' ** '0' = 1 - Warning A non-numeric value encountered
'foo' ** '10' = 0 - Warning A non-numeric value encountered
'foo' ** '010' = 0 - Warning A non-numeric value encountered
'foo' ** '10 elephants' = 0 - Notice A non well formed numeric value encountered
'foo' ** 'foo' = 1 - Warning A non-numeric value encountered
'foo' ** array ( ) = 1 - Warning A non-numeric value encountered
'foo' ** array ( 0 => 1 ) = 1 - Warning A non-numeric value encountered
'foo' ** array ( 0 => 1, 1 => 100 ) = 1 - Warning A non-numeric value encountered
'foo' ** array ( 'foo' => 1, 'bar' => 2 ) = 1 - Warning A non-numeric value encountered
'foo' ** array ( 'bar' => 1, 'foo' => 2 ) = 1 - Warning A non-numeric value encountered
'foo' ** (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to number
'foo' ** (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to number
'foo' ** (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to number
'foo' ** DateTime = 0 - Notice Object of class DateTime could not be converted to number
'foo' ** resource = 0 - Warning A non-numeric value encountered
'foo' ** NULL = 1 - Warning A non-numeric value encountered
array ( ) ** false = 0
array ( ) ** true = 0
array ( ) ** 0 = 0
array ( ) ** 10 = 0
array ( ) ** 0.0 = 0
array ( ) ** 10.0 = 0
array ( ) ** 3.14 = 0
array ( ) ** '0' = 0
array ( ) ** '10' = 0
array ( ) ** '010' = 0
array ( ) ** '10 elephants' = 0
array ( ) ** 'foo' = 0
array ( ) ** array ( ) = 0
array ( ) ** array ( 0 => 1 ) = 0
array ( ) ** array ( 0 => 1, 1 => 100 ) = 0
array ( ) ** array ( 'foo' => 1, 'bar' => 2 ) = 0
array ( ) ** array ( 'bar' => 1, 'foo' => 2 ) = 0
array ( ) ** (object) array ( ) = 0
array ( ) ** (object) array ( 'foo' => 1, 'bar' => 2 ) = 0
array ( ) ** (object) array ( 'bar' => 1, 'foo' => 2 ) = 0
array ( ) ** DateTime = 0
array ( ) ** resource = 0
array ( ) ** NULL = 0
array ( 0 => 1 ) ** false = 0
array ( 0 => 1 ) ** true = 0
array ( 0 => 1 ) ** 0 = 0
array ( 0 => 1 ) ** 10 = 0
array ( 0 => 1 ) ** 0.0 = 0
array ( 0 => 1 ) ** 10.0 = 0
array ( 0 => 1 ) ** 3.14 = 0
array ( 0 => 1 ) ** '0' = 0
array ( 0 => 1 ) ** '10' = 0
array ( 0 => 1 ) ** '010' = 0
array ( 0 => 1 ) ** '10 elephants' = 0
array ( 0 => 1 ) ** 'foo' = 0
array ( 0 => 1 ) ** array ( ) = 0
array ( 0 => 1 ) ** array ( 0 => 1 ) = 0
array ( 0 => 1 ) ** array ( 0 => 1, 1 => 100 ) = 0
array ( 0 => 1 ) ** array ( 'foo' => 1, 'bar' => 2 ) = 0
array ( 0 => 1 ) ** array ( 'bar' => 1, 'foo' => 2 ) = 0
array ( 0 => 1 ) ** (object) array ( ) = 0
array ( 0 => 1 ) ** (object) array ( 'foo' => 1, 'bar' => 2 ) = 0
array ( 0 => 1 ) ** (object) array ( 'bar' => 1, 'foo' => 2 ) = 0
array ( 0 => 1 ) ** DateTime = 0
array ( 0 => 1 ) ** resource = 0
array ( 0 => 1 ) ** NULL = 0
array ( 0 => 1, 1 => 100 ) ** false = 0
array ( 0 => 1, 1 => 100 ) ** true = 0
array ( 0 => 1, 1 => 100 ) ** 0 = 0
array ( 0 => 1, 1 => 100 ) ** 10 = 0
array ( 0 => 1, 1 => 100 ) ** 0.0 = 0
array ( 0 => 1, 1 => 100 ) ** 10.0 = 0
array ( 0 => 1, 1 => 100 ) ** 3.14 = 0
array ( 0 => 1, 1 => 100 ) ** '0' = 0
array ( 0 => 1, 1 => 100 ) ** '10' = 0
array ( 0 => 1, 1 => 100 ) ** '010' = 0
array ( 0 => 1, 1 => 100 ) ** '10 elephants' = 0
array ( 0 => 1, 1 => 100 ) ** 'foo' = 0
array ( 0 => 1, 1 => 100 ) ** array ( ) = 0
array ( 0 => 1, 1 => 100 ) ** array ( 0 => 1 ) = 0
array ( 0 => 1, 1 => 100 ) ** array ( 0 => 1, 1 => 100 ) = 0
array ( 0 => 1, 1 => 100 ) ** array ( 'foo' => 1, 'bar' => 2 ) = 0
array ( 0 => 1, 1 => 100 ) ** array ( 'bar' => 1, 'foo' => 2 ) = 0
array ( 0 => 1, 1 => 100 ) ** (object) array ( ) = 0
array ( 0 => 1, 1 => 100 ) ** (object) array ( 'foo' => 1, 'bar' => 2 ) = 0
array ( 0 => 1, 1 => 100 ) ** (object) array ( 'bar' => 1, 'foo' => 2 ) = 0
array ( 0 => 1, 1 => 100 ) ** DateTime = 0
array ( 0 => 1, 1 => 100 ) ** resource = 0
array ( 0 => 1, 1 => 100 ) ** NULL = 0
array ( 'foo' => 1, 'bar' => 2 ) ** false = 0
array ( 'foo' => 1, 'bar' => 2 ) ** true = 0
array ( 'foo' => 1, 'bar' => 2 ) ** 0 = 0
array ( 'foo' => 1, 'bar' => 2 ) ** 10 = 0
array ( 'foo' => 1, 'bar' => 2 ) ** 0.0 = 0
array ( 'foo' => 1, 'bar' => 2 ) ** 10.0 = 0
array ( 'foo' => 1, 'bar' => 2 ) ** 3.14 = 0
array ( 'foo' => 1, 'bar' => 2 ) ** '0' = 0
array ( 'foo' => 1, 'bar' => 2 ) ** '10' = 0
array ( 'foo' => 1, 'bar' => 2 ) ** '010' = 0
array ( 'foo' => 1, 'bar' => 2 ) ** '10 elephants' = 0
array ( 'foo' => 1, 'bar' => 2 ) ** 'foo' = 0
array ( 'foo' => 1, 'bar' => 2 ) ** array ( ) = 0
array ( 'foo' => 1, 'bar' => 2 ) ** array ( 0 => 1 ) = 0
array ( 'foo' => 1, 'bar' => 2 ) ** array ( 0 => 1, 1 => 100 ) = 0
array ( 'foo' => 1, 'bar' => 2 ) ** array ( 'foo' => 1, 'bar' => 2 ) = 0
array ( 'foo' => 1, 'bar' => 2 ) ** array ( 'bar' => 1, 'foo' => 2 ) = 0
array ( 'foo' => 1, 'bar' => 2 ) ** (object) array ( ) = 0
array ( 'foo' => 1, 'bar' => 2 ) ** (object) array ( 'foo' => 1, 'bar' => 2 ) = 0
array ( 'foo' => 1, 'bar' => 2 ) ** (object) array ( 'bar' => 1, 'foo' => 2 ) = 0
array ( 'foo' => 1, 'bar' => 2 ) ** DateTime = 0
array ( 'foo' => 1, 'bar' => 2 ) ** resource = 0
array ( 'foo' => 1, 'bar' => 2 ) ** NULL = 0
array ( 'bar' => 1, 'foo' => 2 ) ** false = 0
array ( 'bar' => 1, 'foo' => 2 ) ** true = 0
array ( 'bar' => 1, 'foo' => 2 ) ** 0 = 0
array ( 'bar' => 1, 'foo' => 2 ) ** 10 = 0
array ( 'bar' => 1, 'foo' => 2 ) ** 0.0 = 0
array ( 'bar' => 1, 'foo' => 2 ) ** 10.0 = 0
array ( 'bar' => 1, 'foo' => 2 ) ** 3.14 = 0
array ( 'bar' => 1, 'foo' => 2 ) ** '0' = 0
array ( 'bar' => 1, 'foo' => 2 ) ** '10' = 0
array ( 'bar' => 1, 'foo' => 2 ) ** '010' = 0
array ( 'bar' => 1, 'foo' => 2 ) ** '10 elephants' = 0
array ( 'bar' => 1, 'foo' => 2 ) ** 'foo' = 0
array ( 'bar' => 1, 'foo' => 2 ) ** array ( ) = 0
array ( 'bar' => 1, 'foo' => 2 ) ** array ( 0 => 1 ) = 0
array ( 'bar' => 1, 'foo' => 2 ) ** array ( 0 => 1, 1 => 100 ) = 0
array ( 'bar' => 1, 'foo' => 2 ) ** array ( 'foo' => 1, 'bar' => 2 ) = 0
array ( 'bar' => 1, 'foo' => 2 ) ** array ( 'bar' => 1, 'foo' => 2 ) = 0
array ( 'bar' => 1, 'foo' => 2 ) ** (object) array ( ) = 0
array ( 'bar' => 1, 'foo' => 2 ) ** (object) array ( 'foo' => 1, 'bar' => 2 ) = 0
array ( 'bar' => 1, 'foo' => 2 ) ** (object) array ( 'bar' => 1, 'foo' => 2 ) = 0
array ( 'bar' => 1, 'foo' => 2 ) ** DateTime = 0
array ( 'bar' => 1, 'foo' => 2 ) ** resource = 0
array ( 'bar' => 1, 'foo' => 2 ) ** NULL = 0
(object) array ( ) ** false = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( ) ** true = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( ) ** 0 = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( ) ** 10 = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( ) ** 0.0 = 1.0 - Notice Object of class stdClass could not be converted to number
(object) array ( ) ** 10.0 = 1.0 - Notice Object of class stdClass could not be converted to number
(object) array ( ) ** 3.14 = 1.0 - Notice Object of class stdClass could not be converted to number
(object) array ( ) ** '0' = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( ) ** '10' = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( ) ** '010' = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( ) ** '10 elephants' = 1 - Notice A non well formed numeric value encountered
(object) array ( ) ** 'foo' = 1 - Warning A non-numeric value encountered
(object) array ( ) ** array ( ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( ) ** array ( 0 => 1 ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( ) ** array ( 0 => 1, 1 => 100 ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( ) ** array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( ) ** array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( ) ** (object) array ( ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( ) ** (object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( ) ** (object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( ) ** DateTime = 1 - Notice Object of class DateTime could not be converted to number
(object) array ( ) ** resource = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( ) ** NULL = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) ** false = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) ** true = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) ** 0 = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) ** 10 = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) ** 0.0 = 1.0 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) ** 10.0 = 1.0 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) ** 3.14 = 1.0 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) ** '0' = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) ** '10' = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) ** '010' = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) ** '10 elephants' = 1 - Notice A non well formed numeric value encountered
(object) array ( 'foo' => 1, 'bar' => 2 ) ** 'foo' = 1 - Warning A non-numeric value encountered
(object) array ( 'foo' => 1, 'bar' => 2 ) ** array ( ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) ** array ( 0 => 1 ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) ** array ( 0 => 1, 1 => 100 ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) ** array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) ** array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) ** (object) array ( ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) ** (object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) ** (object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) ** DateTime = 1 - Notice Object of class DateTime could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) ** resource = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'foo' => 1, 'bar' => 2 ) ** NULL = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) ** false = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) ** true = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) ** 0 = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) ** 10 = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) ** 0.0 = 1.0 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) ** 10.0 = 1.0 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) ** 3.14 = 1.0 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) ** '0' = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) ** '10' = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) ** '010' = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) ** '10 elephants' = 1 - Notice A non well formed numeric value encountered
(object) array ( 'bar' => 1, 'foo' => 2 ) ** 'foo' = 1 - Warning A non-numeric value encountered
(object) array ( 'bar' => 1, 'foo' => 2 ) ** array ( ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) ** array ( 0 => 1 ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) ** array ( 0 => 1, 1 => 100 ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) ** array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) ** array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) ** (object) array ( ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) ** (object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) ** (object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) ** DateTime = 1 - Notice Object of class DateTime could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) ** resource = 1 - Notice Object of class stdClass could not be converted to number
(object) array ( 'bar' => 1, 'foo' => 2 ) ** NULL = 1 - Notice Object of class stdClass could not be converted to number
DateTime ** false = 1 - Notice Object of class DateTime could not be converted to number
DateTime ** true = 1 - Notice Object of class DateTime could not be converted to number
DateTime ** 0 = 1 - Notice Object of class DateTime could not be converted to number
DateTime ** 10 = 1 - Notice Object of class DateTime could not be converted to number
DateTime ** 0.0 = 1.0 - Notice Object of class DateTime could not be converted to number
DateTime ** 10.0 = 1.0 - Notice Object of class DateTime could not be converted to number
DateTime ** 3.14 = 1.0 - Notice Object of class DateTime could not be converted to number
DateTime ** '0' = 1 - Notice Object of class DateTime could not be converted to number
DateTime ** '10' = 1 - Notice Object of class DateTime could not be converted to number
DateTime ** '010' = 1 - Notice Object of class DateTime could not be converted to number
DateTime ** '10 elephants' = 1 - Notice A non well formed numeric value encountered
DateTime ** 'foo' = 1 - Warning A non-numeric value encountered
DateTime ** array ( ) = 1 - Notice Object of class DateTime could not be converted to number
DateTime ** array ( 0 => 1 ) = 1 - Notice Object of class DateTime could not be converted to number
DateTime ** array ( 0 => 1, 1 => 100 ) = 1 - Notice Object of class DateTime could not be converted to number
DateTime ** array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class DateTime could not be converted to number
DateTime ** array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class DateTime could not be converted to number
DateTime ** (object) array ( ) = 1 - Notice Object of class stdClass could not be converted to number
DateTime ** (object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
DateTime ** (object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to number
DateTime ** DateTime = 1 - Notice Object of class DateTime could not be converted to number
DateTime ** resource = 1 - Notice Object of class DateTime could not be converted to number
DateTime ** NULL = 1 - Notice Object of class DateTime could not be converted to number
resource ** false = 1
resource ** true = 6
resource ** 0 = 1
resource ** 10 = 60466176
resource ** 0.0 = 1.0
resource ** 10.0 = 60466176.0
resource ** 3.14 = 277.5843173597815
resource ** '0' = 1
resource ** '10' = 60466176
resource ** '010' = 60466176
resource ** '10 elephants' = 60466176 - Notice A non well formed numeric value encountered
resource ** 'foo' = 1 - Warning A non-numeric value encountered
resource ** array ( ) = 1
resource ** array ( 0 => 1 ) = 1
resource ** array ( 0 => 1, 1 => 100 ) = 1
resource ** array ( 'foo' => 1, 'bar' => 2 ) = 1
resource ** array ( 'bar' => 1, 'foo' => 2 ) = 1
resource ** (object) array ( ) = 6 - Notice Object of class stdClass could not be converted to number
resource ** (object) array ( 'foo' => 1, 'bar' => 2 ) = 6 - Notice Object of class stdClass could not be converted to number
resource ** (object) array ( 'bar' => 1, 'foo' => 2 ) = 6 - Notice Object of class stdClass could not be converted to number
resource ** DateTime = 6 - Notice Object of class DateTime could not be converted to number
resource ** resource = 46656
resource ** NULL = 1
NULL ** false = 1
NULL ** true = 0
NULL ** 0 = 1
NULL ** 10 = 0
NULL ** 0.0 = 1.0
NULL ** 10.0 = 0.0
NULL ** 3.14 = 0.0
NULL ** '0' = 1
NULL ** '10' = 0
NULL ** '010' = 0
NULL ** '10 elephants' = 0 - Notice A non well formed numeric value encountered
NULL ** 'foo' = 1 - Warning A non-numeric value encountered
NULL ** array ( ) = 1
NULL ** array ( 0 => 1 ) = 1
NULL ** array ( 0 => 1, 1 => 100 ) = 1
NULL ** array ( 'foo' => 1, 'bar' => 2 ) = 1
NULL ** array ( 'bar' => 1, 'foo' => 2 ) = 1
NULL ** (object) array ( ) = 0 - Notice Object of class stdClass could not be converted to number
NULL ** (object) array ( 'foo' => 1, 'bar' => 2 ) = 0 - Notice Object of class stdClass could not be converted to number
NULL ** (object) array ( 'bar' => 1, 'foo' => 2 ) = 0 - Notice Object of class stdClass could not be converted to number
NULL ** DateTime = 0 - Notice Object of class DateTime could not be converted to number
NULL ** resource = 0
NULL ** NULL = 1