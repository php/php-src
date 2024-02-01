--TEST--
Test unserialization of DateTimeZone with null byte 
--FILE--
<?php
$serialized = 'O:12:"DateTimeZone":2:{s:13:"timezone_type";i:3;s:8:"timezone";s:17:"Ame' . "\0" .'rica/New_York";}';

try {
	$tz = unserialize($serialized);
} catch (Throwable $e) {
	echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Warning: DateTimeZone::__unserialize(): Timezone must not contain null bytes in %s on line %d
Invalid serialization data for DateTimeZone object
