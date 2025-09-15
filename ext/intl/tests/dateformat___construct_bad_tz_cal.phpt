--TEST--
IntlDateFormatter::__construct(): bad timezone or calendar
--EXTENSIONS--
intl
--FILE--
<?php

try {
    var_dump(new IntlDateFormatter(NULL, 0, 0, 'bad timezone'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump(new IntlDateFormatter(NULL, 0, 0, NULL, 3));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump(new IntlDateFormatter(NULL, 0, 0, NULL, new stdclass));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
IntlException: IntlDateFormatter::__construct(): No such time zone: "bad timezone"
IntlException: IntlDateFormatter::__construct(): Invalid value for calendar type; it must be one of IntlDateFormatter::TRADITIONAL (locale's default calendar) or IntlDateFormatter::GREGORIAN. Alternatively, it can be an IntlCalendar object
TypeError: IntlDateFormatter::__construct(): Argument #5 ($calendar) must be of type IntlCalendar|int|null, stdClass given

