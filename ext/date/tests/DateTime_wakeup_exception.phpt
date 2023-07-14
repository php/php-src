--TEST--
DateTime invalid serialization data (wakeup)
--FILE--
<?php
$propertySets = [
	'O:8:"DateTime":3:{s:4:"date";s:26:"2023-01-13 14:48:01.705516";s:13:"timezone_type";i:3;s:8:"timezone";s:13:"Europe/London";}',
	'O:8:"DateTime":3:{s:4:"date";i:3;s:13:"timezone_type";i:3;s:8:"timezone";s:3:"UTC";}',
	'O:8:"DateTime":3:{s:4:"date";s:26:"2023-01-13 14:48:01.705516";s:13:"timezone_type";s:1:"X";s:8:"timezone";s:3:"UTC";}',
	'O:8:"DateTime":3:{s:4:"date";s:26:"2023-01-13 14:48:01.705516";s:13:"timezone_type";i:4;s:8:"timezone";s:3:"UTC";}',
	'O:8:"DateTime":3:{s:4:"date";s:26:"2023-01-13 14:48:01.705516";s:13:"timezone_type";i:3;s:8:"timezone";s:11:"Europe/Lviv";}',
	'O:8:"DateTime":3:{s:4:"date";s:26:"2023-01-13 14:48:01.705516";s:13:"timezone_type";i:3;s:8:"timezone";s:11:"Europe/Kyiv";}',
	'O:8:"DateTime":3:{s:4:"date";s:26:"2023-01-13 14:48:01.705516";s:13:"timezone_type";i:3;s:8:"timezone";s:1:"Europe/Kyiv";}',
];

foreach( $propertySets as $propertySet )
{
	try {
		$d = unserialize($propertySet);
		echo "OK? ", gettype($d), "\n";
	} catch (\Error $e) {
		echo $e::class, ': ', $e->getMessage(), "\n";
	}
}
?>
--EXPECTF--
OK? object
Error: Invalid serialization data for DateTime object
Error: Invalid serialization data for DateTime object
Error: Invalid serialization data for DateTime object
Error: Invalid serialization data for DateTime object
OK? object

Warning: unserialize(): Error at offset 109 of 122 bytes in %sDateTime_wakeup_exception.php on line 15
OK? boolean
