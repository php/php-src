--TEST--
Bug #54028 realpath(".") return false
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

$prefix = __DIR__ . DIRECTORY_SEPARATOR . "testBug54028_2" . DIRECTORY_SEPARATOR;

$dirs = array("a", "ソ", "ゾ", "şŞıİğĞ", "多国語", "王", "汚れて掘る");

mkdir($prefix);
foreach ($dirs as $d) {
	mkdir($prefix . $d);
}

$old_cwd = getcwd();
foreach ($dirs as $d) {
	$now = $prefix . $d;
	var_dump(chdir($now));
	var_dump($dn = realpath("."));
	var_dump($d == get_basename_with_cp($dn, 65001, false));
}
chdir($old_cwd);

foreach ($dirs as $d) {
	rmdir($prefix . $d);
}
rmdir($prefix);

?>
===DONE===
--EXPECTF--
bool(true)
string(%d) "%sa"
bool(true)
bool(true)
string(%d) "%sソ"
bool(true)
bool(true)
string(%d) "%sゾ"
bool(true)
bool(true)
string(%d) "%sşŞıİğĞ"
bool(true)
bool(true)
string(%d) "%s多国語"
bool(true)
bool(true)
string(%d) "%s王"
bool(true)
bool(true)
string(%d) "%s汚れて掘る"
bool(true)
===DONE===
