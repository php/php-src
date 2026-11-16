--TEST--
stream_copy_to_stream() into php://memory doesn't reach hard limit
--INI--
memory_limit=64M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--FILE--
<?php
$path = __DIR__ . '/oom_stream_copy_to_stream_memory.txt';
file_put_contents($path, str_repeat('x', 12 * 1024 * 1024));
$dest = fopen('php://memory', 'r+');
ini_set('memory_limit', '4M');
var_dump(stream_copy_to_stream(fopen($path, 'rb'), $dest));
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/oom_stream_copy_to_stream_memory.txt");
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
