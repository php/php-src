--TEST--
iterable type with object should NOT be redundant 2
--FILE--
<?php

function bar(): object|iterable|null {
    return null;
}

?>
===DONE===
--EXPECT--
===DONE===
