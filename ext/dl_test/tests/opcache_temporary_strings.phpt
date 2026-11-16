--TEST--
dl() under opcache: refcounting names declared by a temporary module
--SKIPIF--
<?php include __DIR__ . "/skip.inc"; ?>
--EXTENSIONS--
opcache
--INI--
enable_dl=1
opcache.enable_cli=1
--FILE--
<?php
dl(PHP_OS_FAMILY === 'Windows' ? 'php_dl_test.dll' : 'dl_test.so');
include __DIR__ . '/opcache_temporary_strings.inc';
?>
--EXPECT--
TypeError: dl_test_test2(): Argument #1 ($str) must be of type string, array given
TypeError: DlTest::test(): Argument #1 ($str) must be of type string, array given
string(13) "dl_test_test2"
array(1) {
  ["temporary"]=>
  int(0)
}
