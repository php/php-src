--TEST--
RFC example: Scalar Types

--FILE--
<?php
function answer(): int {
    return 42;
}

var_dump(answer());

--EXPECTF--
int(42)
