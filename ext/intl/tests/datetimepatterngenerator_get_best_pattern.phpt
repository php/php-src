--TEST--
IntlDateTimePatternGenerator::getBestPattern()
--SKIPIF--
<?php
if (!extension_loaded('intl'))	die('skip intl extension not enabled'); ?>
--FILE--
<?php

ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "en_US");

$dtpg = new IntlDateTimePatternGenerator();
$dtpg2 = new IntlDateTimePatternGenerator("de_DE");
$dtpg3 = IntlDateTimePatternGenerator::create();
$dtpg4 = IntlDateTimePatternGenerator::create("de_DE");

echo $dtpg->getBestPattern("jjmm"), "\n";
echo $dtpg2->getBestPattern("jjmm"), "\n";
echo $dtpg3->getBestPattern("YYYYMMMdd"), "\n";
echo $dtpg4->getBestPattern("YYYYMMMdd"), "\n";

echo $dtpg->getBestPattern(""), "\n";

echo datetimepatterngenerator_get_best_pattern(datetimepatterngenerator_create(), "YYYYMMMdd"), "\n";

try {
    $dtpg->getBestPattern();
} catch(\ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
h:mm a
HH:mm
MMM dd, YYYY
dd. MMM YYYY

MMM dd, YYYY
IntlDateTimePatternGenerator::getBestPattern() expects exactly 1 argument, 0 given
