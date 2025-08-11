--TEST--
exit_status() with uncaught exceptions setting exit code
--FILE--
<?php
register_shutdown_function(function() {
    echo "Exit status is: " . exit_status() . "\n";
});

// set exit status to 255
throw new Exception("Test exception");
?>
--EXPECTF--
Fatal error: Uncaught Exception: Test exception in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
Exit status is: 255
