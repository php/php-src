--TEST--
Test zend_version() function
--FILE--
<?php
var_dump(zend_version());
?>
--EXPECTREGEX--
string\(\d+\) "\d\.\d+\.\d+.*"
