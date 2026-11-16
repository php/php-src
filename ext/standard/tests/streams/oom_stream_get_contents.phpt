--TEST--
stream_get_contents() doesn't reach hard limit
--INI--
memory_limit=64M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--FILE--
<?php
$path = __DIR__ . '/oom_stream_get_contents.txt';
file_put_contents($path, str_repeat('x', 12 * 1024 * 1024));
ini_set('memory_limit', '4M');
var_dump(stream_get_contents(fopen($path, 'rb')));
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/oom_stream_get_contents.txt");
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
