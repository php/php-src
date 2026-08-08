--TEST--
Bug #63206 Fully support error_handler stacking, even inside the error_handler
--FILE--
<?php

set_error_handler(function() {
    echo 'First handler' . PHP_EOL;
});

set_error_handler(function() {
    echo 'Second handler' . PHP_EOL;

    set_error_handler(function() {
        echo 'Internal handler' . PHP_EOL;
    });

	trigger_error('Error', E_USER_WARNING); // warnings while handling the error should go into internal handler

    restore_error_handler();
});

trigger_error('Error', E_USER_WARNING);
trigger_error('Error', E_USER_WARNING);
?>
--EXPECT--
Second handler
Internal handler
Second handler
Internal handler
