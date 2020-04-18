--TEST--
Bug #79358: JIT miscompile in composer
--FILE--
<?php

function test(int $x) {
    return ($x > 0xdead && unimportant()) ||
           ($x < 0xbeef && unimportant());
}

var_dump(test(0xcccc));

?>
--EXPECT--
bool(false)
