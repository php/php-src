--TEST--
Exceeding arg count check should not affect magic method calls
--FILE--
<?php

class Magic {
    public function __call($method, array $args) {
        echo __METHOD__, PHP_EOL;
    }

    public static function __callStatic($method, array $args) {
        echo __METHOD__, PHP_EOL;
    }
}

Magic::magic();
Magic::magic(1, 2, 3);
call_user_func_array(["Magic", "magic"], [1, 2, 3]);
(new Magic)->magic();
(new Magic)->magic(1, 2, 3);
call_user_func_array([new Magic, "magic"], [1, 2, 3]);

echo PHP_EOL, "Done", PHP_EOL;

--EXPECTF--
Magic::__callStatic
Magic::__callStatic
Magic::__callStatic
Magic::__call
Magic::__call
Magic::__call

Done
