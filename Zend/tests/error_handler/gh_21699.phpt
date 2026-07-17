--TEST--
GH-21699: Assertion failure in shutdown_executor when error handler throws during self:: callable resolution
--FILE--
<?php
set_error_handler(function () {
    throw new Exception;
});
class bar {
    public static function __callstatic($fusion, $b)
    {
    }
    public function test()
    {
        call_user_func('self::y');
    }
}
$x = new bar;
$x->test();
?>
--EXPECTF--
Fatal error: Uncaught Exception in %s:%d
Stack trace:
#0 %s(%d): {closure:%s}(%d, 'Use of "self" i%s', '%s', %d)
#1 %s(%d): bar->test()
#2 {main}

Next TypeError: call_user_func(): Argument #1 ($callback) must be a valid callback, (null) in %s:%d
Stack trace:
#0 %s(%d): bar->test()
#1 {main}
  thrown in %s on line %d
