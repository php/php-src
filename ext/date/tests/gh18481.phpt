--TEST--
GH-18481 (date_sunrise with utcOffset as INF)
--FILE--
<?php

foreach ([-NAN, NAN, INF, -INF] as $offset) {
	var_dump(date_sunrise(time(), SUNFUNCS_RET_STRING, 38.4, -9, 90, $offset));
}
?>
--EXPECTF--
Deprecated: Constant SUNFUNCS_RET_STRING is deprecated in %s on line %d

Deprecated: Function date_sunrise() is deprecated since 8.1, use date_sun_info() instead in %s on line %d
bool(false)

Deprecated: Constant SUNFUNCS_RET_STRING is deprecated in %s on line %d

Deprecated: Function date_sunrise() is deprecated since 8.1, use date_sun_info() instead in %s on line %d
bool(false)

Deprecated: Constant SUNFUNCS_RET_STRING is deprecated in %s on line %d

Deprecated: Function date_sunrise() is deprecated since 8.1, use date_sun_info() instead in %s on line %d
bool(false)

Deprecated: Constant SUNFUNCS_RET_STRING is deprecated in %s on line %d

Deprecated: Function date_sunrise() is deprecated since 8.1, use date_sun_info() instead in %s on line %d
bool(false)
