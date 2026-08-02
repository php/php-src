--TEST--
GH-22993: DateInterval::createFromDateString() error message with embedded NUL byte
--FILE--
<?php

try {
    DateInterval::createFromDateString("foo\0bar");
} catch (DateMalformedIntervalStringException $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
Unknown or bad format (foo%0bar) at position 0 (f): The timezone could not be found in the database
