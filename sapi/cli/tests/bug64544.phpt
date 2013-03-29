--TEST--
Bug #64544 (Valgrind warnings after using putenv)
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == "WIN") {
	die("skip non windows test");
}
?>
--FILE--
<?php

putenv("HOME=/tmp");
var_dump(getenv("HOME"));

putenv("FOO=BAR");
var_dump(getenv("FOO"));
?>
--EXPECTF--
string(4) "/tmp"
string(3) "BAR"
