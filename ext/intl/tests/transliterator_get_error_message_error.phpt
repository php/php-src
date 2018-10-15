--TEST--
Transliterator::getErrorMessage (error)
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
$t = Transliterator::create("[\p{Bidi_Mirrored}] Hex");
echo transliterator_get_error_message(), "\n";
echo $t->getErrorMessage(null), "\n";
echo transliterator_get_error_message(array()), "\n";
--EXPECTF--
Warning: transliterator_get_error_message() expects exactly 1 parameter, 0 given in %s on line %d

Warning: transliterator_get_error_message(): transliterator_get_error_message: unable to parse input params in %s on line %d


Warning: Transliterator::getErrorMessage() expects exactly 0 parameters, 1 given in %s on line %d

Warning: Transliterator::getErrorMessage(): transliterator_get_error_message: unable to parse input params in %s on line %d


Fatal error: Uncaught TypeError: Argument 1 passed to transliterator_get_error_message() must be an instance of Transliterator, array given in %s:%d
Stack trace:
#0 %s(%d): transliterator_get_error_message(Array)
#1 {main}
  thrown in %s on line %d
