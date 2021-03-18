--TEST--
BUG #77664 (Segmentation fault when using undefined constant in custom wrapper)
--FILE--
<?php
try {
    class ErrorWrapper {
        public $context;
        public $var = self::INVALID;
    }
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
stream_wrapper_register('error',ErrorWrapper::class);
file_get_contents('error://test');
?>
--EXPECTF--
Undefined constant self::INVALID

Fatal error: Uncaught Error: Undefined constant self::INVALID in %s:%d
Stack trace:
#0 %sbug77664.php(%d): file_get_contents('error://test')
#1 {main}
  thrown in %sbug77664.php on line %d
