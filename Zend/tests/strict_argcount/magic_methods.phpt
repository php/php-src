--TEST--
Skip exceeding arg count check for __call, __callStatic and __invoke but not for __construct
--FILE--
<?php

class Magic {
    public function __construct($a, $b) {}

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

new Magic(1);
new Magic(1, 2);
new Magic(1, 2, 3);

Magic::magic();
Magic::magic(1, 2, 3);
call_user_func_array(["Magic", "magic"], [1, 2, 3]);
(new Magic(1, 2))->magic();
(new Magic(1, 2))->magic(1, 2, 3);
call_user_func_array([new Magic(1, 2), "magic"], [1, 2, 3]);

$magic = new Magic(1, 2);
$magic(1, 2, 3);
$magic->__invoke(1, 2, 3, 4);

echo PHP_EOL, "Done", PHP_EOL;

--EXPECTF--
Warning: Missing argument 2 for Magic::__construct(), called in %s on line 19 and defined in %s on line 4

Warning: Magic::__construct() expects at most 2 parameters, 3 given, defined in %s on line 4 and called in %s on line 21
Magic::__callStatic
Magic::__callStatic
Magic::__callStatic
Magic::__call
Magic::__call
Magic::__call
Magic::__invoke
Magic::__invoke

Done
