--TEST--
GH-9708: object_properties_load() bypasses typed property checks
--FILE--
<?php

try {
    unserialize('O:17:"Random\Randomizer":1:{i:0;a:1:{s:6:"engine";N;}}');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Exception: Invalid serialization data for Random\Randomizer object
