--TEST--
Bug #66986 (Some timezones are no longer valid in PHP 5.5.10)
--FILE--
<?php
$zones = array(
	"CST6CDT", "Cuba", "Egypt", "Eire", "EST5EDT", "Factory", "GB-Eire",
	"GMT0", "Greenwich", "Hongkong", "Iceland", "Iran", "Israel", "Jamaica",
	"Japan", "Kwajalein", "Libya", "MST7MDT", "Navajo", "NZ-CHAT", "Poland",
	"Portugal", "PST8PDT", "Singapore", "Turkey", "Universal", "W-SU",

	"UTC", "GMT", "GMT+0100", "-0230",
);

foreach ( $zones as $zone )
{
	$d = new DateTimeZone( $zone );
	print_r($d);
}
?>
--EXPECT--
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => CST6CDT
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => Cuba
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => Egypt
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => Eire
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => EST5EDT
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => Factory
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => GB-Eire
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => GMT0
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => Greenwich
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => Hongkong
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => Iceland
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => Iran
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => Israel
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => Jamaica
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => Japan
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => Kwajalein
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => Libya
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => MST7MDT
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => Navajo
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => NZ-CHAT
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => Poland
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => Portugal
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => PST8PDT
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => Singapore
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => Turkey
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => Universal
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => W-SU
)
DateTimeZone Object
(
    [timezone_type] => 3
    [timezone] => UTC
)
DateTimeZone Object
(
    [timezone_type] => 2
    [timezone] => GMT
)
DateTimeZone Object
(
    [timezone_type] => 1
    [timezone] => +01:00
)
DateTimeZone Object
(
    [timezone_type] => 1
    [timezone] => -02:30
)
