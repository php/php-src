--TEST--
Bug GH-8065: Opcache zend_class_entry.inheritance_cache breaks persistent script checksum
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.consistency_checks=1
opcache.log_verbosity_level=2
opcache.protect_memory=1
--EXTENSIONS--
opcache
--SKIPIF--
<?php if (isset(opcache_get_status()["jit"])) die('skip: JIT breaks checksum due to modification of op handlers'); ?>
--FILE--
<?php

class Foo {}

require_once __DIR__ . '/gh8065.inc';

echo "Done\n";

?>
--EXPECT--
Done
