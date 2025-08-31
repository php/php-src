--TEST--
IntlTimeZone::getCanonicalID(): second argument
--EXTENSIONS--
intl
--FILE--
<?php

var_dump(IntlTimeZone::getCanonicalID('Portugal', $isSystemId));
echo intl_get_error_message(), PHP_EOL;
var_dump($isSystemId);

/* A valid custom time zone ID has the following syntax: GMT[+|-]hh[[:]mm] */
var_dump(IntlTimeZone::getCanonicalID('GMT +01:25', $isSystemId));
echo intl_get_error_message(), PHP_EOL;
var_dump($isSystemId);

?>
--EXPECT--
string(13) "Europe/Lisbon"
U_ZERO_ERROR
bool(true)
bool(false)
IntlTimeZone::getCanonicalID(): error obtaining canonical ID: U_ILLEGAL_ARGUMENT_ERROR
bool(true)
