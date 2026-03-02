--TEST--
Bug #80047: DatePeriod doesn't support custom DateTimeImmutable
--INI--
date.timezone=UTC
--FILE--
<?php
class CustomDateTime extends DateTime {}
class CustomDateTimeImmutable extends DateTimeImmutable {}

$dt = new DateTime('2022-06-24');
$dti = new DateTimeImmutable('2022-06-24');
$cdt = new CustomDateTime('2022-06-25');
$cdti = new CustomDateTimeImmutable('2022-06-25');
$i = new DateInterval('P1D');

$tests = [
	[ $dt,   $i, $cdt  ],
	[ $cdt,  $i, $dt   ],
	[ $cdt,  $i, $cdt  ],
	[ $dti,  $i, $cdti ],
	[ $cdti, $i, $dti  ],
	[ $cdti, $i, $cdti ],
	[ $cdt,  $i, $cdti ],
];

foreach ($tests as $test) {
	$dp = new DatePeriod(...$test);
	foreach ($dp as $date) {}
	echo get_class($date), "\n";
}
?>
--EXPECT--
DateTime
DateTime
DateTime
DateTimeImmutable
DateTimeImmutable
DateTimeImmutable
DateTimeImmutable
