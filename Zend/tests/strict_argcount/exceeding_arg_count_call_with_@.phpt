--TEST--
Test if exceeding arg count check is aware of @ operator
--FILE--
<?php

function fn($arg) {
    echo __FUNCTION__, PHP_EOL;
}

@fn(1, 2);

echo PHP_EOL, "Done", PHP_EOL;

--EXPECTF--
fn

Done
