--TEST--
GitHub #12043 segfault with IntlDateFormatter::dateType where it equals to UDAT_PATTERN (icu 50) but
IntldateFormatter::timeType needs to be set as such.
--EXTENSIONS--
intl
--FILE--
<?php

$datetime = new \DateTime('2017-05-12 23:11:00 GMT+2');
static $UDAT_PATTERN = -2;

try {
    new IntlDateFormatter(
	    locale: 'en',
	    dateType: $UDAT_PATTERN,
	    timeType: 0,
	    timezone: $datetime->getTimezone(),
    );
} catch (\IntlException $e) {
    echo $e->getMessage();
}

--EXPECT--
datefmt_create: time format must be UDAT_PATTERN if date format is UDAT_PATTERN: U_ILLEGAL_ARGUMENT_ERROR
