--TEST--
Exceeding arg count check and __call, __callStatic, __invoke
--FILE--
<?php

class Magic {
    public function __call($method, array $args) {
        echo __METHOD__, PHP_EOL;
    }

    public static function __callStatic($method, array $args) {
        echo __METHOD__, PHP_EOL;
    }

    public function __invoke($arg) {
        echo __METHOD__, PHP_EOL;
    }
}

Magic::magic();
Magic::magic(1, 2, 3);
call_user_func_array(["Magic", "magic"], [1, 2, 3]);
(new Magic)->magic();
(new Magic)->magic(1, 2, 3);
call_user_func_array([new Magic, "magic"], [1, 2, 3]);

$magic = new Magic;
$magic(1, 2, 3);
$magic->__invoke(1, 2, 3, 4);

echo PHP_EOL, "Done", PHP_EOL;

--EXPECTF--
Magic::__callStatic
Magic::__callStatic
Magic::__callStatic
Magic::__call
Magic::__call
Magic::__call

Warning: Magic::__invoke() expects at most 1 parameter, 3 given, defined in %s on line 12 and called in %s on line 25
Magic::__invoke

Warning: Magic::__invoke() expects at most 1 parameter, 4 given, defined in %s on line 12 and called in %s on line 26
Magic::__invoke

Done
