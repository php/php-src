--TEST--
Exceeding arg count check should not affect variadic functions or methods
--FILE--
<?php

function fn(callable $callback) {
    $callback(1, 2, 3, 4, 5, 6);
    $callback(1, 2, 3);
    $callback(1);
}

fn(function($a, $b){
    echo __FUNCTION__, PHP_EOL;
});

echo PHP_EOL, "Done", PHP_EOL;

--EXPECTF--

{closure}
{closure}

Warning: Missing argument 2 for {closure}(), called in %s on line 6 and defined in %s on line 9
{closure}

Done
