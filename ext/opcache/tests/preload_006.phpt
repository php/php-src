--TEST--
Handling of errors during linking
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_inheritance_error_ind.inc
--SKIPIF--
<?php
require_once('skipif.inc');
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
if (getenv('SKIP_ASAN')) die('xfail Startup failure leak');
?>
--FILE--
<?php
echo "Foobar\n";
?>
--EXPECTF--
Fatal error: Declaration of B::foo($bar) must be compatible with A::foo() in %spreload_inheritance_error.inc on line 8
