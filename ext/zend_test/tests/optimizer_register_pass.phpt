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
if (getenv("SKIP_PRELOAD")) {
    die("skip Not compatible with preload");
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
Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0
pass1
pass2
