--TEST--
IntlDatePatternGenerator::getBestPattern(): clone
--SKIPIF--
<?php
if (!extension_loaded('intl')) die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "en_US");

$dtpg = new IntlDatePatternGenerator("de_DE");
echo $dtpg->getBestPattern("YYYYMMMddjjmm"), "\n";

$dtpg_clone = clone $dtpg;
echo $dtpg_clone->getBestPattern("YYYYMMMddjjmm"), "\n";

?>
--EXPECT--
dd. MMM YYYY, HH:mm
dd. MMM YYYY, HH:mm
