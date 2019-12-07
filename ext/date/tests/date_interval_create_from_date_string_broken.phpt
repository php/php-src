--TEST--
Test date_interval_create_from_date_string() function : nonsense data
--FILE--
<?php

try {
    var_dump(date_interval_create_from_date_string("foobar"));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
Unknown or bad format (foobar) at position 0 (f): The timezone could not be found in the database
