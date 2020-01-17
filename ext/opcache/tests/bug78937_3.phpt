--TEST--
Bug #78937.3 (Preloading unlinkable anonymous class can segfault)
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
?>
--EXPECTF--
Warning: Can't preload unlinked class Foo: Unknown parent Bar in %spreload_bug78937.inc on line 6

Warning: Can't preload unlinked class class@anonymous: Unknown parent Bar in %spreload_bug78937.inc on line 3

Fatal error: Uncaught Error: Class 'Bar' not found in %spreload_bug78937.inc:3
Stack trace:
#0 %sbug78937_3.php(3): foo()
#1 {main}
  thrown in %spreload_bug78937.inc on line 3
