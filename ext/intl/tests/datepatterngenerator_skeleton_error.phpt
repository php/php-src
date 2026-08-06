--TEST--
IntlDatePatternGenerator::getSkeleton() and getBaseSkeleton(): errors
--EXTENSIONS--
intl
--FILE--
<?php

$dtpg = new IntlDatePatternGenerator();

foreach (["getSkeleton", "getBaseSkeleton"] as $method) {
    var_dump($dtpg->$method("dd/MMM\x80"));
    echo intl_get_error_message(), "\n";
}

?>
--EXPECT--
bool(false)
IntlDatePatternGenerator::getSkeleton(): Pattern is not a valid UTF-8 string: U_INVALID_CHAR_FOUND
bool(false)
IntlDatePatternGenerator::getBaseSkeleton(): Pattern is not a valid UTF-8 string: U_INVALID_CHAR_FOUND
