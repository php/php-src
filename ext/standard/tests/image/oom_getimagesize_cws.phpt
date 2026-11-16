--TEST--
getimagesize() decompression doesn't reach hard limit
--INI--
memory_limit=128M
--SKIPIF--
<?php
$php = getenv('TEST_PHP_EXECUTABLE');

/* CWS support in ext/standard is only enabled when zlib is built in. */
$process = proc_open([$php, '-n', '-r', 'exit(extension_loaded("zlib") ? 0 : 1);'], [STDIN, STDOUT, STDERR], $pipes);
if (proc_close($process) !== 0) {
    die('skip built-in zlib not available');
}
?>
--FILE--
<?php
require __DIR__ . '/oom_getimagesize_cws.inc';
$path = __DIR__ . '/oom_getimagesize_cws.swf';
generate_file($path);
getimagesize($path);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/oom_getimagesize_cws.swf');
?>
--EXPECTF--
Fatal error: Allowed memory size of 134217728 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
