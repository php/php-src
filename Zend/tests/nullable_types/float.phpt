--TEST--
Explicitly nullable float type
--FILE--
<?php

function _float_(?float $v): ?float {
    return $v;
}

var_dump(_float_(null));
var_dump(_float_(1.3));
--EXPECT--
NULL
float(1.3)
