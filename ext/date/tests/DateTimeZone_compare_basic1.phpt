--TEST--
Test of compare object handler for DateTimeZone objects
--FILE--
<?php

$timezones = array(
    ['+0200', '-0200'],
    ['EST', 'PST'],
    ['Europe/Amsterdam', 'Europe/Berlin']
);

foreach ($timezones as [$timezone1, $timezone2]) {
    compare_timezones($timezone1, $timezone1);
    compare_timezones($timezone1, $timezone2);
}

try {
	var_dump(new DateTimeZone('Europe/Berlin') == new DateTimeZone('CET'));
} catch (DateException $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

function compare_timezones($timezone1, $timezone2)
{
    $tz1 = new DateTimeZone($timezone1);
    $tz2 = new DateTimeZone($timezone2);
    echo "compare $timezone1 with $timezone2\n";
    echo "< ";
    var_dump($tz1 < $tz2);
    echo "= ";
    var_dump($tz1 == $tz2);
    echo "> ";
    var_dump($tz1 > $tz2);
}

// Test comparison of uninitialized DateTimeZone objects.
class MyDateTimeZone extends DateTimeZone {
    function __construct() {
         // parent ctor not called
    }
}

$tz1 = new MyDateTimeZone();
$tz2 = new MyDateTimeZone();
try {
    var_dump($tz1 == $tz2);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
compare +0200 with +0200
< bool(false)
= bool(true)
> bool(false)
compare +0200 with -0200
< bool(false)
= bool(false)
> bool(false)
compare EST with EST
< bool(false)
= bool(true)
> bool(false)
compare EST with PST
< bool(false)
= bool(false)
> bool(false)
compare Europe/Amsterdam with Europe/Amsterdam
< bool(false)
= bool(true)
> bool(false)
compare Europe/Amsterdam with Europe/Berlin
< bool(false)
= bool(false)
> bool(false)
DateException: Cannot compare two different kinds of DateTimeZone objects
DateObjectError: Trying to compare uninitialized DateTimeZone objects
