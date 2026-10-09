--TEST--
chown() and lchown() with a user name
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die('skip no windows support');
if (!function_exists("posix_getpwuid")) die("skip no posix_getpwuid()");
if (posix_getpwuid(posix_getuid()) === false) die("skip current user has no passwd entry");
?>
--FILE--
<?php
$filename = __DIR__ . DIRECTORY_SEPARATOR . 'chown_user_name.txt';
$name = posix_getpwuid(posix_getuid())['name'];

var_dump(touch($filename));
var_dump(chown($filename, $name));
var_dump(lchown($filename, $name));
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'chown_user_name.txt');
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
