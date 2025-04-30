--TEST--
Bug #72666 (stat cache clearing inconsistent - chgrp, chmod)
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die('skip no windows support');
if (!function_exists("posix_getgid")) die("skip no posix_getgid()");
?>
--FILE--
<?php
$filename = __DIR__ . '/bug72666_variation2.txt';

$gid = posix_getgid();

var_dump(touch($filename));
$ctime1 = filectime($filename);
sleep(1);
var_dump(chgrp($filename,$gid));
$ctime2 = filectime($filename);
sleep(1);
var_dump(chmod($filename, 0777));
$ctime3 = filectime($filename);

var_dump($ctime2 > $ctime1);
var_dump($ctime3 > $ctime2);
?>
--CLEAN--
<?php
unlink(__DIR__ . '/bug72666_variation2.txt');
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
