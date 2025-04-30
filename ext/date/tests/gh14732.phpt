--TEST--
GH-14732 (date_sun_info() fails for non-finite values)
--FILE--
<?php
try {
    date_sun_info(1, NAN, 1);
} catch (ValueError $ex) {
    echo $ex->getMessage(), "\n";
}
try {
    date_sun_info(1, -INF, 1);
} catch (ValueError $ex) {
    echo $ex->getMessage(), "\n";
}
try {
    date_sun_info(1, 1, NAN);
} catch (ValueError $ex) {
    echo $ex->getMessage(), "\n";
}
try {
    date_sun_info(1, 1, INF);
} catch (ValueError $ex) {
    echo $ex->getMessage(), "\n";
}
var_dump(date_sunset(1, SUNFUNCS_RET_STRING, NAN, 1));
var_dump(date_sunrise(1, SUNFUNCS_RET_STRING, 1, NAN));
?>
--EXPECTF--
date_sun_info(): Argument #2 ($latitude) must be finite
date_sun_info(): Argument #2 ($latitude) must be finite
date_sun_info(): Argument #3 ($longitude) must be finite
date_sun_info(): Argument #3 ($longitude) must be finite

Deprecated: Constant SUNFUNCS_RET_STRING is deprecated in %s on line %d

Deprecated: Function date_sunset() is deprecated since 8.1, use date_sun_info() instead in %s on line %d
bool(false)

Deprecated: Constant SUNFUNCS_RET_STRING is deprecated in %s on line %d

Deprecated: Function date_sunrise() is deprecated since 8.1, use date_sun_info() instead in %s on line %d
bool(false)
