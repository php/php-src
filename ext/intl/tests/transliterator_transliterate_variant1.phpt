--TEST--
transliterator_transliterate (variant 1, non-transliterator 1st arg)
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
//exec('pause');
$str = " o";
echo transliterator_transliterate("[\p{White_Space}] hex", $str), "\n";

echo transliterator_transliterate("\x8F", $str), "\n";
echo intl_get_error_message(), "\n";

class A {
function __toString() { return "inexistent id"; }
}

echo transliterator_transliterate(new A(), $str), "\n";
echo intl_get_error_message(), "\n";

echo "Done.\n";
--EXPECTF--
\u0020o

Warning: transliterator_transliterate(): String conversion of id to UTF-16 failed in %s on line %d

Warning: transliterator_transliterate(): Could not create transliterator with ID %s

String conversion of id to UTF-16 failed: U_INVALID_CHAR_FOUND

Warning: transliterator_transliterate(): transliterator_create: unable to open ICU transliterator with id "inexistent id" in %s on line %d

Warning: transliterator_transliterate(): Could not create transliterator with ID "inexistent id" (transliterator_create: unable to open ICU transliterator with id "inexistent id": U_INVALID_ID) in %s on line %d

transliterator_create: unable to open ICU transliterator with id "inexistent id": U_INVALID_ID
Done.
