--TEST--
DateInterval invalid serialization data with date_string
--FILE--
<?php
$propertySets = [
	'2023-01-16 17:01:19',
	'2023-01-16-foobar$*',
];

foreach( $propertySets as $propertySet )
{
	try {
		$d = DateInterval::__set_state( [ 'date_string' => $propertySet ] );
		echo "OK\n";
	} catch (\Error $e) {
		echo $e::class, ': ', $e->getMessage(), "\n";
	}
}
?>
--EXPECT--
OK
Error: Unknown or bad format (2023-01-16-foobar$*) at position 10 (-) while unserializing: Unexpected character
