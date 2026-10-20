--TEST--
Transliterator::getErrorMessage() and Transliterator::getErrorCode()(basic)
--EXTENSIONS--
intl
--FILE--
<?php
$t = Transliterator::create("[\p{Bidi_Mirrored}] Hex");
var_dump($t->transliterate("\x8F"));

echo transliterator_get_error_code($t), ': ', transliterator_get_error_message($t), "\n";

echo $t->getErrorCode(), ': ', $t->getErrorMessage(), "\n";

var_dump($t->transliterate(""));
echo $t->getErrorCode(), ': ', $t->getErrorMessage(), "\n";

?>
--EXPECT--
bool(false)
10: Transliterator::transliterate(): String conversion of string to UTF-16 failed: U_INVALID_CHAR_FOUND
10: Transliterator::transliterate(): String conversion of string to UTF-16 failed: U_INVALID_CHAR_FOUND
string(0) ""
0: U_ZERO_ERROR
