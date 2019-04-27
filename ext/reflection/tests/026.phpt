--TEST--
ReflectionExtension::info()
--FILE--
<?php
$r = new ReflectionExtension("reflection");
$r->info();

date_default_timezone_set('Europe/Berlin');
$r = new ReflectionExtension("date");
$r->info();

echo "\nDone!\n";
?>
--EXPECTF--
Reflection

Reflection => enabled

date

date/time support => enabled
timelib version => %s
"Olson" Timezone Database Version => %s
Timezone Database => %s
Default timezone => %s

Directive => %s => %s
date.timezone => %s => %s
date.default_latitude => %s => %s
date.default_longitude => %s => %s
date.sunset_zenith => %s => %s
date.sunrise_zenith => %s => %s

Done!
