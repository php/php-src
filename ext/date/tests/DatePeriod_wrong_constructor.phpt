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
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: DatePeriod::__construct() accepts (DateTimeInterface, DateInterval, int [, int]), or (DateTimeInterface, DateInterval, DateTime [, int]), or (string [, int]) as arguments
