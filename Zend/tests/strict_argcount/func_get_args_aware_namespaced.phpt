--TEST--
Test if exceeding arg count check is func_get_args() aware when on a namespace
--FILE--
<?php

namespace ns;

function fn($arg) {
    $args = \func_get_args();
    echo __FUNCTION__, PHP_EOL;
}

fn(1);
fn();
fn(1, 2);

echo PHP_EOL, "Done", PHP_EOL;

--EXPECTF--
ns\fn

Warning: Missing argument 1 for ns\fn(), called in %s on line 11 and defined in %s on line 5
ns\fn
ns\fn

Done
