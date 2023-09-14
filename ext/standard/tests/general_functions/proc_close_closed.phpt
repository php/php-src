--TEST--
Test that proc_close() does not accept a closed Process
--FILE--
<?php

$pipe = null;
$process = proc_open('echo foo', [], $pipe);
proc_close($process);

try {
    proc_close($process);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
foo
Process has already been closed
