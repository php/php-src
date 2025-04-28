--TEST--
SplObjectStorage disallows structs
--FILE--
<?php

struct DC {}

function test($c) {
    $map = new SplObjectStorage();
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
test(function ($map, $dc) {
    $map->attach($dc, 1);
});
test(function ($map, $dc) {
    $map->detach($dc);
});
test(function ($map, $dc) {
    var_dump($map->getHash($dc));
});
test(function ($map, $dc) {
    var_dump($map->offsetGet($dc));
});
test(function ($map, $dc) {
    var_dump($map->contains($dc));
});

?>
--EXPECT--
Instance of struct DC may not be used as key
Instance of struct DC may not be used as key
Instance of struct DC may not be used as key
Instance of struct DC may not be used as key
Instance of struct DC may not be used as key
Instance of struct DC may not be used as key
Instance of struct DC may not be used as key
Instance of struct DC may not be used as key
Instance of struct DC may not be used as key
