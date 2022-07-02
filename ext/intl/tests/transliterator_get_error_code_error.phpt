--TEST--
Transliterator::getErrorCode (error)
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
echo transliterator_get_error_code(array()), "\n";
?>
--EXPECTF--
Fatal error: Uncaught TypeError: transliterator_get_error_code(): Argument #1 ($transliterator) must be of type Transliterator, array given in %s:%d
Stack trace:
#0 %s(%d): transliterator_get_error_code(Array)
#1 {main}
  thrown in %s on line %d
