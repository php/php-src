--TEST--
Test basic date_sun_info()
--INI--
date.timezone=UTC
--FILE--
<?php
$sun_info = date_sun_info(strtotime("2006-12-12"), 31.7667, 35.2333);
var_dump($sun_info);
echo "Done\n";
?>
--EXPECTF--	
array(9) {
  ["sunrise"]=>
  int(1165899133)
  ["sunset"]=>
  int(1165934481)
  ["transit"]=>
  int(1165916807)
  ["civil_twilight_begin"]=>
  int(1165897449)
  ["civil_twilight_end"]=>
  int(1165936165)
  ["nautical_twilight_begin"]=>
  int(1165895547)
  ["nautical_twilight_end"]=>
  int(1165938067)
  ["astronomical_twilight_begin"]=>
  int(1165893693)
  ["astronomical_twilight_end"]=>
  int(1165939921)
}
Done
