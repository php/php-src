--TEST--
Math constants
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
    printf("%-10s: %.14s\n", $constant, constant($constant));
}
?>
--EXPECT--
M_E       : 2.718281828459
M_LOG2E   : 1.442695040889
M_LOG10E  : 0.434294481903
M_LN2     : 0.693147180559
M_LN10    : 2.302585092994
M_PI      : 3.141592653589
M_PI_2    : 1.570796326794
M_PI_4    : 0.785398163397
M_1_PI    : 0.318309886183
M_2_PI    : 0.636619772367
M_SQRTPI  : 1.772453850905
M_2_SQRTPI: 1.128379167095
M_LNPI    : 1.144729885849
M_EULER   : 0.577215664901
M_SQRT2   : 1.414213562373
M_SQRT1_2 : 0.707106781186
M_SQRT3   : 1.732050807568

