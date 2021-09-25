--TEST--
Transliterator::getErrorCode (basic)
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
$t = Transliterator::create("[\p{Bidi_Mirrored}] Hex");
var_dump($t->transliterate("\x8F"));
echo transliterator_get_error_code($t), "\n";

echo $t->getErrorCode(), "\n";

var_dump($t->transliterate(""));
echo $t->getErrorCode(), "\n";

echo "Done.\n";
?>
--EXPECTF--
Warning: Transliterator::transliterate(): String conversion of string to UTF-16 failed in %s on line %d
bool(false)
10
10
string(0) ""
0
Done.
