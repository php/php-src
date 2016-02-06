--TEST--
Bug #70903 scandir wrongly interprets the Turkish "ı" character
--SKIPIF--
<?php
include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

skip_if_no_required_exts();
skip_if_not_win();

?>
--FILE--
<?php

/* This file is in UTF-8. */

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

$prefix = dirname(__FILE__) . DIRECTORY_SEPARATOR . "testBug70903" . DIRECTORY_SEPARATOR;

$d0 = $prefix . "ı";

mkdir($prefix);

mkdir($d0);

get_basename_with_cp($d0, 65001);

touch("$d0\\test0.txt");

var_dump(count(stat("$d0\\ı.txt")) > 0);

unlink("$d0\\test0.txt");

rmdir($d0);

rmdir($prefix);

?>
===DONE===
--EXPECTF--
Active code page: 65001
getting basename of %s\ı
string(2) "ı"
bool(true)
string(%d) "%s\testBug70903\ı"
Active code page: %d

Warning: stat(): stat failed for C:\php-sdk\phpmaster\vc14\x64\weltling_streams_refactor3\ext\standard\tests\file\windows_mb_path\testBug70903\ı\ı.txt in C:\php-sdk\phpmaster\vc14\x64\weltling_streams_refactor3\ext\standard\tests\file\windows_mb_path\bug70903.php on line 19
bool(true)
===DONE===
