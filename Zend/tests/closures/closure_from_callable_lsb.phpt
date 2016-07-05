--TEST--
Testing Closure::fromCallable() functionality: Late static binding
--FILE--
<?php

class Foo {
    const BAR = 1;
    public static function method() {
        return static::BAR;
    }
}
var_dump(Closure::fromCallable(['Foo', 'method'])());

?>
--EXPECT--
int(1)
