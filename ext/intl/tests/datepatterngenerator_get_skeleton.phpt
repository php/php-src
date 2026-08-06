--TEST--
IntlDatePatternGenerator::getSkeleton() and getBaseSkeleton()
--EXTENSIONS--
intl
--FILE--
<?php

$dtpg = new IntlDatePatternGenerator();

var_dump($dtpg->getSkeleton("dd/MMM"));
var_dump($dtpg->getSkeleton("MMM-dd"));
var_dump($dtpg->getBaseSkeleton("dd/MMM"));
var_dump($dtpg->getBaseSkeleton("MMM-dd"));
var_dump($dtpg->getSkeleton(""));
var_dump($dtpg->getBaseSkeleton(""));

?>
--EXPECT--
string(5) "MMMdd"
string(5) "MMMdd"
string(4) "MMMd"
string(4) "MMMd"
string(0) ""
string(0) ""
