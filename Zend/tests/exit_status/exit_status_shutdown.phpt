--TEST--
exit_status() in shutdown handler preserving original exit code
--FILE--
<?php
register_shutdown_function(function() {
    echo "Current exit status: " . exit_status() . "\n";

    $original_status = exit_status();
    $final_status = $original_status ?: 255;

    echo "Final status will be: " . $final_status . "\n";
});

echo "Setting exit status to 42\n";
exit(42);
?>
--EXPECT--
Setting exit status to 42
Current exit status: 42
Final status will be: 42
