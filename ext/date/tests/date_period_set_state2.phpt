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
} catch (\Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Invalid serialization data for DatePeriod object
