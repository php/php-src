--TEST--
Op array fixed up multiple times during preloading
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_trait_multiple_fixup.inc
--SKIPIF--
<?php
require_once('skipif.inc');
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
(new C2)->method();
(new C4)->method();
?>
--EXPECT--
Foo
Foo
