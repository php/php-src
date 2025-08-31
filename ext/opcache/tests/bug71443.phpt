--TEST--
Bug #71443 (Segfault using built-in webserver with intl using symfony)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache="{TMP}"
opcache.file_cache_only=1
--EXTENSIONS--
opcache
--SKIPIF--
<?php if (substr(PHP_OS, 0, 3) == 'WIN') die('skip..  not for Windows'); ?>
--FILE--
<?php
ini_set("include_path", "/tmp");
?>
okey
--EXPECT--
okey
