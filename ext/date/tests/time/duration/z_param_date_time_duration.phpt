--TEST--
Time\Duration: Z_PARAM_DATE_TIME_DURATION() correctly aborts parameter parsing
--FILE--
<?php

require __DIR__ . '/helper.inc';

try {
    Time\Duration::compare(1, 2);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: Time\Duration::compare(): Argument #1 ($a) must be of type Time\Duration, int given
