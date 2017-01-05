--TEST--
Fixed bug #71537 (PCRE segfault from Opcache)
--FILE--
<?php

var_dump(preg_replace(array("/Monkey/"), array(2016), "Happy Year of Monkey"));
?>
--EXPECT--
string(18) "Happy Year of 2016"
