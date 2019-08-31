--TEST--
Transliterator::transliterate (error)
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

ini_set("intl.error_level", E_WARNING);

$tr = Transliterator::create("latin");

//Arguments
var_dump(transliterator_transliterate($tr,"str",7));
var_dump(transliterator_transliterate($tr,"str",7,6));

//bad UTF-8
transliterator_transliterate($tr, "\x80\x03");

echo "Done.\n";
--EXPECTF--
Warning: transliterator_transliterate(): transliterator_transliterate: Neither "start" nor the "end" arguments can exceed the number of UTF-16 code units (in this case, 3) in %s on line %d
bool(false)

Warning: transliterator_transliterate(): transliterator_transliterate: "start" argument should be non-negative and not bigger than "end" (if defined) in %s on line %d
bool(false)

Warning: transliterator_transliterate(): String conversion of string to UTF-16 failed in %s on line %d
Done.
