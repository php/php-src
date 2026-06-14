--TEST--
Bug #74923 Crash when crawling through network share
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip windows only test');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php

/* No way to affect timeout here. On different systems this might take some
    dozens of seconds to complete. */

$s = '\\\\hello.com' . str_repeat('\\', 260);

var_dump($s, @stat($s));

?>
--EXPECTF--
string(271) "%s"
bool(false)
