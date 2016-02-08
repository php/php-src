--TEST--
Bug #61315 stat() fails with specific DBCS characters
--SKIPIF--
<?php
include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();

?>
--FILE--
<?php

/* This file is in UTF-8. */

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

$prefix = dirname(__FILE__) . DIRECTORY_SEPARATOR . "testBug61315" . DIRECTORY_SEPARATOR;

$d0 = $prefix . "ソフト";
$d1 = $prefix . "フォルダ";

mkdir($prefix);

mkdir($d0);
mkdir($d1);

get_basename_with_cp($d0, 65001);
get_basename_with_cp($d1, 65001);

touch("$d0\\test0.txt");
touch("$d1\\test1.txt");

var_dump(count(stat("$d0\\test0.txt")) > 0);
var_dump(count(stat("$d0\\test0.txt")) > 0);

unlink("$d0\\test0.txt");
unlink("$d1\\test1.txt");

rmdir($d0);
rmdir($d1);

rmdir($prefix);

?>
===DONE===
--EXPECTF--
Active code page: 65001
getting basename of %s\ソフト
string(9) "ソフト"
bool(true)
string(%d) "%s\ソフト"
Active code page: %d
Active code page: 65001
getting basename of %s\フォルダ
string(12) "フォルダ"
bool(true)
string(%d) "%s\フォルダ"
Active code page: %d
bool(true)
bool(true)
===DONE===
