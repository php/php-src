--TEST--
IntlCalendar::toDateTime(): bad arguments
--EXTENSIONS--
intl
--FILE--
<?php

$cal = new IntlGregorianCalendar("Etc/Unknown");
try {
	var_dump($cal->toDateTime());
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump(intlcal_to_date_time($cal));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$cal = IntlCalendar::createInstance("Etc/Unknown");
try {
    var_dump($cal->toDateTime());
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump(intlcal_to_date_time($cal));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
DateInvalidTimeZoneException: DateTimeZone::__construct(): Unknown or bad timezone (Etc/Unknown)
DateInvalidTimeZoneException: DateTimeZone::__construct(): Unknown or bad timezone (Etc/Unknown)
DateInvalidTimeZoneException: DateTimeZone::__construct(): Unknown or bad timezone (Etc/Unknown)
DateInvalidTimeZoneException: DateTimeZone::__construct(): Unknown or bad timezone (Etc/Unknown)
