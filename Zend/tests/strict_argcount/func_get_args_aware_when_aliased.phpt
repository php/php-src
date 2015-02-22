--TEST--
Exceeding arg count check should be func_get_args() call aware when it's aliased
--FILE--
<?php

namespace ns;

use function func_get_args as obfuscated_func_get_args;

function fn($arg) {
    obfuscated_func_get_args();
    echo __FUNCTION__, PHP_EOL;
}

fn(1);
fn();
fn(1, 2);

echo PHP_EOL, "Done", PHP_EOL;

--EXPECTF--
ns\fn

Warning: Missing argument 1 for ns\fn(), called in %s on line %d and defined in %s on line 7
ns\fn
ns\fn

Done
