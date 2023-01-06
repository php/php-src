--TEST--
Bug #50555 (DateTime::sub() allows 'relative' time modifications) (OO).
--FILE--
<?php
$now = '2010-03-07 13:21:38 UTC';
//positive DateInterval
$da1 = date_create( $now );
$ds1 = date_create( $now );
$i = DateInterval::createFromDateString('third Tuesday of next month');
echo $da1->format( DateTime::ISO8601 ), "\n";
echo date_add($da1, $i)->format( DateTime::ISO8601 ), "\n";
try {
	$ds1->sub($i);
} catch (DateInvalidOperationException $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

//negative DateInterval
$da2 = date_create( $now );
$ds2 = date_create( $now );
$i2 = DateInterval::createFromDateString('third Tuesday of last month');
echo $da2->format( DateTime::ISO8601 ), "\n";
echo date_add($da2, $i2)->format( DateTime::ISO8601 ), "\n";//works
try {
	$ds2->sub($i);
} catch (DateInvalidOperationException $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
2010-03-07T13:21:38+0000
2010-04-20T13:21:38+0000
DateInvalidOperationException: DateTime::sub(): Only non-special relative time specifications are supported for subtraction
2010-03-07T13:21:38+0000
2010-02-16T13:21:38+0000
DateInvalidOperationException: DateTime::sub(): Only non-special relative time specifications are supported for subtraction
