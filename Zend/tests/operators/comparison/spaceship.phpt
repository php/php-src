--TEST--
spaceship '<=>' operator
--FILE--
<?php
require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_two_operands('$a <=> $b', function($a, $b) { return $a <=> $b; });

--EXPECT--
false <=> false = 0
false <=> true = -1
false <=> 0 = 0
false <=> 10 = -1
false <=> 0.0 = 0
false <=> 10.0 = -1
false <=> 3.14 = -1
false <=> '0' = 0
false <=> '10' = -1
false <=> '010' = -1
false <=> '10 elephants' = -1
false <=> 'foo' = -1
false <=> array ( ) = 0
false <=> array ( 0 => 1 ) = -1
false <=> array ( 0 => 1, 1 => 100 ) = -1
false <=> array ( 'foo' => 1, 'bar' => 2 ) = -1
false <=> array ( 'bar' => 1, 'foo' => 2 ) = -1
false <=> (object) array ( ) = -1
false <=> (object) array ( 'foo' => 1, 'bar' => 2 ) = -1
false <=> (object) array ( 'bar' => 1, 'foo' => 2 ) = -1
false <=> DateTime = -1
false <=> resource = -1
false <=> NULL = 0
true <=> false = 1
true <=> true = 0
true <=> 0 = 1
true <=> 10 = 0
true <=> 0.0 = 1
true <=> 10.0 = 0
true <=> 3.14 = 0
true <=> '0' = 1
true <=> '10' = 0
true <=> '010' = 0
true <=> '10 elephants' = 0
true <=> 'foo' = 0
true <=> array ( ) = 1
true <=> array ( 0 => 1 ) = 0
true <=> array ( 0 => 1, 1 => 100 ) = 0
true <=> array ( 'foo' => 1, 'bar' => 2 ) = 0
true <=> array ( 'bar' => 1, 'foo' => 2 ) = 0
true <=> (object) array ( ) = 0
true <=> (object) array ( 'foo' => 1, 'bar' => 2 ) = 0
true <=> (object) array ( 'bar' => 1, 'foo' => 2 ) = 0
true <=> DateTime = 0
true <=> resource = 0
true <=> NULL = 1
0 <=> false = 0
0 <=> true = -1
0 <=> 0 = 0
0 <=> 10 = -1
0 <=> 0.0 = 0
0 <=> 10.0 = -1
0 <=> 3.14 = -1
0 <=> '0' = 0
0 <=> '10' = -1
0 <=> '010' = -1
0 <=> '10 elephants' = -1
0 <=> 'foo' = 0
0 <=> array ( ) = -1
0 <=> array ( 0 => 1 ) = -1
0 <=> array ( 0 => 1, 1 => 100 ) = -1
0 <=> array ( 'foo' => 1, 'bar' => 2 ) = -1
0 <=> array ( 'bar' => 1, 'foo' => 2 ) = -1
0 <=> (object) array ( ) = -1 - Notice Object of class stdClass could not be converted to int
0 <=> (object) array ( 'foo' => 1, 'bar' => 2 ) = -1 - Notice Object of class stdClass could not be converted to int
0 <=> (object) array ( 'bar' => 1, 'foo' => 2 ) = -1 - Notice Object of class stdClass could not be converted to int
0 <=> DateTime = -1 - Notice Object of class DateTime could not be converted to int
0 <=> resource = -1
0 <=> NULL = 0
10 <=> false = 1
10 <=> true = 0
10 <=> 0 = 1
10 <=> 10 = 0
10 <=> 0.0 = 1
10 <=> 10.0 = 0
10 <=> 3.14 = 1
10 <=> '0' = 1
10 <=> '10' = 0
10 <=> '010' = 0
10 <=> '10 elephants' = 0
10 <=> 'foo' = 1
10 <=> array ( ) = -1
10 <=> array ( 0 => 1 ) = -1
10 <=> array ( 0 => 1, 1 => 100 ) = -1
10 <=> array ( 'foo' => 1, 'bar' => 2 ) = -1
10 <=> array ( 'bar' => 1, 'foo' => 2 ) = -1
10 <=> (object) array ( ) = 1 - Notice Object of class stdClass could not be converted to int
10 <=> (object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
10 <=> (object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to int
10 <=> DateTime = 1 - Notice Object of class DateTime could not be converted to int
10 <=> resource = 1
10 <=> NULL = 1
0.0 <=> false = 0
0.0 <=> true = -1
0.0 <=> 0 = 0
0.0 <=> 10 = -1
0.0 <=> 0.0 = 0
0.0 <=> 10.0 = -1
0.0 <=> 3.14 = -1
0.0 <=> '0' = 0
0.0 <=> '10' = -1
0.0 <=> '010' = -1
0.0 <=> '10 elephants' = -1
0.0 <=> 'foo' = 0
0.0 <=> array ( ) = -1
0.0 <=> array ( 0 => 1 ) = -1
0.0 <=> array ( 0 => 1, 1 => 100 ) = -1
0.0 <=> array ( 'foo' => 1, 'bar' => 2 ) = -1
0.0 <=> array ( 'bar' => 1, 'foo' => 2 ) = -1
0.0 <=> (object) array ( ) = -1 - Notice Object of class stdClass could not be converted to float
0.0 <=> (object) array ( 'foo' => 1, 'bar' => 2 ) = -1 - Notice Object of class stdClass could not be converted to float
0.0 <=> (object) array ( 'bar' => 1, 'foo' => 2 ) = -1 - Notice Object of class stdClass could not be converted to float
0.0 <=> DateTime = -1 - Notice Object of class DateTime could not be converted to float
0.0 <=> resource = -1
0.0 <=> NULL = 0
10.0 <=> false = 1
10.0 <=> true = 0
10.0 <=> 0 = 1
10.0 <=> 10 = 0
10.0 <=> 0.0 = 1
10.0 <=> 10.0 = 0
10.0 <=> 3.14 = 1
10.0 <=> '0' = 1
10.0 <=> '10' = 0
10.0 <=> '010' = 0
10.0 <=> '10 elephants' = 0
10.0 <=> 'foo' = 1
10.0 <=> array ( ) = -1
10.0 <=> array ( 0 => 1 ) = -1
10.0 <=> array ( 0 => 1, 1 => 100 ) = -1
10.0 <=> array ( 'foo' => 1, 'bar' => 2 ) = -1
10.0 <=> array ( 'bar' => 1, 'foo' => 2 ) = -1
10.0 <=> (object) array ( ) = 1 - Notice Object of class stdClass could not be converted to float
10.0 <=> (object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to float
10.0 <=> (object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to float
10.0 <=> DateTime = 1 - Notice Object of class DateTime could not be converted to float
10.0 <=> resource = 1
10.0 <=> NULL = 1
3.14 <=> false = 1
3.14 <=> true = 0
3.14 <=> 0 = 1
3.14 <=> 10 = -1
3.14 <=> 0.0 = 1
3.14 <=> 10.0 = -1
3.14 <=> 3.14 = 0
3.14 <=> '0' = 1
3.14 <=> '10' = -1
3.14 <=> '010' = -1
3.14 <=> '10 elephants' = -1
3.14 <=> 'foo' = 1
3.14 <=> array ( ) = -1
3.14 <=> array ( 0 => 1 ) = -1
3.14 <=> array ( 0 => 1, 1 => 100 ) = -1
3.14 <=> array ( 'foo' => 1, 'bar' => 2 ) = -1
3.14 <=> array ( 'bar' => 1, 'foo' => 2 ) = -1
3.14 <=> (object) array ( ) = 1 - Notice Object of class stdClass could not be converted to float
3.14 <=> (object) array ( 'foo' => 1, 'bar' => 2 ) = 1 - Notice Object of class stdClass could not be converted to float
3.14 <=> (object) array ( 'bar' => 1, 'foo' => 2 ) = 1 - Notice Object of class stdClass could not be converted to float
3.14 <=> DateTime = 1 - Notice Object of class DateTime could not be converted to float
3.14 <=> resource = -1
3.14 <=> NULL = 1
'0' <=> false = 0
'0' <=> true = -1
'0' <=> 0 = 0
'0' <=> 10 = -1
'0' <=> 0.0 = 0
'0' <=> 10.0 = -1
'0' <=> 3.14 = -1
'0' <=> '0' = 0
'0' <=> '10' = -1
'0' <=> '010' = -1
'0' <=> '10 elephants' = -1
'0' <=> 'foo' = -1
'0' <=> array ( ) = -1
'0' <=> array ( 0 => 1 ) = -1
'0' <=> array ( 0 => 1, 1 => 100 ) = -1
'0' <=> array ( 'foo' => 1, 'bar' => 2 ) = -1
'0' <=> array ( 'bar' => 1, 'foo' => 2 ) = -1
'0' <=> (object) array ( ) = -1
'0' <=> (object) array ( 'foo' => 1, 'bar' => 2 ) = -1
'0' <=> (object) array ( 'bar' => 1, 'foo' => 2 ) = -1
'0' <=> DateTime = -1
'0' <=> resource = -1
'0' <=> NULL = 1
'10' <=> false = 1
'10' <=> true = 0
'10' <=> 0 = 1
'10' <=> 10 = 0
'10' <=> 0.0 = 1
'10' <=> 10.0 = 0
'10' <=> 3.14 = 1
'10' <=> '0' = 1
'10' <=> '10' = 0
'10' <=> '010' = 0
'10' <=> '10 elephants' = -1
'10' <=> 'foo' = -1
'10' <=> array ( ) = -1
'10' <=> array ( 0 => 1 ) = -1
'10' <=> array ( 0 => 1, 1 => 100 ) = -1
'10' <=> array ( 'foo' => 1, 'bar' => 2 ) = -1
'10' <=> array ( 'bar' => 1, 'foo' => 2 ) = -1
'10' <=> (object) array ( ) = -1
'10' <=> (object) array ( 'foo' => 1, 'bar' => 2 ) = -1
'10' <=> (object) array ( 'bar' => 1, 'foo' => 2 ) = -1
'10' <=> DateTime = -1
'10' <=> resource = 1
'10' <=> NULL = 1
'010' <=> false = 1
'010' <=> true = 0
'010' <=> 0 = 1
'010' <=> 10 = 0
'010' <=> 0.0 = 1
'010' <=> 10.0 = 0
'010' <=> 3.14 = 1
'010' <=> '0' = 1
'010' <=> '10' = 0
'010' <=> '010' = 0
'010' <=> '10 elephants' = -1
'010' <=> 'foo' = -1
'010' <=> array ( ) = -1
'010' <=> array ( 0 => 1 ) = -1
'010' <=> array ( 0 => 1, 1 => 100 ) = -1
'010' <=> array ( 'foo' => 1, 'bar' => 2 ) = -1
'010' <=> array ( 'bar' => 1, 'foo' => 2 ) = -1
'010' <=> (object) array ( ) = -1
'010' <=> (object) array ( 'foo' => 1, 'bar' => 2 ) = -1
'010' <=> (object) array ( 'bar' => 1, 'foo' => 2 ) = -1
'010' <=> DateTime = -1
'010' <=> resource = 1
'010' <=> NULL = 1
'10 elephants' <=> false = 1
'10 elephants' <=> true = 0
'10 elephants' <=> 0 = 1
'10 elephants' <=> 10 = 0
'10 elephants' <=> 0.0 = 1
'10 elephants' <=> 10.0 = 0
'10 elephants' <=> 3.14 = 1
'10 elephants' <=> '0' = 1
'10 elephants' <=> '10' = 1
'10 elephants' <=> '010' = 1
'10 elephants' <=> '10 elephants' = 0
'10 elephants' <=> 'foo' = -1
'10 elephants' <=> array ( ) = -1
'10 elephants' <=> array ( 0 => 1 ) = -1
'10 elephants' <=> array ( 0 => 1, 1 => 100 ) = -1
'10 elephants' <=> array ( 'foo' => 1, 'bar' => 2 ) = -1
'10 elephants' <=> array ( 'bar' => 1, 'foo' => 2 ) = -1
'10 elephants' <=> (object) array ( ) = -1
'10 elephants' <=> (object) array ( 'foo' => 1, 'bar' => 2 ) = -1
'10 elephants' <=> (object) array ( 'bar' => 1, 'foo' => 2 ) = -1
'10 elephants' <=> DateTime = -1
'10 elephants' <=> resource = 1
'10 elephants' <=> NULL = 1
'foo' <=> false = 1
'foo' <=> true = 0
'foo' <=> 0 = 0
'foo' <=> 10 = -1
'foo' <=> 0.0 = 0
'foo' <=> 10.0 = -1
'foo' <=> 3.14 = -1
'foo' <=> '0' = 1
'foo' <=> '10' = 1
'foo' <=> '010' = 1
'foo' <=> '10 elephants' = 1
'foo' <=> 'foo' = 0
'foo' <=> array ( ) = -1
'foo' <=> array ( 0 => 1 ) = -1
'foo' <=> array ( 0 => 1, 1 => 100 ) = -1
'foo' <=> array ( 'foo' => 1, 'bar' => 2 ) = -1
'foo' <=> array ( 'bar' => 1, 'foo' => 2 ) = -1
'foo' <=> (object) array ( ) = -1
'foo' <=> (object) array ( 'foo' => 1, 'bar' => 2 ) = -1
'foo' <=> (object) array ( 'bar' => 1, 'foo' => 2 ) = -1
'foo' <=> DateTime = -1
'foo' <=> resource = -1
'foo' <=> NULL = 1
array ( ) <=> false = 0
array ( ) <=> true = -1
array ( ) <=> 0 = 1
array ( ) <=> 10 = 1
array ( ) <=> 0.0 = 1
array ( ) <=> 10.0 = 1
array ( ) <=> 3.14 = 1
array ( ) <=> '0' = 1
array ( ) <=> '10' = 1
array ( ) <=> '010' = 1
array ( ) <=> '10 elephants' = 1
array ( ) <=> 'foo' = 1
array ( ) <=> array ( ) = 0
array ( ) <=> array ( 0 => 1 ) = -1
array ( ) <=> array ( 0 => 1, 1 => 100 ) = -1
array ( ) <=> array ( 'foo' => 1, 'bar' => 2 ) = -1
array ( ) <=> array ( 'bar' => 1, 'foo' => 2 ) = -1
array ( ) <=> (object) array ( ) = -1
array ( ) <=> (object) array ( 'foo' => 1, 'bar' => 2 ) = -1
array ( ) <=> (object) array ( 'bar' => 1, 'foo' => 2 ) = -1
array ( ) <=> DateTime = -1
array ( ) <=> resource = 1
array ( ) <=> NULL = 0
array ( 0 => 1 ) <=> false = 1
array ( 0 => 1 ) <=> true = 0
array ( 0 => 1 ) <=> 0 = 1
array ( 0 => 1 ) <=> 10 = 1
array ( 0 => 1 ) <=> 0.0 = 1
array ( 0 => 1 ) <=> 10.0 = 1
array ( 0 => 1 ) <=> 3.14 = 1
array ( 0 => 1 ) <=> '0' = 1
array ( 0 => 1 ) <=> '10' = 1
array ( 0 => 1 ) <=> '010' = 1
array ( 0 => 1 ) <=> '10 elephants' = 1
array ( 0 => 1 ) <=> 'foo' = 1
array ( 0 => 1 ) <=> array ( ) = 1
array ( 0 => 1 ) <=> array ( 0 => 1 ) = 0
array ( 0 => 1 ) <=> array ( 0 => 1, 1 => 100 ) = -1
array ( 0 => 1 ) <=> array ( 'foo' => 1, 'bar' => 2 ) = -1
array ( 0 => 1 ) <=> array ( 'bar' => 1, 'foo' => 2 ) = -1
array ( 0 => 1 ) <=> (object) array ( ) = -1
array ( 0 => 1 ) <=> (object) array ( 'foo' => 1, 'bar' => 2 ) = -1
array ( 0 => 1 ) <=> (object) array ( 'bar' => 1, 'foo' => 2 ) = -1
array ( 0 => 1 ) <=> DateTime = -1
array ( 0 => 1 ) <=> resource = 1
array ( 0 => 1 ) <=> NULL = 1
array ( 0 => 1, 1 => 100 ) <=> false = 1
array ( 0 => 1, 1 => 100 ) <=> true = 0
array ( 0 => 1, 1 => 100 ) <=> 0 = 1
array ( 0 => 1, 1 => 100 ) <=> 10 = 1
array ( 0 => 1, 1 => 100 ) <=> 0.0 = 1
array ( 0 => 1, 1 => 100 ) <=> 10.0 = 1
array ( 0 => 1, 1 => 100 ) <=> 3.14 = 1
array ( 0 => 1, 1 => 100 ) <=> '0' = 1
array ( 0 => 1, 1 => 100 ) <=> '10' = 1
array ( 0 => 1, 1 => 100 ) <=> '010' = 1
array ( 0 => 1, 1 => 100 ) <=> '10 elephants' = 1
array ( 0 => 1, 1 => 100 ) <=> 'foo' = 1
array ( 0 => 1, 1 => 100 ) <=> array ( ) = 1
array ( 0 => 1, 1 => 100 ) <=> array ( 0 => 1 ) = 1
array ( 0 => 1, 1 => 100 ) <=> array ( 0 => 1, 1 => 100 ) = 0
array ( 0 => 1, 1 => 100 ) <=> array ( 'foo' => 1, 'bar' => 2 ) = 1
array ( 0 => 1, 1 => 100 ) <=> array ( 'bar' => 1, 'foo' => 2 ) = 1
array ( 0 => 1, 1 => 100 ) <=> (object) array ( ) = -1
array ( 0 => 1, 1 => 100 ) <=> (object) array ( 'foo' => 1, 'bar' => 2 ) = -1
array ( 0 => 1, 1 => 100 ) <=> (object) array ( 'bar' => 1, 'foo' => 2 ) = -1
array ( 0 => 1, 1 => 100 ) <=> DateTime = -1
array ( 0 => 1, 1 => 100 ) <=> resource = 1
array ( 0 => 1, 1 => 100 ) <=> NULL = 1
array ( 'foo' => 1, 'bar' => 2 ) <=> false = 1
array ( 'foo' => 1, 'bar' => 2 ) <=> true = 0
array ( 'foo' => 1, 'bar' => 2 ) <=> 0 = 1
array ( 'foo' => 1, 'bar' => 2 ) <=> 10 = 1
array ( 'foo' => 1, 'bar' => 2 ) <=> 0.0 = 1
array ( 'foo' => 1, 'bar' => 2 ) <=> 10.0 = 1
array ( 'foo' => 1, 'bar' => 2 ) <=> 3.14 = 1
array ( 'foo' => 1, 'bar' => 2 ) <=> '0' = 1
array ( 'foo' => 1, 'bar' => 2 ) <=> '10' = 1
array ( 'foo' => 1, 'bar' => 2 ) <=> '010' = 1
array ( 'foo' => 1, 'bar' => 2 ) <=> '10 elephants' = 1
array ( 'foo' => 1, 'bar' => 2 ) <=> 'foo' = 1
array ( 'foo' => 1, 'bar' => 2 ) <=> array ( ) = 1
array ( 'foo' => 1, 'bar' => 2 ) <=> array ( 0 => 1 ) = 1
array ( 'foo' => 1, 'bar' => 2 ) <=> array ( 0 => 1, 1 => 100 ) = 1
array ( 'foo' => 1, 'bar' => 2 ) <=> array ( 'foo' => 1, 'bar' => 2 ) = 0
array ( 'foo' => 1, 'bar' => 2 ) <=> array ( 'bar' => 1, 'foo' => 2 ) = -1
array ( 'foo' => 1, 'bar' => 2 ) <=> (object) array ( ) = -1
array ( 'foo' => 1, 'bar' => 2 ) <=> (object) array ( 'foo' => 1, 'bar' => 2 ) = -1
array ( 'foo' => 1, 'bar' => 2 ) <=> (object) array ( 'bar' => 1, 'foo' => 2 ) = -1
array ( 'foo' => 1, 'bar' => 2 ) <=> DateTime = -1
array ( 'foo' => 1, 'bar' => 2 ) <=> resource = 1
array ( 'foo' => 1, 'bar' => 2 ) <=> NULL = 1
array ( 'bar' => 1, 'foo' => 2 ) <=> false = 1
array ( 'bar' => 1, 'foo' => 2 ) <=> true = 0
array ( 'bar' => 1, 'foo' => 2 ) <=> 0 = 1
array ( 'bar' => 1, 'foo' => 2 ) <=> 10 = 1
array ( 'bar' => 1, 'foo' => 2 ) <=> 0.0 = 1
array ( 'bar' => 1, 'foo' => 2 ) <=> 10.0 = 1
array ( 'bar' => 1, 'foo' => 2 ) <=> 3.14 = 1
array ( 'bar' => 1, 'foo' => 2 ) <=> '0' = 1
array ( 'bar' => 1, 'foo' => 2 ) <=> '10' = 1
array ( 'bar' => 1, 'foo' => 2 ) <=> '010' = 1
array ( 'bar' => 1, 'foo' => 2 ) <=> '10 elephants' = 1
array ( 'bar' => 1, 'foo' => 2 ) <=> 'foo' = 1
array ( 'bar' => 1, 'foo' => 2 ) <=> array ( ) = 1
array ( 'bar' => 1, 'foo' => 2 ) <=> array ( 0 => 1 ) = 1
array ( 'bar' => 1, 'foo' => 2 ) <=> array ( 0 => 1, 1 => 100 ) = 1
array ( 'bar' => 1, 'foo' => 2 ) <=> array ( 'foo' => 1, 'bar' => 2 ) = -1
array ( 'bar' => 1, 'foo' => 2 ) <=> array ( 'bar' => 1, 'foo' => 2 ) = 0
array ( 'bar' => 1, 'foo' => 2 ) <=> (object) array ( ) = -1
array ( 'bar' => 1, 'foo' => 2 ) <=> (object) array ( 'foo' => 1, 'bar' => 2 ) = -1
array ( 'bar' => 1, 'foo' => 2 ) <=> (object) array ( 'bar' => 1, 'foo' => 2 ) = -1
array ( 'bar' => 1, 'foo' => 2 ) <=> DateTime = -1
array ( 'bar' => 1, 'foo' => 2 ) <=> resource = 1
array ( 'bar' => 1, 'foo' => 2 ) <=> NULL = 1
(object) array ( ) <=> false = 1
(object) array ( ) <=> true = 0
(object) array ( ) <=> 0 = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( ) <=> 10 = -1 - Notice Object of class stdClass could not be converted to int
(object) array ( ) <=> 0.0 = 1 - Notice Object of class stdClass could not be converted to float
(object) array ( ) <=> 10.0 = -1 - Notice Object of class stdClass could not be converted to float
(object) array ( ) <=> 3.14 = -1 - Notice Object of class stdClass could not be converted to float
(object) array ( ) <=> '0' = 1
(object) array ( ) <=> '10' = 1
(object) array ( ) <=> '010' = 1
(object) array ( ) <=> '10 elephants' = 1
(object) array ( ) <=> 'foo' = 1
(object) array ( ) <=> array ( ) = 1
(object) array ( ) <=> array ( 0 => 1 ) = 1
(object) array ( ) <=> array ( 0 => 1, 1 => 100 ) = 1
(object) array ( ) <=> array ( 'foo' => 1, 'bar' => 2 ) = 1
(object) array ( ) <=> array ( 'bar' => 1, 'foo' => 2 ) = 1
(object) array ( ) <=> (object) array ( ) = 0
(object) array ( ) <=> (object) array ( 'foo' => 1, 'bar' => 2 ) = -1
(object) array ( ) <=> (object) array ( 'bar' => 1, 'foo' => 2 ) = -1
(object) array ( ) <=> DateTime = 1
(object) array ( ) <=> resource = 1
(object) array ( ) <=> NULL = 1
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> false = 1
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> true = 0
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> 0 = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> 10 = -1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> 0.0 = 1 - Notice Object of class stdClass could not be converted to float
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> 10.0 = -1 - Notice Object of class stdClass could not be converted to float
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> 3.14 = -1 - Notice Object of class stdClass could not be converted to float
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> '0' = 1
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> '10' = 1
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> '010' = 1
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> '10 elephants' = 1
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> 'foo' = 1
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> array ( ) = 1
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> array ( 0 => 1 ) = 1
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> array ( 0 => 1, 1 => 100 ) = 1
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> array ( 'foo' => 1, 'bar' => 2 ) = 1
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> array ( 'bar' => 1, 'foo' => 2 ) = 1
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> (object) array ( ) = 1
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> (object) array ( 'foo' => 1, 'bar' => 2 ) = 0
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> (object) array ( 'bar' => 1, 'foo' => 2 ) = -1
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> DateTime = 1
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> resource = 1
(object) array ( 'foo' => 1, 'bar' => 2 ) <=> NULL = 1
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> false = 1
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> true = 0
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> 0 = 1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> 10 = -1 - Notice Object of class stdClass could not be converted to int
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> 0.0 = 1 - Notice Object of class stdClass could not be converted to float
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> 10.0 = -1 - Notice Object of class stdClass could not be converted to float
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> 3.14 = -1 - Notice Object of class stdClass could not be converted to float
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> '0' = 1
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> '10' = 1
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> '010' = 1
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> '10 elephants' = 1
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> 'foo' = 1
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> array ( ) = 1
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> array ( 0 => 1 ) = 1
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> array ( 0 => 1, 1 => 100 ) = 1
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> array ( 'foo' => 1, 'bar' => 2 ) = 1
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> array ( 'bar' => 1, 'foo' => 2 ) = 1
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> (object) array ( ) = 1
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> (object) array ( 'foo' => 1, 'bar' => 2 ) = -1
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> (object) array ( 'bar' => 1, 'foo' => 2 ) = 0
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> DateTime = 1
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> resource = 1
(object) array ( 'bar' => 1, 'foo' => 2 ) <=> NULL = 1
DateTime <=> false = 1
DateTime <=> true = 0
DateTime <=> 0 = 1 - Notice Object of class DateTime could not be converted to int
DateTime <=> 10 = -1 - Notice Object of class DateTime could not be converted to int
DateTime <=> 0.0 = 1 - Notice Object of class DateTime could not be converted to float
DateTime <=> 10.0 = -1 - Notice Object of class DateTime could not be converted to float
DateTime <=> 3.14 = -1 - Notice Object of class DateTime could not be converted to float
DateTime <=> '0' = 1
DateTime <=> '10' = 1
DateTime <=> '010' = 1
DateTime <=> '10 elephants' = 1
DateTime <=> 'foo' = 1
DateTime <=> array ( ) = 1
DateTime <=> array ( 0 => 1 ) = 1
DateTime <=> array ( 0 => 1, 1 => 100 ) = 1
DateTime <=> array ( 'foo' => 1, 'bar' => 2 ) = 1
DateTime <=> array ( 'bar' => 1, 'foo' => 2 ) = 1
DateTime <=> (object) array ( ) = 1
DateTime <=> (object) array ( 'foo' => 1, 'bar' => 2 ) = 1
DateTime <=> (object) array ( 'bar' => 1, 'foo' => 2 ) = 1
DateTime <=> DateTime = 0
DateTime <=> resource = 1
DateTime <=> NULL = 1
resource <=> false = 1
resource <=> true = 0
resource <=> 0 = 1
resource <=> 10 = -1
resource <=> 0.0 = 1
resource <=> 10.0 = -1
resource <=> 3.14 = 1
resource <=> '0' = 1
resource <=> '10' = -1
resource <=> '010' = -1
resource <=> '10 elephants' = -1
resource <=> 'foo' = 1
resource <=> array ( ) = -1
resource <=> array ( 0 => 1 ) = -1
resource <=> array ( 0 => 1, 1 => 100 ) = -1
resource <=> array ( 'foo' => 1, 'bar' => 2 ) = -1
resource <=> array ( 'bar' => 1, 'foo' => 2 ) = -1
resource <=> (object) array ( ) = -1
resource <=> (object) array ( 'foo' => 1, 'bar' => 2 ) = -1
resource <=> (object) array ( 'bar' => 1, 'foo' => 2 ) = -1
resource <=> DateTime = -1
resource <=> resource = 0
resource <=> NULL = 1
NULL <=> false = 0
NULL <=> true = -1
NULL <=> 0 = 0
NULL <=> 10 = -1
NULL <=> 0.0 = 0
NULL <=> 10.0 = -1
NULL <=> 3.14 = -1
NULL <=> '0' = -1
NULL <=> '10' = -1
NULL <=> '010' = -1
NULL <=> '10 elephants' = -1
NULL <=> 'foo' = -1
NULL <=> array ( ) = 0
NULL <=> array ( 0 => 1 ) = -1
NULL <=> array ( 0 => 1, 1 => 100 ) = -1
NULL <=> array ( 'foo' => 1, 'bar' => 2 ) = -1
NULL <=> array ( 'bar' => 1, 'foo' => 2 ) = -1
NULL <=> (object) array ( ) = -1
NULL <=> (object) array ( 'foo' => 1, 'bar' => 2 ) = -1
NULL <=> (object) array ( 'bar' => 1, 'foo' => 2 ) = -1
NULL <=> DateTime = -1
NULL <=> resource = -1
NULL <=> NULL = 0