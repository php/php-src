--TEST--
Test for bug #74652: Incomplete dates
--INI--
date.timezone=UTC
--FILE--
<?php
$formats = [
	'2017-03-25 10:52:09',
	'2017-03-25 10:52',
	'2017-03-25 10am',
	'2017-03-25',
	'2017-03',
	'2017.042',
	'2017043',
];

foreach ( $formats as $format )
{
	$dt = new DateTimeImmutable( $format );
	echo $dt->format( 'Y-m-d H:i:s' ), "\n";
}
?>
--EXPECT--
2017-03-25 10:52:09
2017-03-25 10:52:00
2017-03-25 10:00:00
2017-03-25 00:00:00
2017-03-01 00:00:00
2017-02-11 00:00:00
2017-02-12 00:00:00
