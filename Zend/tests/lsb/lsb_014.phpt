--TEST--
ZE2 Late Static Binding access to static::const through defined() and get_constant()
--FILE--
<?php

class Test1 {
    static function test() {
        var_dump(defined("static::ok"));
        if (defined("static::ok")) {
            echo constant("static::ok");
        }
    }
}

class Test2 extends Test1 {
    const ok = "ok";
}
Test1::test();
Test2::test();
?>
--EXPECT--
bool(false)
bool(true)
ok
