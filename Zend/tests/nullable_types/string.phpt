--TEST--
Explicitly nullable string type
--FILE--
<?php

function _string_(?string $v): ?string {
    return $v;
}

var_dump(_string_(null));
var_dump(_string_("php"));
?>
--EXPECT--
NULL
string(3) "php"
