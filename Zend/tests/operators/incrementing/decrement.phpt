--TEST--
decrement (--) operator
--FILE--
<?php
require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_one_operand('--$a', function($a) { return --$a; });

--EXPECT--
--false = false
--true = true
--0 = -1
--10 = 9
--0.0 = -1.0
--10.0 = 9.0
--3.14 = 2.14
--'0' = -1
--'10' = 9
--'010' = 9
--'10 elephants' = '10 elephants'
--'foo' = 'foo'
--array ( ) = array ( )
--array ( 0 => 1 ) = array ( 0 => 1 )
--array ( 0 => 1, 1 => 100 ) = array ( 0 => 1, 1 => 100 )
--array ( 'foo' => 1, 'bar' => 2 ) = array ( 'foo' => 1, 'bar' => 2 )
--array ( 'bar' => 1, 'foo' => 2 ) = array ( 'bar' => 1, 'foo' => 2 )
--(object) array ( ) = (object) array ( )
--(object) array ( 'foo' => 1, 'bar' => 2 ) = (object) array ( 'foo' => 1, 'bar' => 2 )
--(object) array ( 'bar' => 1, 'foo' => 2 ) = (object) array ( 'bar' => 1, 'foo' => 2 )
--DateTime = DateTime
--resource = resource
--NULL = NULL