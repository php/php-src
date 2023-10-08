--TEST--
DatePeriod invalid serialization data
--FILE--
<?php
$propertySets = [
	/* Two correct elements */
	[
		'start' => new \DateTimeImmutable("2023-01-13 12:29:30"), 'end' => new \DateTimeImmutable("2023-01-16 16:49:29"), 'current' => new \DateTimeImmutable("2023-01-15 00:00:00"),
		'interval' => DateInterval::createFromDateString("tomorrow"), 'recurrences' => 1, 'include_start_date' => true, 'include_end_date' => true,
	],
	[
		'start' => null, 'end' => null, 'current' => null,
		'interval' => DateInterval::createFromDateString("tomorrow"), 'recurrences' => 1, 'include_start_date' => false, 'include_end_date' => false,
	],
	/* Error situations */
	[
		'start' => "2023-01-13 12:29:30", 'end' => new \DateTimeImmutable("2023-01-16 16:49:29"), 'current' => new \DateTimeImmutable("2023-01-15 00:00:00"),
		'interval' => DateInterval::createFromDateString("tomorrow"), 'recurrences' => 1, 'include_start_date' => true, 'include_end_date' => true,
	],
	[
		'start' => new \DateTimeImmutable("2023-01-13 12:29:30"), 'end' => "2023-01-16 16:49:29", 'current' => new \DateTimeImmutable("2023-01-15 00:00:00"),
		'interval' => DateInterval::createFromDateString("tomorrow"), 'recurrences' => 1, 'include_start_date' => true, 'include_end_date' => true,
	],
	[
		'start' => new \DateTimeImmutable("2023-01-13 12:29:30"), 'end' => new \DateTimeImmutable("2023-01-16 16:49:29"), 'current' => "2023-01-15 00:00:00",
		'interval' => DateInterval::createFromDateString("tomorrow"), 'recurrences' => 1, 'include_start_date' => true, 'include_end_date' => true,
	],
	[
		'start' => new \DateTimeImmutable("2023-01-13 12:29:30"), 'end' => new \DateTimeImmutable("2023-01-16 16:49:29"), 'current' => new \DateTimeImmutable("2023-01-15 00:00:00"),
		'interval' => "tomorrow", 'recurrences' => 1, 'include_start_date' => true, 'include_end_date' => true,
	],
	[
		'start' => new \DateTimeImmutable("2023-01-13 12:29:30"), 'end' => new \DateTimeImmutable("2023-01-16 16:49:29"), 'current' => new \DateTimeImmutable("2023-01-15 00:00:00"),
		'interval' => DateInterval::createFromDateString("tomorrow"), 'recurrences' => -1, 'include_start_date' => true, 'include_end_date' => true,
	],
	[
		'start' => new \DateTimeImmutable("2023-01-13 12:29:30"), 'end' => new \DateTimeImmutable("2023-01-16 16:49:29"), 'current' => new \DateTimeImmutable("2023-01-15 00:00:00"),
		'interval' => DateInterval::createFromDateString("tomorrow"), 'recurrences' => 1, 'include_start_date' => "true", 'include_end_date' => true,
	],
	[
		'start' => new \DateTimeImmutable("2023-01-13 12:29:30"), 'end' => new \DateTimeImmutable("2023-01-16 16:49:29"), 'current' => new \DateTimeImmutable("2023-01-15 00:00:00"),
		'interval' => DateInterval::createFromDateString("tomorrow"), 'recurrences' => 1, 'include_start_date' => true, 'include_end_date' => "true",
	],
];

foreach( $propertySets as $propertySet )
{
	try {
		$d = DatePeriod::__set_state( $propertySet );
		echo "OK\n";
	} catch (\Error $e) {
		echo $e::class, ': ', $e->getMessage(), "\n";
	}
}
?>
--EXPECT--
OK
OK
Error: Invalid serialization data for DatePeriod object
Error: Invalid serialization data for DatePeriod object
Error: Invalid serialization data for DatePeriod object
Error: Invalid serialization data for DatePeriod object
Error: Invalid serialization data for DatePeriod object
Error: Invalid serialization data for DatePeriod object
Error: Invalid serialization data for DatePeriod object
