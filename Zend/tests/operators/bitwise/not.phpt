--TEST--
bitwise not operator
--FILE--
<?php
require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_one_operand('~$a', function($a) { return ~$a; }, 'var_out_base64');

--EXPECT--
~false - Error Unsupported operand types
~true - Error Unsupported operand types
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
~array ( ) - Error Unsupported operand types
~array ( 0 => 1 ) - Error Unsupported operand types
~array ( 0 => 1, 1 => 100 ) - Error Unsupported operand types
~array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
~array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
~(object) array ( ) - Error Unsupported operand types
~(object) array ( 'foo' => 1, 'bar' => 2 ) - Error Unsupported operand types
~(object) array ( 'bar' => 1, 'foo' => 2 ) - Error Unsupported operand types
~DateTime - Error Unsupported operand types
~resource - Error Unsupported operand types
~NULL - Error Unsupported operand types
