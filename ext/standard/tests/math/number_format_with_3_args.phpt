--TEST--
number_format should use default thousands seperator when 3 arguments are used
--FILE--
<?php

$number = 2020.1415;

var_dump(number_format($number, 2, 'F'));

?>
--EXPECT--
string(8) "2,020F14"
