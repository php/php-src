--TEST--
Bug object gc not working in shutdown
--FILE--
<?php
ini_set('memory_limit', '2M');
register_shutdown_function(function () {
    for ($n = 1000 * 1000; $n--;) {
        new stdClass;
    }
    echo "OK\n";
});
?>
--EXPECT--
OK
