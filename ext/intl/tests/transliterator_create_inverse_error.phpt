--TEST--
Transliterator::createInverse (error)
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

ini_set("intl.error_level", E_WARNING);

$tr = Transliterator::create("Katakana-Latin");
$tr->createInverse(array());

$tr = Transliterator::create("Katakana-Latin");
transliterator_create_inverse("jj");

--EXPECTF--
Warning: Transliterator::createInverse() expects exactly 0 parameters, 1 given in %s on line %d

Warning: Transliterator::createInverse(): transliterator_create_inverse: bad arguments in %s on line %d

Catchable fatal error: Argument 1 passed to transliterator_create_inverse() must be an instance of Transliterator, string given in %s on line %d
