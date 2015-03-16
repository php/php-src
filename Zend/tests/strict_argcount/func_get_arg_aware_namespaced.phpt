--TEST--
Test if exceeding arg count check is func_get_arg() aware when on a namespace
--FILE--
<?php

namespace ns;

function fn($arg) {
    $arg = func_get_arg(0);
    echo __FUNCTION__, PHP_EOL;
}

fn(1);
fn(1, 2);

echo PHP_EOL, "Done", PHP_EOL;

--EXPECTF--
ns\fn
ns\fn

Done
