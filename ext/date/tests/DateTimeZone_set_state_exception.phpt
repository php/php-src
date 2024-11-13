--TEST--
DateTimeZone invalid serialization data
--FILE--
<?php
$propertySets = [
	[ 'timezone_type' => 1,   'timezone' => "+02:30"      ],
	[ 'timezone_type' => 3,   'timezone' => "Europe/Kyiv" ],
	[ 'timezone_type' => 1.4, 'timezone' => "+02:30"      ],
	[ 'timezone_type' => 3.4, 'timezone' => "Europe/Kyiv" ],
	[ 'timezone_type' => 1,   'timezone' => 2.5           ],
	[ 'timezone_type' => 3,   'timezone' => 2.5           ],
	[ 'timezone_type' => 3,   'timezone' => "Europe/K\0v" ],
	[ 'timezone_type' => 3,   'timezone' => "99:99:99"    ],
	[ 'timezone_type' => 3,   'timezone' => "Europe/Lviv" ],
	[ 'timezone_type' => 4,   'timezone' => "Europe/Kyiv" ],
];

foreach( $propertySets as $propertySet )
{
	try {
		$d = DateTimeZone::__set_state( $propertySet );
		echo "OK\n";
	} catch (\Error $e) {
		echo $e::class, ': ', $e->getMessage(), "\n";
	}
}
?>
--EXPECT--
OK
OK
Error: Invalid serialization data for DateTimeZone object
Error: Invalid serialization data for DateTimeZone object
Error: Invalid serialization data for DateTimeZone object
Error: Invalid serialization data for DateTimeZone object
Error: Invalid serialization data for DateTimeZone object
Error: Invalid serialization data for DateTimeZone object
Error: Invalid serialization data for DateTimeZone object
Error: Invalid serialization data for DateTimeZone object
