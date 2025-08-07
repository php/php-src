--TEST--
IntlTimeZone::toDateTimeZone(): errors
--EXTENSIONS--
intl
--FILE--
<?php

$tz = IntlTimeZone::createTimeZone('Etc/Unknown');

try {
    var_dump($tz->toDateTimeZone());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    $previous = $e->getPrevious();
    echo '    ', $previous::class, ': ', $previous->getMessage(), PHP_EOL;
}

?>
--EXPECT--
IntlException: DateTimeZone constructor threw exception
    DateInvalidTimeZoneException: DateTimeZone::__construct(): Unknown or bad timezone (Etc/Unknown)
