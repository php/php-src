--TEST--
number_format should use default values when passed null
--FILE--
<?php

$number = 2020.1415;

var_dump(number_format($number, 2, null, 'T'));
var_dump(number_format($number, 2, 'F', null));

?>
--EXPECT--
string(8) "2T020.14"
string(8) "2,020F14"
