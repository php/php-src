--TEST--
IntlDatePatternGenerator::getBestPattern(): clone
--EXTENSIONS--
intl
--FILE--
<?php

$dtpg = new IntlDatePatternGenerator("de_DE");
echo $dtpg->getBestPattern("YYYYMMMddjjmm"), "\n";

$dtpg_clone = clone $dtpg;
echo $dtpg_clone->getBestPattern("YYYYMMMddjjmm"), "\n";

?>
--EXPECT--
dd. MMM YYYY, HH:mm
dd. MMM YYYY, HH:mm
