--TEST--
ResourceBundle constructor bundle accepts NULL for first two arguments
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
if (version_compare(INTL_ICU_VERSION, '4.8') < 0)
	die('skip ICU >= 4.8 only');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$r = new ResourceBundle('en_US', NULL);
$c = $r->get('calendar')->get('gregorian')->get('DateTimePatterns')->get(0);
var_dump($c);

ini_set('intl.default_locale', 'pt_PT');
$r = new ResourceBundle(NULL, NULL);
$c = $r->get('calendar')->get('gregorian')->get('DateTimePatterns')->get(0);
var_dump($c);
?>
==DONE==
--EXPECT--
string(14) "h:mm:ss a zzzz"
string(12) "H:mm:ss zzzz"
==DONE==
