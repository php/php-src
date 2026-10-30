--TEST--
Test that calling DatePeriod::__set_state() directly with missing arguments throws
--FILE--
<?php

try {
    DatePeriod::__set_state(
        [
            "start" => new DateTime,
        ]
    );
} catch (\Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Invalid serialization data for DatePeriod object
