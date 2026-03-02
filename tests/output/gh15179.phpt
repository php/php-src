--TEST--
GH-15179 (Segmentation fault (null pointer dereference) in ext/standard/url_scanner_ex.re)
--CREDITS--
YuanchengJiang
--INI--
memory_limit=64M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip requires ZendMM");
?>
--FILE--
<?php
$var = str_repeat('a', 20 * 1024 * 1024);

output_add_rewrite_var($var, $var);
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %s
