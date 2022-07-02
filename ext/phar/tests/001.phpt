--TEST--
Phar::apiVersion
--EXTENSIONS--
phar
--FILE--
<?php
var_dump(Phar::apiVersion());
?>
--EXPECT--
string(5) "1.1.1"
