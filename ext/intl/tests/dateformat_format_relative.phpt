--TEST--
datefmt_format_code() with relative formats
--EXTENSIONS--
intl
--FILE--
<?php

printFormat(IntlDateFormatter::RELATIVE_FULL, IntlDateFormatter::NONE, getYesterday());
printFormat(IntlDateFormatter::RELATIVE_LONG, IntlDateFormatter::NONE, getYesterday());
printFormat(IntlDateFormatter::RELATIVE_MEDIUM, IntlDateFormatter::NONE, getYesterday());
printFormat(IntlDateFormatter::RELATIVE_SHORT, IntlDateFormatter::NONE, getYesterday());

printFormat(IntlDateFormatter::RELATIVE_FULL, IntlDateFormatter::NONE, getToday());
printFormat(IntlDateFormatter::RELATIVE_LONG, IntlDateFormatter::NONE, getToday());
printFormat(IntlDateFormatter::RELATIVE_MEDIUM, IntlDateFormatter::NONE, getToday());
printFormat(IntlDateFormatter::RELATIVE_SHORT, IntlDateFormatter::NONE, getToday());

printFormat(IntlDateFormatter::RELATIVE_FULL, IntlDateFormatter::NONE, getTomorrow());
printFormat(IntlDateFormatter::RELATIVE_LONG, IntlDateFormatter::NONE, getTomorrow());
printFormat(IntlDateFormatter::RELATIVE_MEDIUM, IntlDateFormatter::NONE, getTomorrow());
printFormat(IntlDateFormatter::RELATIVE_SHORT, IntlDateFormatter::NONE, getTomorrow());

printFormat(IntlDateFormatter::RELATIVE_FULL, IntlDateFormatter::NONE, getDayInPast());
printFormat(IntlDateFormatter::RELATIVE_LONG, IntlDateFormatter::NONE, getDayInPast());
printFormat(IntlDateFormatter::RELATIVE_MEDIUM, IntlDateFormatter::NONE, getDayInPast());
printFormat(IntlDateFormatter::RELATIVE_SHORT, IntlDateFormatter::NONE, getDayInPast());

function printFormat(int $dateFormat, int $timeFormat, DateTimeImmutable $time) {
    $formatter = new IntlDateFormatter(
        "en_US",
        $dateFormat,
        $timeFormat,
        "America/Los_Angeles",
        IntlDateFormatter::GREGORIAN
    );

    echo $formatter->format($time) . "\n";
}

function getToday(): DateTimeImmutable {
    return new DateTimeImmutable("now", new DateTimeZone("America/Los_Angeles"));
}

function getYesterday(): DateTimeImmutable {
    return new DateTimeImmutable("-1 day", new DateTimeZone("America/Los_Angeles"));
}

function getTomorrow(): DateTimeImmutable {
    return new DateTimeImmutable("+1 day", new DateTimeZone("America/Los_Angeles"));
}

function getDayInPast(): DateTimeImmutable {
    return new DateTimeImmutable("2020-01-20 20:20:20", new DateTimeZone("America/Los_Angeles"));
}

?>
--EXPECT--
yesterday
yesterday
yesterday
yesterday
today
today
today
today
tomorrow
tomorrow
tomorrow
tomorrow
Monday, January 20, 2020
January 20, 2020
Jan 20, 2020
1/20/20
