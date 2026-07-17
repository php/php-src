--TEST--
GH-21699 (parent::): no shutdown_executor trampoline assertion when error handler throws during parent:: callable resolution
--FILE--
<?php
set_error_handler(function () {
    throw new Exception;
});
class Base {
    public static function __callStatic($name, $args)
    {
    }
}
class Child extends Base {
    public function test()
    {
        call_user_func('parent::missing');
    }
}
(new Child)->test();
?>
--EXPECTF--
Fatal error: Uncaught Exception in %s:%d
Stack trace:
#0 %s(%d): {closure:%s}(%d, 'Use of "parent"%s', '%s', %d)
#1 %s(%d): Child->test()
#2 {main}

Next TypeError: call_user_func(): Argument #1 ($callback) must be a valid callback, (null) in %s:%d
Stack trace:
#0 %s(%d): Child->test()
#1 {main}
  thrown in %s on line %d
