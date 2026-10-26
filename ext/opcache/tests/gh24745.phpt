--TEST--
GH-24745: File cache loses attributes on constants on 32-bit builds
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache="{TMP}"
opcache.file_cache_only=1
--FILE--
<?php
require __DIR__ . '/gh24745.inc';

var_dump((new ReflectionConstant('FOO'))->getAttributes()[0]->getArguments());
?>
--EXPECT--
array(1) {
  ["message"]=>
  string(3) "bar"
}
