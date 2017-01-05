--TEST--
Exception after separation during indirect write to fcall result
--XFAIL--
See Bug #62210 and attempt to fix it in "tmp_livelibess" branch
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
