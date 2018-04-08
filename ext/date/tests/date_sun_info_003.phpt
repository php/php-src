--TEST--
Test basic date_sun_info()
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--FILE--
<?php
date_default_timezone_set('America/Sao_Paulo');
$sun_info = date_sun_info(strtotime("2015-01-12 00:00:00 UTC"), 89.00, 1.00);
foreach ($sun_info as $key => $elem ) {
    echo "$key: " . date("H:i:s", $elem) . "\n";
}

echo "\n";

$sun_info = date_sun_info(strtotime("2015-09-12 00:00:00 UTC"), 89.00, 1.00);
foreach ($sun_info as $key => $elem ) {
    echo "$key: " . date("H:i:s", $elem) . "\n";
}

echo "Done\n";
?>
--EXPECT--
sunrise: 21:00:00
sunset: 21:00:00
transit: 10:03:48
civil_twilight_begin: 21:00:00
civil_twilight_end: 21:00:00
nautical_twilight_begin: 21:00:00
nautical_twilight_end: 21:00:00
astronomical_twilight_begin: 21:00:00
astronomical_twilight_end: 21:00:00

sunrise: 21:00:01
sunset: 21:00:01
transit: 08:52:44
civil_twilight_begin: 21:00:01
civil_twilight_end: 21:00:01
nautical_twilight_begin: 21:00:01
nautical_twilight_end: 21:00:01
astronomical_twilight_begin: 21:00:01
astronomical_twilight_end: 21:00:01
Done
