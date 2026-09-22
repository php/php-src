--TEST--
IntlDatePatternGenerator::getBestPattern(): errors
--EXTENSIONS--
intl
--FILE--
<?php

$dtpg = new IntlDatePatternGenerator();
var_dump($dtpg->getBestPattern("jjmm\x80"));
var_dump(intl_get_error_message());

?>
--EXPECT--
bool(false)
string(102) "IntlDatePatternGenerator::getBestPattern(): Skeleton is not a valid UTF-8 string: U_INVALID_CHAR_FOUND"
