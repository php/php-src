--TEST--
GH-8458 (DateInterval::createFromDateString does not warn when non-relative items are present)
--FILE--
<?php
$formats = [
	'next weekday 15:30',
	'+5 hours noon',
	'-8 days March 23',
	'+72 seconds UTC',
];

foreach ($formats as $format) {
	try {
		$d = DateInterval::createFromDateString($format);
	} catch (DateMalformedIntervalStringException $e) {
		echo $e::class, ': ', $e->getMessage(), "\n";
	}
}

echo "====\n";

foreach ($formats as $format) {
	$d = date_interval_create_from_date_string($format);
}

?>
--EXPECTF--
DateMalformedIntervalStringException: String 'next weekday 15:30' contains non-relative elements
DateMalformedIntervalStringException: String '+5 hours noon' contains non-relative elements
DateMalformedIntervalStringException: String '-8 days March 23' contains non-relative elements
DateMalformedIntervalStringException: String '+72 seconds UTC' contains non-relative elements
====

Warning: date_interval_create_from_date_string(): String 'next weekday 15:30' contains non-relative elements in %s on line %d

Warning: date_interval_create_from_date_string(): String '+5 hours noon' contains non-relative elements in %s on line %d

Warning: date_interval_create_from_date_string(): String '-8 days March 23' contains non-relative elements in %s on line %d

Warning: date_interval_create_from_date_string(): String '+72 seconds UTC' contains non-relative elements in %s on line %d
