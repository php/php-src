--TEST--
Pattern matching: Bug 001
--FILE--
<?php

function test($a) {
    $a is 42|43;
}

test(42);

?>
===DONE===
--EXPECT--
===DONE===
