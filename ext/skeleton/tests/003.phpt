--TEST--
%EXTNAME%_test2() Basic test
--EXTENSIONS--
%EXTNAME%
--FILE--
<?php
var_dump(%EXTNAME%_test2());
var_dump(%EXTNAME%_test2('PHP'));
?>
--EXPECT--
string(11) "Hello World"
string(9) "Hello PHP"
