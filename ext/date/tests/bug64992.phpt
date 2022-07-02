--TEST--
Bug #64992 (dst not handled past 2038)
--FILE--
<?php
$firstyear = 2035;
$lastyear = 2040;
$tz = 'America/Los_Angeles';
date_default_timezone_set('America/Los_Angeles');

$dt = new DateTime((string) ($firstyear - 1) . "-07-02");
$di = new DateInterval('P6M');

for ($i = 0; $i < ($lastyear - $firstyear) * 2; $i++) {
	$dt->add($di);
	$gmto = $dt->getOffset();
	echo "Time Zone offset for $tz for " , $dt->format('Y-m-d') , " is $gmto\n";
}
?>
--EXPECT--
Time Zone offset for America/Los_Angeles for 2035-01-02 is -28800
Time Zone offset for America/Los_Angeles for 2035-07-02 is -25200
Time Zone offset for America/Los_Angeles for 2036-01-02 is -28800
Time Zone offset for America/Los_Angeles for 2036-07-02 is -25200
Time Zone offset for America/Los_Angeles for 2037-01-02 is -28800
Time Zone offset for America/Los_Angeles for 2037-07-02 is -25200
Time Zone offset for America/Los_Angeles for 2038-01-02 is -28800
Time Zone offset for America/Los_Angeles for 2038-07-02 is -25200
Time Zone offset for America/Los_Angeles for 2039-01-02 is -28800
Time Zone offset for America/Los_Angeles for 2039-07-02 is -25200
