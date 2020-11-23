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

try {
    transliterator_transliterate($tr,"str",7,6);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

//bad UTF-8
transliterator_transliterate($tr, "\x80\x03");

echo "Done.\n";
?>
--EXPECTF--
Warning: transliterator_transliterate(): transliterator_transliterate: Neither "start" nor the "end" arguments can exceed the number of UTF-16 code units (in this case, 3) in %s on line %d
bool(false)
transliterator_transliterate(): Argument #2 ($string) must be less than or equal to argument #3 ($end)

Warning: transliterator_transliterate(): String conversion of string to UTF-16 failed in %s on line %d
Done.
