--TEST--
Bug #45186 (__call depends on __callStatic in class scope)
--FILE--
<?php

class bar  {
    public function __call($a, $b) {
        print "__call:\n";
        var_dump($a);
    }
    static public function __callStatic($a, $b) {
        print "__callStatic:\n";
        var_dump($a);
    }
    public function test() {
        self::ABC();
        bar::ABC();
        call_user_func(array('BAR', 'xyz'));
        call_user_func('BAR::www');
        call_user_func(array('self', 'y'));
        call_user_func('self::y');
    }
    static function x() {
        print "ok\n";
    }
}

$x = new bar;

$x->test();

call_user_func(array('BAR','x'));
call_user_func('BAR::www');
try {
    call_user_func('self::y');
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
__call:
string(%d) "ABC"
__call:
string(%d) "ABC"

Fatal error: Uncaught TypeError: call_user_func(): Argument #%d ($callback) must be a valid callback, class "BAR" not found in %s:%d
Stack trace:
#%d %s(27): bar->test()
#%d {main}
  thrown in %s on line %d
