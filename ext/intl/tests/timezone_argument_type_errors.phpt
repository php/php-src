--TEST--
Intl timezone argument APIs reject non-stringable objects with TypeError
--EXTENSIONS--
intl
--FILE--
<?php

function dump_exception(callable $cb): void {
	try {
		$cb();
	} catch (Throwable $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
}

$std = new stdClass();
$calendar = IntlCalendar::createInstance();
$formatter = new IntlDateFormatter(null, IntlDateFormatter::NONE, IntlDateFormatter::NONE);

dump_exception(fn() => intlcal_create_instance($std));
dump_exception(fn() => IntlCalendar::createInstance($std));
dump_exception(fn() => intlcal_set_time_zone($calendar, $std));
dump_exception(fn() => $calendar->setTimeZone($std));
dump_exception(fn() => new IntlGregorianCalendar($std));
dump_exception(fn() => datefmt_create(null, IntlDateFormatter::NONE, IntlDateFormatter::NONE, $std));
dump_exception(fn() => IntlDateFormatter::create(null, IntlDateFormatter::NONE, IntlDateFormatter::NONE, $std));
dump_exception(fn() => new IntlDateFormatter(null, IntlDateFormatter::NONE, IntlDateFormatter::NONE, $std));
dump_exception(fn() => datefmt_set_timezone($formatter, $std));
dump_exception(fn() => $formatter->setTimeZone($std));

?>
--EXPECT--
TypeError: intlcal_create_instance(): Argument #1 ($timezone) Object of class stdClass could not be converted to string
TypeError: IntlCalendar::createInstance(): Argument #1 ($timezone) Object of class stdClass could not be converted to string
TypeError: intlcal_set_time_zone(): Argument #2 ($timezone) Object of class stdClass could not be converted to string
TypeError: IntlCalendar::setTimeZone(): Argument #1 ($timezone) Object of class stdClass could not be converted to string
TypeError: IntlGregorianCalendar::__construct(): Argument #1 ($timezoneOrYear) Object of class stdClass could not be converted to string
TypeError: datefmt_create(): Argument #4 ($timezone) Object of class stdClass could not be converted to string
TypeError: IntlDateFormatter::create(): Argument #4 ($timezone) Object of class stdClass could not be converted to string
TypeError: IntlDateFormatter::__construct(): Argument #4 ($timezone) Object of class stdClass could not be converted to string
TypeError: datefmt_set_timezone(): Argument #2 ($timezone) Object of class stdClass could not be converted to string
TypeError: IntlDateFormatter::setTimeZone(): Argument #1 ($timezone) Object of class stdClass could not be converted to string
