--TEST--
Unit enums do not have values()
--FILE--
<?php

enum U {
    case A;
}

var_dump(method_exists(U::class, 'values'));

?>
--EXPECT--
bool(false)

