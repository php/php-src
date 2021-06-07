--TEST--
Optimizer: Pass Registration
--SKIPIF--
<?php
if (getenv("SKIP_REPEAT")) {
    die("skip pass registration does not need repeat");
}
?>
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
