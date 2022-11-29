--TEST--
iterable type with object should NOT be redundant 1
--FILE--
<?php

function bar(): iterable|object|null {
    return null;
}

?>
===DONE===
--EXPECT--
===DONE===
