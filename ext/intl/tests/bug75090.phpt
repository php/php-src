--TEST--
Bug #75090 Constants of parent IntlCalendar class not inherited
--EXTENSIONS--
intl
--FILE--
<?php
class Foo extends IntlCalendar {}

$fooRef = new ReflectionClass(Foo::class);
$intlGregorianCalendarRef = new ReflectionClass(IntlGregorianCalendar::class);
$intlCalendarRef = new ReflectionClass(IntlCalendar::class);

var_dump(
    count($fooRef->getConstants()) === count($intlCalendarRef->getConstants()),
    count($intlGregorianCalendarRef->getConstants()) === count($intlCalendarRef->getConstants())
);
?>
--EXPECT--
bool(true)
bool(true)
