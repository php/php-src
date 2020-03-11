--TEST--
Bug #78937.2 (Preloading unlinkable anonymous class can segfault)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_bug78937.inc
--SKIPIF--
<?php
require_once('skipif.inc');
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
include(__DIR__ . "/preload_bug78937.inc");
var_dump(foo());
var_dump(bar(false));
?>
--EXPECTF--
Loading AnonDep
Loading FooDep
Loading FooDep2
object(class@anonymous)#2 (0) {
}
object(Foo)#2 (1) {
  ["id"]=>
  int(2)
}
