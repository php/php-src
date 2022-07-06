--TEST--
Test date_format() function : timezone offset
--FILE--
<?php

$tz = array("UTC", "Europe/London", "Europe/Berlin", "America/Chicago");

foreach ($tz as $zone) {
    echo $zone, "\n";
    date_default_timezone_set($zone);

    $date = date_create("2020-03-10 22:30:41");

    var_dump( date_format($date, "O") );
    var_dump( date_format($date, "P") );
    var_dump( date_format($date, "p") );
}

echo "Z\n";
$date = date_create("2020-03-10 22:30:41Z");

var_dump( date_format($date, "p") );

?>
--EXPECT--
UTC
string(5) "+0000"
string(6) "+00:00"
string(1) "Z"
Europe/London
string(5) "+0000"
string(6) "+00:00"
string(6) "+00:00"
Europe/Berlin
string(5) "+0100"
string(6) "+01:00"
string(6) "+01:00"
America/Chicago
string(5) "-0500"
string(6) "-05:00"
string(6) "-05:00"
Z
string(1) "Z"
