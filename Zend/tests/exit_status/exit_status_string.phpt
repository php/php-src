--TEST--
exit_status() with string exit codes
--FILE--
<?php
register_shutdown_function(function() {
    echo "Exit status: " . exit_status() . "\n";
});

echo "Before exit with string\n";
exit("Error message");
?>
--EXPECT--
Before exit with string
Error messageExit status: 0
