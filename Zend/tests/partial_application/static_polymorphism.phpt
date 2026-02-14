--TEST--
PFA: static polymorphism
--FILE--
<?php

class P {
    public static function m(string $a): void {
        echo __METHOD__, PHP_EOL;
        var_dump($a);
    }


    public static function get() {
        /* The method is resolved before creating the PFA, and captured by the
         * PFA. We use the signature of the resolved method. */
        return static::m(?);
    }
}

class C extends P {
    public static function m(string|array $b): void {
        echo __METHOD__, PHP_EOL;
        var_dump($b);
    }
}

for ($i = 0; $i < 2; $i++) {
    P::get()(a: 'a');
    C::get()(b: []);
}

?>
--EXPECT--
P::m
string(1) "a"
C::m
array(0) {
}
P::m
string(1) "a"
C::m
array(0) {
}
