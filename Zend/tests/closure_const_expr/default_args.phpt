--TEST--
Closures in default argument
--FILE--
<?php

function test(
    Closure $name = static function () {
        echo "default", PHP_EOL;
    },
) {
    $name();
}

test();
test(function () {
    echo "explicit", PHP_EOL;
});

?>
--EXPECT--
default
explicit
