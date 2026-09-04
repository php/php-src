--TEST--
IntlRelativeDateTimeFormatter argument validation and object state errors
--EXTENSIONS--
intl
--FILE--
<?php

function showThrowable(Closure $callback): void {
    try {
        $callback();
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

showThrowable(fn() => new IntlRelativeDateTimeFormatter('f'));
showThrowable(fn() => new IntlRelativeDateTimeFormatter(str_repeat('a', 157)));
showThrowable(fn() => new IntlRelativeDateTimeFormatter('en_US', -1));
showThrowable(fn() => new IntlRelativeDateTimeFormatter(
    'en_US',
    IntlRelativeDateTimeFormatter::STYLE_LONG,
    -1,
));

$formatter = new IntlRelativeDateTimeFormatter('en_US');
showThrowable(fn() => $formatter->format(1, -1));
showThrowable(fn() => $formatter->formatNumeric(1, 15));
showThrowable(fn() => $formatter->__construct());
showThrowable(fn() => clone $formatter);

$unconstructedNumberFormatter = (new ReflectionClass(NumberFormatter::class))
    ->newInstanceWithoutConstructor();
showThrowable(fn() => new IntlRelativeDateTimeFormatter(
    'en_US',
    IntlRelativeDateTimeFormatter::STYLE_LONG,
    IntlRelativeDateTimeFormatter::CAPITALIZATION_NONE,
    $unconstructedNumberFormatter,
));

?>
--EXPECT--
ValueError: IntlRelativeDateTimeFormatter::__construct(): Argument #1 ($locale) "f" is invalid
ValueError: IntlRelativeDateTimeFormatter::__construct(): Argument #1 ($locale) must be less than or equal to 156 characters
ValueError: IntlRelativeDateTimeFormatter::__construct(): Argument #2 ($style) must be one of IntlRelativeDateTimeFormatter::STYLE_LONG, IntlRelativeDateTimeFormatter::STYLE_SHORT, or IntlRelativeDateTimeFormatter::STYLE_NARROW
ValueError: IntlRelativeDateTimeFormatter::__construct(): Argument #3 ($capitalizationContext) must be one of the IntlRelativeDateTimeFormatter::CAPITALIZATION_* constants
ValueError: IntlRelativeDateTimeFormatter::format(): Argument #2 ($unit) must be one of the IntlRelativeDateTimeFormatter::UNIT_* constants
ValueError: IntlRelativeDateTimeFormatter::formatNumeric(): Argument #2 ($unit) must be one of the IntlRelativeDateTimeFormatter::UNIT_* constants
Error: IntlRelativeDateTimeFormatter object is already constructed
Error: Trying to clone an uncloneable object of class IntlRelativeDateTimeFormatter
Error: Found unconstructed NumberFormatter
