--TEST--
GH-15973 (Segmentation fault in JIT mode 1135)
--EXTENSIONS--
opcache
--INI--
opcache.jit=1215
opcache.jit_buffer_size=64M
--FILE--
<?php
class Test {
}
$appendProp2 = (function() {
$this->prop[] = 1;
})->bindTo($test, Test::class);
$appendProp2();
?>
--EXPECTF--
Warning: Undefined variable $test in %sgh15973.php on line 6

Fatal error: Uncaught Error: Using $this when not in object context in %sgh15973.php:5
Stack trace:
#0 %sgh15973.php(7): Test::{closure:%s}()
#1 {main}
  thrown in %sgh15973.php on line 5
