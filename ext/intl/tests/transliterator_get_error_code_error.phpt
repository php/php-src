--TEST--
Transliterator::getErrorCode (error)
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
$t = Transliterator::create("[\p{Bidi_Mirrored}] Hex");
echo transliterator_get_error_code(), "\n";
echo $t->getErrorCode(null), "\n";
echo transliterator_get_error_code(array()), "\n";

--EXPECTF--
Warning: transliterator_get_error_code() expects exactly 1 parameter, 0 given in %s on line %d

Warning: transliterator_get_error_code(): transliterator_get_error_code: unable to parse input params in %s on line %d


Warning: Transliterator::getErrorCode() expects exactly 0 parameters, 1 given in %s on line %d

Warning: Transliterator::getErrorCode(): transliterator_get_error_code: unable to parse input params in %s on line %d


Catchable fatal error: Argument 1 passed to transliterator_get_error_code() must be an instance of Transliterator, array given in %s on line %d
