--TEST--
Processes spawned while preloading do not inherit the signals blocked by the SHM lock
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_signal_mask.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Linux') die('skip Linux only');
?>
--FILE--
<?php
var_dump(file_get_contents(__DIR__ . '/preload_signal_mask.txt') === preload_child_sigblk());
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/preload_signal_mask.txt');
?>
--EXPECT--
bool(true)
