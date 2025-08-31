--TEST--
GH-17275 Incorrect result of bcdiv function
--EXTENSIONS--
bcmath
--FILE--
<?php
var_dump(
    bcdiv('0.03772321', '9650.0', 8),
    bcdiv('0.03772321', '9650.0', 9),
);
?>
--EXPECT--
string(10) "0.00000390"
string(11) "0.000003909"
