--TEST--
IntlTimeZone::getIDForWindowsID() errors
--EXTENSIONS--
intl
--FILE--
<?php

var_dump(IntlTimeZone::getIDForWindowsID('Gnomeregan', null));
var_dump(intl_get_error_message());

?>
--EXPECT--
bool(false)
string(50) "unknown windows timezone: U_ILLEGAL_ARGUMENT_ERROR"
