--TEST--
bitwise not operator with strict operators
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_one_operand('~$a', function($a) { return ~$a; }, 'var_out_base64');

--EXPECT--
~false - TypeError Unsupported operand type bool for bitwise not
~true - TypeError Unsupported operand type bool for bitwise not
~0 = -1
~10 = -11
~0.0 = -1
~10.0 = -11
~3.14 = -4
~'0' = base64:zw==
~'10' = base64:zs8=
~'010' = base64:z87P
~'10 elephants' = base64:zs/fmpOaj5eekYuM
~'foo' = base64:mZCQ
~array ( ) - TypeError Unsupported operand type array for bitwise not
~array ( 0 => 1 ) - TypeError Unsupported operand type array for bitwise not
~array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand type array for bitwise not
~array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type array for bitwise not
~array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type array for bitwise not
~(object) array ( ) - TypeError Unsupported operand type object for bitwise not
~(object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand type object for bitwise not
~(object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand type object for bitwise not
~DateTime - TypeError Unsupported operand type object for bitwise not
~resource - TypeError Unsupported operand type resource for bitwise not
~NULL - TypeError Unsupported operand type null for bitwise not