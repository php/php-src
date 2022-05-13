--TEST--
Bug #76536 (PHP crashes with core dump when throwing exception in error handler)
--SKIPIF--
<?php
if (getenv('SKIP_PRELOAD')) die('skip Error handler not available during preloading');
?>
--FILE--
<?php
class SomeConstants {const SOME_CONSTANT = "0foo" % 5; }

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
