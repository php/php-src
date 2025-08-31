--TEST--
IntlDatePatternGenerator::getBestPattern()
--EXTENSIONS--
intl
--INI--
intl.default_locale=en_US
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '72.1') < 0) die('skip for ICU >= 72.1'); ?>
--FILE--
<?php

$dtpg = new IntlDatePatternGenerator();
$dtpg2 = new IntlDatePatternGenerator("de_DE");
$dtpg3 = IntlDatePatternGenerator::create();
$dtpg4 = IntlDatePatternGenerator::create("de_DE");

echo $dtpg->getBestPattern("jjmm"), "\n";
echo $dtpg2->getBestPattern("jjmm"), "\n";
echo $dtpg3->getBestPattern("YYYYMMMdd"), "\n";
echo $dtpg4->getBestPattern("YYYYMMMdd"), "\n";

echo $dtpg->getBestPattern(""), "\n";

?>
--EXPECT--
h:mm a
HH:mm
MMM dd, YYYY
dd. MMM YYYY
