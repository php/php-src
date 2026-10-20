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

$formatter = new IntlRelativeDateTimeFormatter('en_US');
showThrowable(fn() => $formatter->__construct());
showThrowable(fn() => clone $formatter);

$unconstructedNumberFormatter = (new ReflectionClass(NumberFormatter::class))
    ->newInstanceWithoutConstructor();
showThrowable(fn() => new IntlRelativeDateTimeFormatter(
    'en_US',
    IntlRelativeDateTimeFormatterStyle::Long,
    IntlRelativeDateTimeFormatterCapitalization::None,
    $unconstructedNumberFormatter,
));

?>
--EXPECT--
ValueError: IntlRelativeDateTimeFormatter::__construct(): Argument #1 ($locale) "f" is invalid
ValueError: IntlRelativeDateTimeFormatter::__construct(): Argument #1 ($locale) must be less than or equal to 156 characters
Error: IntlRelativeDateTimeFormatter object is already constructed
Error: Trying to clone an uncloneable object of class IntlRelativeDateTimeFormatter
Error: Found unconstructed NumberFormatter
