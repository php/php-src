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
    $previous = $e->getPrevious();
    echo '    ', $previous::class, ': ', $previous->getMessage(), PHP_EOL;
}

try {
    var_dump(intlcal_to_date_time($cal));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    $previous = $e->getPrevious();
    echo '    ', $previous::class, ': ', $previous->getMessage(), PHP_EOL;
}

$cal = IntlCalendar::createInstance("Etc/Unknown");
try {
    var_dump($cal->toDateTime());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    $previous = $e->getPrevious();
    echo '    ', $previous::class, ': ', $previous->getMessage(), PHP_EOL;
}

try {
    var_dump(intlcal_to_date_time($cal));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    $previous = $e->getPrevious();
    echo '    ', $previous::class, ': ', $previous->getMessage(), PHP_EOL;
}

?>
--EXPECT--
IntlException: DateTimeZone constructor threw exception
    DateInvalidTimeZoneException: DateTimeZone::__construct(): Unknown or bad timezone (Etc/Unknown)
IntlException: DateTimeZone constructor threw exception
    DateInvalidTimeZoneException: DateTimeZone::__construct(): Unknown or bad timezone (Etc/Unknown)
IntlException: DateTimeZone constructor threw exception
    DateInvalidTimeZoneException: DateTimeZone::__construct(): Unknown or bad timezone (Etc/Unknown)
IntlException: DateTimeZone constructor threw exception
    DateInvalidTimeZoneException: DateTimeZone::__construct(): Unknown or bad timezone (Etc/Unknown)
