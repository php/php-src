--TEST--
Bug GH-15582: Crash when not calling parent constructor of DateTimeZone
--FILE--
<?php
class MyDateTimeZone extends DateTimeZone
{
	function __construct()
	{
	}
}

$mdtz = new MyDateTimeZone();
$fusion = $mdtz;
try {
	date_create("2005-07-14 22:30:41", $fusion);
} catch (Error $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: The DateTimeZone object has not been correctly initialized by its constructor
