--TEST--
Test that proc_terminate() does not accept a closed Process
--FILE--
<?php

$pipes = null;
$process = proc_open('echo "foo";', [], $pipes);
proc_close($process);

try {
    proc_terminate($process);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
foo
Process has already been closed
