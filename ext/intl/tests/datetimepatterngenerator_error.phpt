--TEST--
IntlDateTimePatternGenerator::getBestPattern(): errors
--SKIPIF--
<?php
if (!extension_loaded('intl')) die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$dtpg = new IntlDateTimePatternGenerator();
var_dump($dtpg->getBestPattern("jjmm\x80"));

$dtpg = new IntlDateTimePatternGenerator("xxxxxxx");
var_dump($dtpg);

?>
--EXPECTF--
Warning: IntlDateTimePatternGenerator::getBestPattern(): datetimepatterngenerator_get_best_pattern: skeleton was not a valid UTF-8 string in %s on line %d
bool(false)
