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
	$d = DateInterval::createFromDateString($format);
}

foreach ($formats as $format) {
	$d = date_interval_create_from_date_string($format);
}

?>
--EXPECTF--
Warning: DateInterval::createFromDateString(): String 'next weekday 15:30' contains non-relative elements in %sdate_interval_non_relative_warning.php on line %d

Warning: DateInterval::createFromDateString(): String '+5 hours noon' contains non-relative elements in %sdate_interval_non_relative_warning.php on line %d

Warning: DateInterval::createFromDateString(): String '-8 days March 23' contains non-relative elements in %sdate_interval_non_relative_warning.php on line %d

Warning: DateInterval::createFromDateString(): String '+72 seconds UTC' contains non-relative elements in %sdate_interval_non_relative_warning.php on line %d

Warning: date_interval_create_from_date_string(): String 'next weekday 15:30' contains non-relative elements in %sdate_interval_non_relative_warning.php on line %d

Warning: date_interval_create_from_date_string(): String '+5 hours noon' contains non-relative elements in %sdate_interval_non_relative_warning.php on line %d

Warning: date_interval_create_from_date_string(): String '-8 days March 23' contains non-relative elements in %sdate_interval_non_relative_warning.php on line %d

Warning: date_interval_create_from_date_string(): String '+72 seconds UTC' contains non-relative elements in %sdate_interval_non_relative_warning.php on line %d
