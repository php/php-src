--TEST--
WeakMaps disallow structs
--FILE--
<?php

struct DC {}

function test($c) {
    $map = new WeakMap();
    $dc = new DC();

    try {
        $c($map, $dc);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}

test(function ($map, $dc) {
    var_dump($map[$dc]);
});
test(function ($map, $dc) {
    $map[$dc] = 1;
});
test(function ($map, $dc) {
    unset($map[$dc]);
});
test(function ($map, $dc) {
    var_dump(isset($map[$dc]));
});

?>
--EXPECT--
Instance of struct DC may not be used as key
Instance of struct DC may not be used as key
Instance of struct DC may not be used as key
Instance of struct DC may not be used as key
