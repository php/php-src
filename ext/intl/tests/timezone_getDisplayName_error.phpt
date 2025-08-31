--TEST--
IntlTimeZone::getDisplayName(): errors
--EXTENSIONS--
intl
--FILE--
<?php

$tz = IntlTimeZone::createTimeZone('Europe/Lisbon');
var_dump($tz->getDisplayName(false, -1));
echo intl_get_error_message(), PHP_EOL;

?>
--EXPECT--
bool(false)
IntlTimeZone::getDisplayName(): wrong display type: U_ILLEGAL_ARGUMENT_ERROR
