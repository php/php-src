--TEST--
WeakMaps disallow data classes
--FILE--
<?php

data class DC {}

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
Instance of data class DC may not be used as key
Instance of data class DC may not be used as key
Instance of data class DC may not be used as key
Instance of data class DC may not be used as key
