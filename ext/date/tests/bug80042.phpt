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
$cdt = new CustomDateTime('2022-06-24');
$cdti = new CustomDateTimeImmutable('2022-06-24');
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
	try {
		$dp = new DatePeriod(...$test);
	} catch ( Exception $e ) {
		echo $e->getMessage(), "\n";
	}
}
?>
--EXPECT--
DatePeriod::__construct(): Class of end date must be exactly DateTime or DateTimeImmutable, object of class CustomDateTime provided
DatePeriod::__construct(): Class of start date must be exactly DateTime or DateTimeImmutable, object of class CustomDateTime provided
DatePeriod::__construct(): Class of start date must be exactly DateTime or DateTimeImmutable, object of class CustomDateTime provided
DatePeriod::__construct(): Class of end date must be exactly DateTime or DateTimeImmutable, object of class CustomDateTimeImmutable provided
DatePeriod::__construct(): Class of start date must be exactly DateTime or DateTimeImmutable, object of class CustomDateTimeImmutable provided
DatePeriod::__construct(): Class of start date must be exactly DateTime or DateTimeImmutable, object of class CustomDateTimeImmutable provided
DatePeriod::__construct(): Class of start date must be exactly DateTime or DateTimeImmutable, object of class CustomDateTime provided
