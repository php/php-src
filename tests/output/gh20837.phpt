--TEST--
ob_start(): NULL dereference when calling ob_start() in shutdown function triggered by bailout in php_output_lock_error()
--FILE--
<?php

register_shutdown_function(function () {
    ob_start(function ($input) {
        echo "bar";
        return strtoupper($input);
    });
});

ob_start(function () {
    ob_start();
}, 1);

echo "foo";

?>
--EXPECTF--
Fatal error: ob_start(): Cannot use output buffering in output buffering display handlers in %s on line %d

Notice: ob_start(): Failed to create buffer in %s on line %d
