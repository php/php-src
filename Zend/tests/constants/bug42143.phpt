--TEST--
bug #42143 (The constant NAN is reported as 0 on Windows build)
--CREDITS--
Venkat Raman Don
--FILE--
<?php
echo "Testing NAN:\n";
echo "NAN= ";
var_dump(NAN);
var_dump(tan(-1) == 123);
var_dump(cos(-100) == "PHP String");
var_dump(deg2rad(-5.6) == null);
var_dump(sqrt(-0.1) == false);
var_dump(sqrt(cos(M_PI)) == 0.1);
var_dump(NAN);
var_dump(is_nan(sqrt(-1.005)) == false);
var_dump(is_nan(floor(1)) == true);
var_dump(log10(-1) == log(-1));
var_dump(log10(-1) != log10(-1));
var_dump(is_finite(log10(-1)) == false);
var_dump(NAN == NAN);
?>
--EXPECT--
Testing NAN:
NAN= float(NAN)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
float(NAN)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
