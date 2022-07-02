--TEST--
While it may not be very useful, static is also permitted in final classes
--FILE--
<?php

final class Test {
    public static function create(): static {
        return new static;
    }
}

var_dump(Test::create());

?>
--EXPECT--
object(Test)#1 (0) {
}
