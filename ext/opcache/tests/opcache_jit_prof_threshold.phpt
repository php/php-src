--TEST--
opcache_get_configuration() properly reports jit_prof_threshold
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (!isset(opcache_get_configuration()["directives"]["opcache.jit_prof_threshold"])) die("skip no JIT");
?>
--FILE--
<?php
$expected = 1 / 128; // needs to be exactly representable as IEEE double
ini_set("opcache.jit_prof_threshold", $expected);
$actual = opcache_get_configuration()["directives"]["opcache.jit_prof_threshold"];
var_dump($actual);
var_dump($actual === $expected);
?>
--EXPECTF--
float(0.0078125)
bool(true)
