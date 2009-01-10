--TEST--
Test basic date_sun_info()
--FILE--
<?php
date_default_timezone_set('UTC');
$sun_info = date_sun_info(strtotime("2006-12-12"), 31.7667, 35.2333);
var_dump($sun_info);
echo "Done\n";
?>
--EXPECTF--	
array(9) {
  ["sunrise"]=>
  int(1165897782)
  ["sunset"]=>
  int(1165934168)
  ["transit"]=>
  int(1165915975)
  ["civil_twilight_begin"]=>
  int(1165896176)
  ["civil_twilight_end"]=>
  int(1165935773)
  ["nautical_twilight_begin"]=>
  int(1165894353)
  ["nautical_twilight_end"]=>
  int(1165937597)
  ["astronomical_twilight_begin"]=>
  int(1165892570)
  ["astronomical_twilight_end"]=>
  int(1165939380)
}
Done
