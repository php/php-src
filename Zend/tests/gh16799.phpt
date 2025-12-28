--TEST--
GH-16799 (Assertion failure at Zend/zend_vm_execute.h)
--FILE--
<?php
set_error_handler(function($_, $m) { throw new Exception($m); });
class Test {
    static function test() {
        call_user_func("static::ok");
    }
    static function ok() {
    }
}
Test::test();
?>
--EXPECTF--
Fatal error: Uncaught Exception: Use of "static" in callables is deprecated in %s:%d
Stack trace:
#0 %s(%d): {closure:%s:%d}(8192, 'Use of "static"...', %s, %d)
#1 %s(%d): Test::test()
#2 {main}
  thrown in %s on line %d
