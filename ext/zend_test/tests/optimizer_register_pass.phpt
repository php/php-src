--TEST--
Optimizer: Pass Registration
--EXTENSIONS--
opcache
--INI--
opcache.enable_cli=1
zend_test.register_passes=1
--FILE--
<?php

?>
--EXPECT--
pass1
pass2
