--TEST--
Bug #35740 (memory leak when including a directory)
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die("skip Not for Windows");
?>
--FILE--
<?php

include (__DIR__);

echo "Done\n";
?>
--EXPECTF--
Notice: include(): Read of %s bytes failed with errno=21 Is a directory in %s on line %d

Warning: include(): Failed opening '%s' for inclusion (include_path='%s') in %s on line %d
Done
