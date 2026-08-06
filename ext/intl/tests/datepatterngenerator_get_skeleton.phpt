--TEST--
IntlDatePatternGenerator::getSkeleton() and getBaseSkeleton()
--EXTENSIONS--
intl
--FILE--
<?php

var_dump(IntlDatePatternGenerator::getSkeleton("dd/MMM"));
var_dump(IntlDatePatternGenerator::getSkeleton("MMM-dd"));
var_dump(IntlDatePatternGenerator::getBaseSkeleton("dd/MMM"));
var_dump(IntlDatePatternGenerator::getBaseSkeleton("MMM-dd"));
var_dump(IntlDatePatternGenerator::getSkeleton(""));
var_dump(IntlDatePatternGenerator::getBaseSkeleton(""));

?>
--EXPECT--
string(5) "MMMdd"
string(5) "MMMdd"
string(4) "MMMd"
string(4) "MMMd"
string(0) ""
string(0) ""
