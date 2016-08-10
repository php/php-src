--TEST--
Don't statically bind arguments for self:: calls in traits
--FILE--
<?php

trait T {
    public static function method($arg) {
    }
    public static function call() {
        $i = 0;
        self::method($i);
        var_dump($i);
    }
}

class C {
    use T;

    public static function method(&$arg) {
        $arg++;
    }
}

C::call();

?>
--EXPECT--
int(1)
