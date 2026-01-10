--TEST--
intl.use_exceptions INI setting
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.use_exceptions", true);
$t = transliterator_create('any-hex');
try {
    var_dump($t->transliterate('a', 3));
} catch (IntlException $intlE) {
    var_dump($intlE->getMessage());
}
ini_set("intl.use_exceptions", false);
ini_set("intl.error_level", E_NOTICE);
var_dump($t->transliterate('a', 3));
?>
--EXPECTF--
string(133) "Transliterator::transliterate(): Neither "start" nor the "end" arguments can exceed the number of UTF-16 code units (in this case, 1)"

Deprecated: ini_set(): Using a value different than 0 for intl.error_level is deprecated, as the intl.error_level INI setting is deprecated. Instead the intl.use_exceptions INI setting should be enabled to throw exceptions on errors or intl_get_error_code()/intl_get_error_message() should be used to manually deal with errors in %s on line %d

Notice: Transliterator::transliterate(): Neither "start" nor the "end" arguments can exceed the number of UTF-16 code units (in this case, 1) in %s on line %d
bool(false)
