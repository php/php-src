--TEST--
overflow check for _php_math_basetozval
--FILE--
<?php

var_dump(hexdec("012345"));
var_dump(hexdec("12345"));
var_dump(hexdec("q12345"));
var_dump(hexdec("12345+?!"));
var_dump(hexdec("12345q"));
var_dump(hexdec("1234500001"));
var_dump(hexdec("17fffffff"));

?>
--EXPECT--
int(74565)
int(74565)
int(74565)
int(74565)
int(74565)
float(78187069441)
float(6442450943)
