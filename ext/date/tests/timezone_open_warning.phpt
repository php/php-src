--TEST--
timezone_open() invalid timezones
--FILE--
<?php
$timezones = [ "+02:30", "Europe/Kyiv", 2.5, /*"Europe/K\0v",*/ "99:60", "Europe/Lviv" ];

foreach( $timezones as $timezone )
{
	$d = timezone_open( $timezone );
	if ($d) {
		echo "In: {$timezone}; Out: ", $d->getName(), "\n";
	}
}
?>
--EXPECTF--
In: +02:30; Out: +02:30
In: Europe/Kyiv; Out: Europe/Kyiv

Warning: timezone_open(): Unknown or bad timezone (2.5) in %stimezone_open_warning.php on line 6

Warning: timezone_open(): Unknown or bad timezone (99:60) in %stimezone_open_warning.php on line 6

Warning: timezone_open(): Unknown or bad timezone (Europe/Lviv) in %stimezone_open_warning.php on line 6
