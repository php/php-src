--TEST--
Bug #74923 Crash when crawling through network share
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");

?>
--FILE--
<?php

/* No way to affect timeout here. On different systems this might take some
	dozens of seconds to complete. */

$s = '\\\\hello.com' . str_repeat('\\', 260);

var_dump($s, @stat($s));

?>
===DONE===
--EXPECTF--
string(271) "%s"
bool(false)
===DONE===
