--TEST--
Check the function UConverter::getAvailable with parameter wrong
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php UConverter::getAvailable("This is an ascii string"); ?>
--EXPECTF--
Warning: UConverter::getAvailable() expects exactly 0 parameters, 1 given in %s on line %d
