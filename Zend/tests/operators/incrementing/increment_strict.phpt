--TEST--
increment (++) operator with strict_operators
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_one_operand('++$a', function($a) { return ++$a; });

--EXPECT--
++false - TypeError Unsupported operand type bool for '++' (increment) operator
++true - TypeError Unsupported operand type bool for '++' (increment) operator
++0 = 1
++10 = 11
++0.0 = 1.0
++10.0 = 11.0
++3.14 = 4.140000000000001
++'0' = '1'
++'10' = '11'
++'010' = '011'
++'10 elephants' = '10 elephantt'
++'foo' = 'fop'
++array ( ) - TypeError Unsupported operand type array for '++' (increment) operator
++array ( 0 => 1 ) - TypeError Unsupported operand type array for '++' (increment) operator
++array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for '++' (increment) operator
++array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for '++' (increment) operator
++array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for '++' (increment) operator
++(object) array ( ) - TypeError Unsupported operand type object for '++' (increment) operator
++(object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for '++' (increment) operator
++(object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for '++' (increment) operator
++DateTime - TypeError Unsupported operand type object for '++' (increment) operator
++resource - TypeError Unsupported operand type resource for '++' (increment) operator
++NULL - TypeError Unsupported operand type null for '++' (increment) operator