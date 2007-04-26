--TEST--
Math constants
--INI--
precision=14
--FILE--
<?php
$constants = array(
    "M_E",
    "M_LOG2E",
    "M_LOG10E",
    "M_LN2",
    "M_LN10",
    "M_PI",
    "M_PI_2",
    "M_PI_4",
    "M_1_PI",
    "M_2_PI",
    "M_SQRTPI",
    "M_2_SQRTPI",
    "M_LNPI",
    "M_EULER",
    "M_SQRT2",
    "M_SQRT1_2",
    "M_SQRT3"
);
foreach($constants as $constant) {
    printf("%-10s: %s\n", $constant, constant($constant));
}
?>
--EXPECTREGEX--
M_E       : 2.718281[0-9]*
M_LOG2E   : 1.442695[0-9]*
M_LOG10E  : 0.434294[0-9]*
M_LN2     : 0.693147[0-9]*
M_LN10    : 2.302585[0-9]*
M_PI      : 3.141592[0-9]*
M_PI_2    : 1.570796[0-9]*
M_PI_4    : 0.785398[0-9]*
M_1_PI    : 0.318309[0-9]*
M_2_PI    : 0.636619[0-9]*
M_SQRTPI  : 1.772453[0-9]*
M_2_SQRTPI: 1.128379[0-9]*
M_LNPI    : 1.144729[0-9]*
M_EULER   : 0.577215[0-9]*
M_SQRT2   : 1.414213[0-9]*
M_SQRT1_2 : 0.707106[0-9]*
M_SQRT3   : 1.732050[0-9]*

