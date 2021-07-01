--TEST--
Optimizer: Pass Registration
--EXTENSIONS--
opcache
zend_test
--SKIPIF--
<?php
if (getenv("SKIP_REPEAT")) {
    die("skip pass registration does not need repeat");
}
?>
--INI--
opcache.enable_cli=1
opcache.file_cache=
opcache.file_cache_only=0
zend_test.register_passes=1
--FILE--
<?php

?>
--EXPECT--
pass1
pass2
