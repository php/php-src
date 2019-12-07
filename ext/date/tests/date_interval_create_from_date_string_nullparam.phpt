--TEST--
Test date_interval_create_from_date_string() function : null parameter
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--FILE--
<?php

try {
    var_dump(date_interval_create_from_date_string(null));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
Unknown or bad format () at position 0 ( ): Empty string
