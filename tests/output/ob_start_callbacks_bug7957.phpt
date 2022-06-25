--TEST--
Test ob_start() with callbacks with exit()
--FILE--
<?php

var_dump(ob_get_level());

// capture any unexpected output and exit execution
ob_start(function($buffer, $phase) {
    if (($phase & \PHP_OUTPUT_HANDLER_FLUSH ) === 0) {
        echo 'unexpected output: ' . $buffer . "\n"; // output from ob handler is not buffered
        exit;
    }
}, 1);

// do some work on shutdown
register_shutdown_function(function() {
    ob_flush();
    ob_end_clean();
    echo 'shutting down' . "\n";
    var_dump(ob_get_level());
    
    ob_start(function($buffer, $phase) {
        echo 'unexpected output II: ' . (strlen($buffer) === 0 ? '<empty>': $buffer) . "\n";
    }, 1);
    ob_flush();
    echo 'y';
    ob_end_clean();
    var_dump(ob_get_level());
});

ob_flush();
echo 'x';
echo 'y'; // never reached

?>
--EXPECT--
int(0)
unexpected output: x
shutting down
int(0)
unexpected output II: <empty>
unexpected output II: y
int(0)
