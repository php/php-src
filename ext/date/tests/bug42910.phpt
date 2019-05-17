--TEST--
Bug #42910 (Constructing DateTime with TimeZone Indicator invalidates DateTimeZone)
--FILE--
<?php
	date_default_timezone_set('America/Los_Angeles');
	$foo = new DateTime('2007-03-11');
	$bar = new DateTime('2007-03-11T00:00:00-0800');

	print $foo->format(DateTime::ISO8601) . ' - ' .  $foo->getTimezone()->getName() . ' - ' . $foo->format('U') . "\r\n";
	print $bar->format(DateTime::ISO8601) . ' - ' .  $bar->getTimezone()->getName() . ' - ' . $bar->format('U') . "\r\n";

	$foo->setDate(2007, 03, 12);
	$bar->setDate(2007, 03, 12);

	print $foo->format(DateTime::ISO8601) . ' - ' .  $foo->getTimezone()->getName() . ' - ' . $foo->format('U') . "\r\n";
	print $bar->format(DateTime::ISO8601) . ' - ' .  $bar->getTimezone()->getName() . ' - ' . $bar->format('U') . "\r\n";

// --------------

    date_default_timezone_set('Australia/Sydney');

    $date= date_create('2007-11-04 12:00:00+0200');
    var_dump(date_format($date, 'O e'));
?>
--EXPECT--
2007-03-11T00:00:00-0800 - America/Los_Angeles - 1173600000
2007-03-11T00:00:00-0800 - -08:00 - 1173600000
2007-03-12T00:00:00-0700 - America/Los_Angeles - 1173682800
2007-03-12T00:00:00-0800 - -08:00 - 1173686400
string(12) "+0200 +02:00"
