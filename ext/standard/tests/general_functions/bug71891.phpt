--TEST--
Bug #71891 (header_register_callback() and register_shutdown_function())
--FILE--
<?php

header_register_callback(function () {
    echo 'header';
    register_shutdown_function(function () {
        echo 'shutdown';
    });
});
?>
--EXPECT--
header
