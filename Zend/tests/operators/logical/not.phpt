--TEST--
logical not '!' operator
--FILE--
<?php
require_once __DIR__ . '/../_helper.inc';

set_error_handler('error_to_exception');

test_one_operand('!$a', function($a) { return !a; });

--EXPECT--
!false = false - Warning Use of undefined constant a - assumed 'a' (this will throw an Error in a future version of PHP)
!true = false - Warning Use of undefined constant a - assumed 'a' (this will throw an Error in a future version of PHP)
!0 = false - Warning Use of undefined constant a - assumed 'a' (this will throw an Error in a future version of PHP)
!10 = false - Warning Use of undefined constant a - assumed 'a' (this will throw an Error in a future version of PHP)
!0.0 = false - Warning Use of undefined constant a - assumed 'a' (this will throw an Error in a future version of PHP)
!10.0 = false - Warning Use of undefined constant a - assumed 'a' (this will throw an Error in a future version of PHP)
!3.14 = false - Warning Use of undefined constant a - assumed 'a' (this will throw an Error in a future version of PHP)
!'0' = false - Warning Use of undefined constant a - assumed 'a' (this will throw an Error in a future version of PHP)
!'10' = false - Warning Use of undefined constant a - assumed 'a' (this will throw an Error in a future version of PHP)
!'010' = false - Warning Use of undefined constant a - assumed 'a' (this will throw an Error in a future version of PHP)
!'10 elephants' = false - Warning Use of undefined constant a - assumed 'a' (this will throw an Error in a future version of PHP)
!'foo' = false - Warning Use of undefined constant a - assumed 'a' (this will throw an Error in a future version of PHP)
!array ( ) = false - Warning Use of undefined constant a - assumed 'a' (this will throw an Error in a future version of PHP)
!array ( 0 => 1 ) = false - Warning Use of undefined constant a - assumed 'a' (this will throw an Error in a future version of PHP)
!array ( 0 => 1, 1 => 100 ) = false - Warning Use of undefined constant a - assumed 'a' (this will throw an Error in a future version of PHP)
!array ( 'foo' => 1, 'bar' => 2 ) = false - Warning Use of undefined constant a - assumed 'a' (this will throw an Error in a future version of PHP)
!array ( 'bar' => 1, 'foo' => 2 ) = false - Warning Use of undefined constant a - assumed 'a' (this will throw an Error in a future version of PHP)
!(object) array ( ) = false - Warning Use of undefined constant a - assumed 'a' (this will throw an Error in a future version of PHP)
!(object) array ( 'foo' => 1, 'bar' => 2 ) = false - Warning Use of undefined constant a - assumed 'a' (this will throw an Error in a future version of PHP)
!(object) array ( 'bar' => 1, 'foo' => 2 ) = false - Warning Use of undefined constant a - assumed 'a' (this will throw an Error in a future version of PHP)
!DateTime = false - Warning Use of undefined constant a - assumed 'a' (this will throw an Error in a future version of PHP)
!resource = false - Warning Use of undefined constant a - assumed 'a' (this will throw an Error in a future version of PHP)
!NULL = false - Warning Use of undefined constant a - assumed 'a' (this will throw an Error in a future version of PHP)