--TEST--
IntlTimeZone::getDisplayName(): errors
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$tz = IntlTimeZone::createTimeZone('Europe/Lisbon');
var_dump($tz->getDisplayName(false, -1));

var_dump(intltz_get_display_name(null, IntlTimeZone::DISPLAY_SHORT, false, 'pt_PT'));
?>
--EXPECTF--
Warning: IntlTimeZone::getDisplayName(): wrong display type in %s on line %d
bool(false)

Fatal error: Uncaught TypeError: intltz_get_display_name(): Argument #1 ($timezone) must be of type IntlTimeZone, null given in %s:%d
Stack trace:
#0 %s(%d): intltz_get_display_name(NULL, 1, false, 'pt_PT')
#1 {main}
  thrown in %s on line %d
