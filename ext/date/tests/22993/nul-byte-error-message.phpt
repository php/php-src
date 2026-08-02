--TEST--
GH-22993: DateTimeImmutable error message with embedded NUL byte
--FILE--
<?php

try {
    new DateTimeImmutable("foo\0bar");
} catch (DateMalformedStringException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Failed to parse time string (foo%0bar) at position 0 (f): The timezone could not be found in the database
