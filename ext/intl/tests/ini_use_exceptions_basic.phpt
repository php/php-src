--TEST--
intl.use_exceptions INI setting
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
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
--EXPECTF--
string(130) "transliterator_transliterate: Neither "start" nor the "end" arguments can exceed the number of UTF-16 code units (in this case, 1)"

Notice: Transliterator::transliterate(): transliterator_transliterate: Neither "start" nor the "end" arguments can exceed the number of UTF-16 code units (in this case, 1) in %s on line %d
bool(false)
