--TEST--
Test basic date_sun_info()
--FILE--
<?php
date_default_timezone_set('UTC');
$sun_info = date_sun_info(strtotime("2006-12-12"), 31.7667, 35.2333);
var_dump($sun_info);
echo "Done\n";
?>
--EXPECT--
array(9) {
  ["sunrise"]=>
  int(1165897682)
  ["sunset"]=>
  int(1165934239)
  ["transit"]=>
  int(1165915961)
  ["civil_twilight_begin"]=>
  int(1165896156)
  ["civil_twilight_end"]=>
  int(1165935765)
  ["nautical_twilight_begin"]=>
  int(1165894334)
  ["nautical_twilight_end"]=>
  int(1165937588)
  ["astronomical_twilight_begin"]=>
  int(1165892551)
  ["astronomical_twilight_end"]=>
  int(1165939371)
}
Done
