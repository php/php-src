--TEST--
Test basic date_sun_info()
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--FILE--
<?php
date_default_timezone_set('America/Sao_Paulo');

function print_sun_info(string $date) {
    echo $date, "\n";
    $sun_info = date_sun_info(strtotime($date), 89.00, 1.00);
    foreach ($sun_info as $key => $elem ) {
        echo "$key: " . match ($elem) {
            true => 'always',
            false => 'never',
            default => date("H:i:s", $elem),
        } . "\n";
    }
}

print_sun_info("2015-01-12 00:00:00 UTC");
echo "\n";
print_sun_info("2015-09-12 00:00:00 UTC");

?>
--EXPECT--
2015-01-12 00:00:00 UTC
sunrise: never
sunset: never
transit: 10:03:48
civil_twilight_begin: never
civil_twilight_end: never
nautical_twilight_begin: never
nautical_twilight_end: never
astronomical_twilight_begin: never
astronomical_twilight_end: never

2015-09-12 00:00:00 UTC
sunrise: always
sunset: always
transit: 08:52:44
civil_twilight_begin: always
civil_twilight_end: always
nautical_twilight_begin: always
nautical_twilight_end: always
astronomical_twilight_begin: always
astronomical_twilight_end: always
