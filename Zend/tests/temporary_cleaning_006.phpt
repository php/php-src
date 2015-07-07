--TEST--
Exception after separation during indirect write to fcall result
--FILE--
<?php

function throwing() { throw new Exception; }

function getArray() { return [0]; }

try {
    getArray()[throwing()] = 1;
} catch (Exception $e) {
    echo "Exception\n";
}

?>
--EXPECT--
Exception
