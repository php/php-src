--TEST--
GH-18481 (date_sunrise with utcOffset as INF)
--FILE--
<?php

foreach ([-NAN, NAN, INF, -INF] as $offset) {
	var_dump(date_sunrise(time(), SUNFUNCS_RET_STRING, 38.4, -9, 90, $offset));
}
?>
--EXPECTF--
Deprecated: Function date_sunrise() is deprecated in %s on line %d
bool(false)

Deprecated: Function date_sunrise() is deprecated in %s on line %d
bool(false)

Deprecated: Function date_sunrise() is deprecated in %s on line %d
bool(false)

Deprecated: Function date_sunrise() is deprecated in %s on line %d
bool(false)
