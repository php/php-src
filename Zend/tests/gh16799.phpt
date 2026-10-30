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
#0 %s(%d): {closure:%s}(8192, 'Use of "static"%s', '%s', %d)
#1 %s(%d): Test::test()
#2 {main}

Next TypeError: call_user_func(): Argument #1 ($callback) must be a valid callback, (null) in %s:%d
Stack trace:
#0 %s(%d): Test::test()
#1 {main}
  thrown in %s on line %d
