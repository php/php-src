--TEST--
GH-22993: DateInterval error message with embedded NUL byte
--FILE--
<?php

try {
    new DateInterval("foo\0bar");
} catch (DateMalformedIntervalStringException $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
Fatal error: Uncaught ValueError: DateInterval::__construct(): Argument #1 ($duration) must not contain any null bytes in %s:%d
Stack trace:
#0 %s(%d): DateInterval->__construct('foo\x00bar')
#1 {main}
  thrown in %s on line %d
