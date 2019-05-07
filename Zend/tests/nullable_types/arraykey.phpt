--TEST--
Explicitly nullable arraykey type
--FILE--
<?php

function _arraykey_(?arraykey $v): ?arraykey {
    return $v;
}

var_dump(_arraykey_(null));
var_dump(_arraykey_("php"));
var_dump(_arraykey_(1));
--EXPECT--
NULL
string(3) "php"
int(1)
