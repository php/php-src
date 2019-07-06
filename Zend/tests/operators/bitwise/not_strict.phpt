--TEST--
bitwise not operator with strict operators
--FILE--
<?php
declare(strict_operators=1);

require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_one_operand('~$a', function($a) { return ~$a; }, 'var_out_base64');

--EXPECT--
~false - TypeError Unsupported operand types
~true - TypeError Unsupported operand types
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
~array ( ) - TypeError Unsupported operand types
~array ( 0 => 1 ) - TypeError Unsupported operand types
~array ( 0 => 1, 1 => 100 ) - TypeError Unsupported operand types
~array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
~array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
~(object) array ( ) - TypeError Unsupported operand types
~(object) array ( 'foo' => 1, 'bar' => 2 ) - TypeError Unsupported operand types
~(object) array ( 'bar' => 1, 'foo' => 2 ) - TypeError Unsupported operand types
~DateTime - TypeError Unsupported operand types
~resource - TypeError Unsupported operand types
~NULL - TypeError Unsupported operand types