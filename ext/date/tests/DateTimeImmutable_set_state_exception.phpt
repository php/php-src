--TEST--
DateTimeImmutable invalid serialization data
--FILE--
<?php
$propertySets = [
	[ 'date' => "2023-01-13 12:29:30", 'timezone_type' => 1,   'timezone' => "+02:30"      ],
	[ 'date' => "2023-01-13 12:29:30", 'timezone_type' => 3,   'timezone' => "Europe/Kyiv" ],
	[ 'date' => 2023.113,              'timezone_type' => 1,   'timezone' => "+02:30"      ],
	[ 'date' => 2023.113,              'timezone_type' => 3,   'timezone' => "Europe/Kyiv" ],
	[ 'date' => "2023-01-13 12:29:30", 'timezone_type' => 1.4, 'timezone' => "+02:30"      ],
	[ 'date' => "2023-01-13 12:29:30", 'timezone_type' => 3.4, 'timezone' => "Europe/Kyiv" ],
	[ 'date' => "2023-01-13 12:29:30", 'timezone_type' => 1,   'timezone' => 2.5           ],
	[ 'date' => "2023-01-13 12:29:30", 'timezone_type' => 3,   'timezone' => 2.5           ],
	[ 'date' => "2023-01-13 12:29:30", 'timezone_type' => 3,   'timezone' => "Europe/Lviv" ],
	[ 'date' => "2023-01-13 12:29:30", 'timezone_type' => 4,   'timezone' => "Europe/Kyiv" ],
];

foreach( $propertySets as $propertySet )
{
	try {
		$d = DateTimeImmutable::__set_state( $propertySet );
		echo "OK\n";
	} catch (\Error $e) {
		echo $e::class, ': ', $e->getMessage(), "\n";
	}
}
?>
--EXPECT--
OK
OK
Error: Invalid serialization data for DateTimeImmutable object
Error: Invalid serialization data for DateTimeImmutable object
Error: Invalid serialization data for DateTimeImmutable object
Error: Invalid serialization data for DateTimeImmutable object
Error: Invalid serialization data for DateTimeImmutable object
Error: Invalid serialization data for DateTimeImmutable object
Error: Invalid serialization data for DateTimeImmutable object
Error: Invalid serialization data for DateTimeImmutable object
