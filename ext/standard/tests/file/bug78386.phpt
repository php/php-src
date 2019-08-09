--TEST--
Bug #78386 (fstat mode has unexpected value on PHP 7.4)
--FILE--
<?php
$handle = popen('dir', 'r');
$stat = fstat($handle);
var_dump(decoct($stat['mode']));
?>
--EXPECT--
string(5) "10666"
