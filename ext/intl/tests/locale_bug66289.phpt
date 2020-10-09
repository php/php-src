--TEST--
Bug #66289 Locale::lookup incorrectly returns en or en_US if locale is empty
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

ini_set("intl.default_locale", "en-US");
$availableLocales = array('fr_FR', 'de', 'es_ES', 'es_419', 'en_US');
var_dump(locale_lookup($availableLocales, false, true, 'fr_FR'));
var_dump(locale_lookup($availableLocales, false, true, null));

$availableLocales = array('fr_FR', 'de', 'es_ES', 'es_419');
var_dump(locale_lookup($availableLocales, false, true, 'fr_FR'));

ini_set("intl.default_locale", "de-DE");
$availableLocales = array(Locale::getDefault());
var_dump(locale_lookup($availableLocales, false, true));

?>
--EXPECT--
string(5) "fr_fr"
string(5) "en_us"
string(5) "fr_fr"
string(5) "de_de"
