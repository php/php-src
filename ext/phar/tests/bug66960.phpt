--TEST--
Bug #66960 phar long filename crash
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly = 0
memory_limit = -1
--FILE--
<?php
$file = __DIR__ . DIRECTORY_SEPARATOR . 'bug66960.phar';
$phar = new Phar($file);
assert(false === file_exists("phar://$file/". str_repeat('a', PHP_MAXPATHLEN-1)));
assert(false === file_exists("phar://$file/". str_repeat('a', PHP_MAXPATHLEN)));
assert(false === file_exists("phar://$file/". str_repeat('a', PHP_MAXPATHLEN+1)));
echo 'done';
--EXPECTF--
done
