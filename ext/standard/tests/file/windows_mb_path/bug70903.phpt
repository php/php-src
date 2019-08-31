--TEST--
Bug #70903 scandir wrongly interprets the Turkish "ı" character
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();

?>
--FILE--
<?php

/* This file is in UTF-8. */

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$prefix = __DIR__ . DIRECTORY_SEPARATOR . "testBug70903" . DIRECTORY_SEPARATOR;

$d0 = $prefix . "ı";

mkdir($prefix);

mkdir($d0);

get_basename_with_cp($d0, 65001);

touch("$d0\\ı.txt");

var_dump(count(stat("$d0\\ı.txt")) > 0);

unlink("$d0\\ı.txt");

rmdir($d0);

rmdir($prefix);

?>
===DONE===
--EXPECTF--
Active code page: 65001
getting basename of %s\ı
string(2) "ı"
bool(true)
string(%d) "%s\ı"
Active code page: %d
bool(true)
===DONE===
