--TEST--
Transliterator::transliterate (error)
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

ini_set("intl.error_level", E_WARNING);

$tr = Transliterator::create("latin");

//Arguments
var_dump(transliterator_transliterate());
var_dump(transliterator_transliterate($tr,array()));
var_dump(transliterator_transliterate($tr,"str",7));
var_dump(transliterator_transliterate($tr,"str",7,6));
var_dump(transliterator_transliterate($tr,"str",2,-1,"extra"));

//Arguments
var_dump($tr->transliterate());
var_dump($tr->transliterate(array()));

//bad UTF-8
transliterator_transliterate($tr, "\x80\x03");

echo "Done.\n";
--EXPECTF--
Warning: transliterator_transliterate() expects at least 2 parameters, 0 given in %s on line %d

Warning: transliterator_transliterate(): transliterator_transliterate: bad arguments in %s on line %d
bool(false)

Warning: transliterator_transliterate() expects parameter 2 to be string, array given in %s on line %d

Warning: transliterator_transliterate(): transliterator_transliterate: bad arguments in %s on line %d
bool(false)

Warning: transliterator_transliterate(): transliterator_transliterate: Neither "start" nor the "end" arguments can exceed the number of UTF-16 code units (in this case, 3) in %s on line %d
bool(false)

Warning: transliterator_transliterate(): transliterator_transliterate: "start" argument should be non-negative and not bigger than "end" (if defined) in %s on line %d
bool(false)

Warning: transliterator_transliterate() expects at most 4 parameters, 5 given in %s on line %d

Warning: transliterator_transliterate(): transliterator_transliterate: bad arguments in %s on line %d
bool(false)

Warning: Transliterator::transliterate() expects at least 1 parameter, 0 given in %s on line %d

Warning: Transliterator::transliterate(): transliterator_transliterate: bad arguments in %s on line %d
bool(false)

Warning: Transliterator::transliterate() expects parameter 1 to be string, array given in %s on line %d

Warning: Transliterator::transliterate(): transliterator_transliterate: bad arguments in %s on line %d
bool(false)

Warning: transliterator_transliterate(): String conversion of string to UTF-16 failed in %s on line %d
Done.
