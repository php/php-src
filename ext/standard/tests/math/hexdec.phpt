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
var_dump((float)hexdec("1234500001"));
var_dump((float)hexdec("17fffffff"));

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
float(78187069441)
float(6442450943)
