--TEST--
Initializer of overwritten property should be resolved against the correct class
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_overwritten_prop_init.inc
--SKIPIF--
<?php
require_once('skipif.inc');
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
var_dump((new Bar)->prop);
?>
--EXPECT--
int(42)
