--TEST--
BUG #77664 (Segmentation fault when using undefined constant in custom wrapper)
--FILE--
<?php
class ErrorWrapper {
    public $context;
    public $var = self::INVALID;
}
stream_wrapper_register('error',ErrorWrapper::class);
file_get_contents('error://test');
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant self::INVALID in %s:%d
Stack trace:
#0 %s(%d): [constant expression]()
#1 %s(%d): file_get_contents('error://test')
#2 {main}
  thrown in %sbug77664.php on line %d
