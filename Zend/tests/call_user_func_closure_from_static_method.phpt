--TEST--
call_user_func() on non-static closure without $this inside a static method
--FILE--
<?php

class A {
    public static function exec(callable $c) {
        return call_user_func($c);
    }

    public static function doSomething() {
        return self::exec(function(){
            return "okay";
        });
    }
}

var_dump(A::doSomething());

?>
--EXPECT--
string(4) "okay"
