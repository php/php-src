--TEST--
GH-16577 (EG(strtod_state).freelist leaks with opcache.preload)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/gh16577.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
echo "Done\n";
?>
--EXPECT--
float(1.5)
Done
