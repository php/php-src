--TEST--
GH-21052: Preloaded constant propagated to file-cached script
--CREDITS--
Grummfy
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache="{TMP}"
opcache.preload={PWD}/gh21052_a.inc
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
require __DIR__ . '/gh21052_b.inc';
?>
--EXPECT--
array(1) {
  [0]=>
  string(3) "foo"
}
array(1) {
  [0]=>
  string(3) "foo"
}
