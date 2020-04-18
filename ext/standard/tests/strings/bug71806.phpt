--TEST--
Bug #71806 (php_strip_whitespace() fails on some numerical values)
--FILE--
<?php

echo php_strip_whitespace(__DIR__ . '/bug71806.data');

?>
--EXPECT--
<?php
 echo 098 ;
