--TEST--
Transliterator::getErrorMessage (basic)
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
$t = Transliterator::create("[\p{Bidi_Mirrored}] Hex");
var_dump($t->transliterate("\x8F"));
echo transliterator_get_error_message($t), "\n";

echo $t->getErrorMessage(), "\n";

var_dump($t->transliterate(""));
echo $t->getErrorMessage(), "\n";

echo "Done.\n";
--EXPECTF--
Warning: Transliterator::transliterate(): String conversion of string to UTF-16 failed in %s on line %d
bool(false)
String conversion of string to UTF-16 failed: U_INVALID_CHAR_FOUND
String conversion of string to UTF-16 failed: U_INVALID_CHAR_FOUND
string(0) ""
U_ZERO_ERROR
Done.
