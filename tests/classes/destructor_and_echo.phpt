--TEST--
ZE2 Destructors and echo
--FILE--
<?php

class Test
{
    function __construct() {
        echo __METHOD__ . "\n";
    }

    function __destruct() {
        echo __METHOD__ . "\n";
    }
}

$o = new Test;

?>
===DONE===
--EXPECT--
Test::__construct
===DONE===
Test::__destruct
