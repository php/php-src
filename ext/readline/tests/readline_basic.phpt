--TEST--
readline(): Basic test
--CREDITS--
Milwaukee PHP User Group
#PHPTestFest2017
--EXTENSIONS--
readline
--FILE--
<?php

var_dump(readline('Enter some text:'));

?>
--STDIN--
I love PHP
--EXPECTF--
%Astring(10) "I love PHP"
