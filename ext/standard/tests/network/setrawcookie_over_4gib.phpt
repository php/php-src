--TEST--
setrawcookie() checks the whole Set-Cookie line when it is longer than 4 GiB
--SKIPIF--
<?php
if (!getenv('RUN_RESOURCE_HEAVY_TESTS')) die('skip resource-heavy test');
if (PHP_INT_SIZE != 8) die('skip this test is for 64bit platforms only');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow tests excluded by request');
?>
--INI--
memory_limit=-1
--FILE--
<?php
// Cookie value validation stops at the first NUL byte, so it can sit past the first 4 GiB.
var_dump(setrawcookie('name', str_repeat('a', 2**32) . "\0"));
?>
--EXPECTF--
Warning: Header may not contain NUL bytes in %s on line %d
bool(false)
