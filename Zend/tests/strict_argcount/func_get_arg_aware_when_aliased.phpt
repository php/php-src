--TEST--
Exceeding arg count check should be func_get_args() call aware when it's aliased
--FILE--
<?php

namespace ns;

use function func_get_arg as obfuscated_func_get_arg;

function fn($arg) {
    obfuscated_func_get_arg(0);
    echo __FUNCTION__, PHP_EOL;
}

fn(1);
fn(1, 2);

echo PHP_EOL, "Done", PHP_EOL;

--EXPECTF--
ns\fn
ns\fn

Done
