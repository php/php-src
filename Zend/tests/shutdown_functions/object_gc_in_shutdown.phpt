--TEST--
Bug object gc not working in shutdown
--FILE--
<?php
ini_set('memory_limit', '4M');
register_shutdown_function(function () {
    for ($n = 2 * 1000 * 1000; $n--;) {
        new stdClass;
    }
    echo "OK\n";
});
?>
--EXPECT--
OK
