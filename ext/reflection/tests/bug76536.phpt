--TEST--
Bug #76536 (PHP crashes with core dump when throwing exception in error handler)
--FILE--
<?php
class SomeConstants {const SOME_CONSTANT = SOME_NONSENSE;}

function handleError() {throw new ErrorException();}

set_error_handler('handleError');
set_exception_handler('handleError');

$r = new \ReflectionClass(SomeConstants::class);
$r->getConstants();
?>
--EXPECTF--
Fatal error: Uncaught ErrorException in %sbug76536.php:%d
Stack trace:
#0 [internal function]: handleError(Object(ErrorException))
#1 {main}
  thrown in %sbug76536.php on line %d
