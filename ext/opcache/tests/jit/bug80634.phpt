--TEST--
Bug #80634 (write_property handler of internal classes is skipped on preloaded JITted code)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
opcache.jit=function
opcache.preload={PWD}/preload_bug80634.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
$v = new SomeClass(5);
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot modify readonly property DatePeriod::$current in %spreload_bug80634.inc:7
Stack trace:
#0 %sbug80634.php(2): SomeClass->__construct(5)
#1 {main}
  thrown in %spreload_bug80634.inc on line 7
