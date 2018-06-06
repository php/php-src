--TEST--
Bug #75090 Constants of parent IntlCalendar class not inherited
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
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
===DONE===
--EXPECT--
bool(true)
bool(true)
===DONE===
