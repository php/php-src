--TEST--
Bug #66960 phar long filename crash
--EXTENSIONS--
phar
--INI--
phar.readonly = 0
memory_limit = -1
--FILE--
<?php
$file = __DIR__ . DIRECTORY_SEPARATOR . 'bug66960.phar';
$phar = new Phar($file);
var_dump(file_exists("phar://$file/". str_repeat('a', PHP_MAXPATHLEN-1)));
var_dump(file_exists("phar://$file/". str_repeat('a', PHP_MAXPATHLEN)));
var_dump(file_exists("phar://$file/". str_repeat('a', PHP_MAXPATHLEN+1)));
echo 'done';
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
done
