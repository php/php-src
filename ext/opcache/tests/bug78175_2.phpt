--TEST--
Bug #78175.2 (Preloading segfaults at preload time and at runtime)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_bug78175_2.inc
--SKIPIF--
<?php
require_once('skipif.inc');
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
var_dump(get_class(Loader::getLoader()));
var_dump(Loader::getCounter());
?>
OK
--EXPECT--
string(6) "Loader"
int(0)
OK
