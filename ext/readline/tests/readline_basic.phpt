--TEST--
readline(): Basic test
--CREDITS--
Milwaukee PHP User Group
#PHPTestFest2017
--SKIPIF--
<?php if (!extension_loaded("readline")) die("skip"); ?>
--FILE--
<?php

var_dump(readline('Enter some text:'));

?>
--STDIN--
I love PHP
--EXPECTF--
Enter some text:I love PHP
string(10) "I love PHP"
