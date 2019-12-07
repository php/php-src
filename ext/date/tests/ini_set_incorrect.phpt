--TEST--
Test invalid time zone passed to ini_set
--FILE--
<?php

try {
    ini_set("date.timezone", "Incorrect/Zone");
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
Invalid date.timezone value 'Incorrect/Zone'
