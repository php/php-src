--TEST--
increment (++) operator with strict_operators
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_one_operand('++$a', function($a) { return ++$a; });

--EXPECT--
++false - TypeError Unsupported operand type bool for increment operation
++true - TypeError Unsupported operand type bool for increment operation
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
++array ( ) - TypeError Unsupported operand type array for increment operation
++array ( 0 => 1 ) - TypeError Unsupported operand type array for increment operation
++array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for increment operation
++array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for increment operation
++array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for increment operation
++(object) array ( ) - TypeError Unsupported operand type object for increment operation
++(object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for increment operation
++(object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for increment operation
++DateTime - TypeError Unsupported operand type object for increment operation
++resource - TypeError Unsupported operand type resource for increment operation
++NULL - TypeError Unsupported operand type null for increment operation