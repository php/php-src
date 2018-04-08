--TEST--
Explicitly nullable array type
--FILE--
<?php

function _array_(?array $v): ?array {
    return $v;
}

var_dump(_array_(null));
var_dump(_array_([]));

--EXPECT--
NULL
array(0) {
}

