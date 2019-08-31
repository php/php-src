--TEST--
Bug #66461 (PHP crashes if opcache.interned_strings_buffer=0)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.log_verbosity_level=1
opcache.file_update_protection=0
opcache.interned_strings_buffer=0
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
echo "ok\n";
--EXPECT--
ok
