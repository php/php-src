--TEST--
PHP_BUILD_DATE
--FILE--
<?php
var_dump(PHP_BUILD_DATE);
?>
--EXPECTREGEX--
string\(20\) "[A-Za-z]{3} ( \d|\d{2}) \d{4} \d{2}:\d{2}:\d{2}"
