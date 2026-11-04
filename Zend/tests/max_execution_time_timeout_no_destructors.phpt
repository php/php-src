--TEST--
max_execution_time timeout does not call destructors
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--INI--
max_execution_time=1
--FILE--
<?php

$obj = new class {
    function __destruct() {
        echo "destructor called\n";
    }
};

register_shutdown_function(function() {
    echo "shutdown function called\n";
});

while (true) {}

?>
--EXPECTF--
Fatal error: Maximum execution time of 1 second exceeded %s
shutdown function called
