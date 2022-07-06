--TEST--
Bug #78239: Deprecation notice during string conversion converted to exception hangs
--SKIPIF--
<?php if (!extension_loaded("zend-test")) die("skip requires zend-test extension"); ?>
--FILE--
<?php
function handleError($level, $message, $file = '', $line = 0, $context = [])
{
    throw new ErrorException($message, 0, $level, $file, $line);
}

set_error_handler('handleError');

$r = new _ZendTestClass;
(string)$r ?: "";

?>
--EXPECTF--
Fatal error: Uncaught ErrorException: Method _ZendTestClass::__toString() is deprecated in %s:%d
Stack trace:
#0 %s(%d): handleError(%s)
#1 {main}
  thrown in %s on line %d
