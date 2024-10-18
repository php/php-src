--TEST--
GH-16454 (Unhandled INF in date_sunset() with tiny $utcOffset)
--FILE--
<?php
var_dump(date_sunrise(0, SUNFUNCS_RET_STRING, 61, -150, 90, PHP_FLOAT_MAX));
var_dump(date_sunrise(0, SUNFUNCS_RET_STRING, 61, -150, 90, -PHP_FLOAT_MAX));
var_dump(date_sunset(0, SUNFUNCS_RET_STRING, 61, -150, 90, PHP_FLOAT_MAX));
var_dump(date_sunset(0, SUNFUNCS_RET_STRING, 61, -150, 90, -PHP_FLOAT_MAX));
?>
--EXPECTF--
Deprecated: Constant SUNFUNCS_RET_STRING is deprecated in %s on line %d

Deprecated: Function date_sunrise() is deprecated since 8.1, use date_sun_info() instead in %s on line %d
bool(false)

Deprecated: Constant SUNFUNCS_RET_STRING is deprecated in %s on line %d

Deprecated: Function date_sunrise() is deprecated since 8.1, use date_sun_info() instead in %s on line %d
bool(false)

Deprecated: Constant SUNFUNCS_RET_STRING is deprecated in %s on line %d

Deprecated: Function date_sunset() is deprecated since 8.1, use date_sun_info() instead in %s on line %d
bool(false)

Deprecated: Constant SUNFUNCS_RET_STRING is deprecated in %s on line %d

Deprecated: Function date_sunset() is deprecated since 8.1, use date_sun_info() instead in %s on line %d
bool(false)
