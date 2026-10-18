--TEST--
file() doesn't reach hard limit
--INI--
memory_limit=64M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--FILE--
<?php
$path = __DIR__ . '/oom_file.txt';
file_put_contents($path, str_repeat("a\n", 1024 * 1024));
// Each one byte line becomes a zval plus a zend_string.
ini_set('memory_limit', '8M');
var_dump(file($path));
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/oom_file.txt");
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
