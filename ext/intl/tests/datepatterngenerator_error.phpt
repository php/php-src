--TEST--
IntlDatePatternGenerator::getBestPattern(): errors
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$dtpg = new IntlDatePatternGenerator();
var_dump($dtpg->getBestPattern("jjmm\x80"));

?>
--EXPECTF--
Warning: IntlDatePatternGenerator::getBestPattern(): Skeleton is not a valid UTF-8 string in %s on line %d
bool(false)
