--TEST--
DatePeriod by-ref iterator
--FILE--
<?php
$properties = [
	'start' => new \DateTimeImmutable("2023-01-13 12:29:30"), 'end' => new \DateTimeImmutable("2023-01-16 16:49:29"), 'current' => new \DateTimeImmutable("2023-01-15 00:00:00"),
	'interval' => DateInterval::createFromDateString("tomorrow"), 'recurrences' => 1, 'include_start_date' => true, 'include_end_date' => true,
];

$d = DatePeriod::__set_state( $properties );
try {
	foreach( $d as &$item )
	{
		echo $item->format(DateTime::ISO8601), "\n";
	}
	echo "OK\n";
} catch (\Error $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: An iterator cannot be used with foreach by reference
