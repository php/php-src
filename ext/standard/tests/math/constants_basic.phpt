--TEST--
Test for pre-defined math constants
--INI--
precision=14
--FILE--
<?php
echo "M_E= ";
var_dump(M_E);
echo "M_LOG2E= ";
var_dump(M_LOG2E);
echo "M_LOG10E= ";
var_dump(M_LOG10E);
echo "M_LN2= ";
var_dump(M_LN2);
echo "M_LN10= ";
var_dump(M_LN10);
echo "M_PI= ";
var_dump(M_PI);
echo "M_PI_2= ";
var_dump(M_PI_2);
echo "M_PI_4= ";
var_dump(M_PI_4);
echo "M_1_PI= ";
var_dump(M_1_PI);
echo "M_2_PI= ";
var_dump(M_2_PI);
echo "M_SQRTPI= ";
var_dump(M_SQRTPI);
echo "M_2_SQRTPI= ";
var_dump(M_2_SQRTPI);
echo "M_LNPI= ";
var_dump(M_LNPI);
echo "M_EULER= ";
var_dump(M_EULER);
echo "M_SQRT2= ";
var_dump(M_SQRT2);
echo "M_SQRT1_2= ";
var_dump(M_SQRT1_2);
echo "M_SQRT3= ";
var_dump(M_SQRT3);
echo "INF= ";
var_dump(INF);
echo "NAN= ";
var_dump(NAN);
?>
--EXPECT--
M_E= float(2.718281828459)
M_LOG2E= float(1.442695040889)
M_LOG10E= float(0.43429448190325)
M_LN2= float(0.69314718055995)
M_LN10= float(2.302585092994)
M_PI= float(3.1415926535898)
M_PI_2= float(1.5707963267949)
M_PI_4= float(0.78539816339745)
M_1_PI= float(0.31830988618379)
M_2_PI= float(0.63661977236758)
M_SQRTPI= float(1.7724538509055)
M_2_SQRTPI= float(1.1283791670955)
M_LNPI= float(1.1447298858494)
M_EULER= float(0.57721566490153)
M_SQRT2= float(1.4142135623731)
M_SQRT1_2= float(0.70710678118655)
M_SQRT3= float(1.7320508075689)
INF= float(INF)
NAN= float(NAN)
