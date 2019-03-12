--TEST--
Transliterator::createInverse (error)
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

ini_set("intl.error_level", E_WARNING);

transliterator_create_inverse("jj");
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Argument 1 passed to transliterator_create_inverse() must be an instance of Transliterator, string given in %s:%d
Stack trace:
#0 %s(%d): transliterator_create_inverse('jj')
#1 {main}
  thrown in %s on line %d
