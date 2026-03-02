--TEST--
Exception after separation during indirect write to fcall result
--FILE--
<?php

function throwing() { throw new Exception; }

function getArray($x) { return [$x]; }

try {
    getArray(0)[throwing()] = 1;
} catch (Exception $e) {
    echo "Exception\n";
}

?>
--EXPECT--
Exception
