--TEST--
Test that proc_get_status() does not accept a closed Process 
--FILE--
<?php

$pipe = null;
$process = proc_open('echo "foo";', [], $pipe);
proc_close($process);

try {
    proc_get_status($process);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
foo
Process has already been closed
