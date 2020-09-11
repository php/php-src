--TEST--
DatePeriod: Test wrong __construct parameter
--CREDITS--
Havard Eide <nucleuz@gmail.com>
#PHPTestFest2009 Norway 2009-06-09 \o/
--INI--
date.timezone=UTC
--FILE--
<?php

try {
    new DatePeriod();
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
DatePeriod::__construct() accepts either (DateTimeInterface, DateInterval, int [, int]) OR (DateTimeInterface, DateInterval, DateTime [, int]) OR (string [, int]) as arguments
