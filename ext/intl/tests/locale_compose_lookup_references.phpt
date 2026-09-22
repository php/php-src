--TEST--
locale_compose()/locale_lookup() with values as references.
--EXTENSIONS--
intl
--FILE--
<?php

$en = 'en';
$data = [Locale::LANG_TAG => 'en', Locale::REGION_TAG => &$en];

var_dump(locale_compose($data));

$data = [
	'language' => 'de', 
	'script' => 'Hans',
	'region' => 'DE',
	'variant2' => 'fr',
	'variant1' => &$en,
	'private1' => 'private1',
	'private2' => 'private2',
	];
var_dump(locale_compose($data));
$data = ['de', &$en];
var_dump(locale_lookup($data, "en", false, "en"));
?>
--EXPECT--
string(5) "en_en"
string(36) "de_Hans_DE_en_fr_x_private1_private2"
string(2) "en"
