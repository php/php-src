--TEST--
overflow check for _php_math_basetozval
--INI--
precision=14
--FILE--
<?php

var_dump(hexdec("012345"));
var_dump(hexdec("12345"));
var_dump(hexdec("q12345"));
var_dump(hexdec("12345+?!"));
var_dump(hexdec("12345q"));
var_dump(hexdec("12345678901234567"));
var_dump(hexdec("17fffffffffffffff"));

?>
--EXPECTF--
int(74565)
int(74565)

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(74565)

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(74565)

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(74565)

Notice: Input number exceeds maximum integer value, precision has been lost in conversion in %s on line %d
float(2.0988295476557332E+19)

Notice: Input number exceeds maximum integer value, precision has been lost in conversion in %s on line %d
float(2.7670116110564327E+19)
