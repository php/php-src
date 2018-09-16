--TEST--
Bug #54977 UTF-8 files and folder are not shown
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

$prefix = dirname(__FILE__) . DIRECTORY_SEPARATOR . "testBug54977" . DIRECTORY_SEPARATOR;

$paths = array("多国語", "王", "汚れて掘る");

mkdir($prefix);
foreach ($paths as $d) {
	mkdir($prefix . $d);
	file_put_contents($prefix . $d . ".test", $d);
}

$myDirectory = opendir($prefix);
while($entryName = readdir($myDirectory)) {
	echo get_basename_with_cp($prefix . $entryName, 65001, false) . "\n";
}
closedir($myDirectory);

foreach ($paths as $d) {
	rmdir($prefix . $d);
	unlink($prefix . $d . ".test");
}
rmdir($prefix);

?>
===DONE===
--EXPECT--
testBug54977
windows_mb_path
多国語
多国語.test
汚れて掘る
汚れて掘る.test
王
王.test
===DONE===
