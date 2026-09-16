--TEST--
GitHub #12043 segfault with IntlDateFormatter::dateType where it equals to IntlDateFormatter::PATTERN (icu 50) but
IntldateFormatter::timeType needs to be set as such.
--EXTENSIONS--
intl
--FILE--
<?php

$datetime = new \DateTime('2017-05-12 23:11:00 GMT+2');

try {
    new IntlDateFormatter(
	    locale: 'en',
	    dateType: IntlDateFormatter::PATTERN,
	    timeType: 0,
	    timezone: $datetime->getTimezone(),
    );
} catch (\IntlException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
IntlException: IntlDateFormatter::__construct(): datefmt_create: time format must be IntlDateFormatter::PATTERN if date format is IntlDateFormatter::PATTERN
