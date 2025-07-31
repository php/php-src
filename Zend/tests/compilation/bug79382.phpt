--TEST--
Bug #79382: Cannot redeclare disabled function
--INI--
disable_functions=strlen
--FILE--
<?php

function strlen(string $x): int {
    return 42;
}

var_dump(strlen("foobar"));

?>
--EXPECT--
int(42)
