--TEST--
DateTime::__construct() -- fall daylight/standard overlap
--CREDITS--
Daniel Convissor <danielc@php.net>
--FILE--
<?php

date_default_timezone_set('America/New_York');
// PHP defaults to Daylight Saving Time.  Ensure consistency in future.
$d = new DateTime('2011-11-06 01:30:00');
echo $d->format('P') . "\n";

date_default_timezone_set('Atlantic/Azores');
// PHP defaults to Daylight Saving Time.  Ensure consistency in future.
$d = new DateTime('2011-10-30 00:30:00');
echo $d->format('P') . "\n";

date_default_timezone_set('Europe/London');
// PHP defaults to normal time.  Ensure consistency in future.
$d = new DateTime('2011-10-30 01:30:00');
echo $d->format('P') . "\n";

date_default_timezone_set('Europe/Amsterdam');
// PHP defaults to normal time.  Ensure consistency in future.
$d = new DateTime('2011-10-30 02:30:00');
echo $d->format('P') . "\n";

?>
--EXPECT--
-04:00
+00:00
+00:00
+01:00
