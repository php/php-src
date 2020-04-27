--TEST--
Bug #79382: Cannot redeclare disabled function
--INI--
disable_functions=strlen
--FILE--
<?php

function strlen(string $x): int {
    $len = 0;
    while (isset($x[$len])) $len++;
    return $len;
}

var_dump(strlen("foobar"));

?>
--EXPECT--
int(6)
