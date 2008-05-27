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
--EXPECT--
array(9) {
  [u"sunrise"]=>
  int(1165897782)
  [u"sunset"]=>
  int(1165934168)
  [u"transit"]=>
  int(1165915975)
  [u"civil_twilight_begin"]=>
  int(1165896176)
  [u"civil_twilight_end"]=>
  int(1165935773)
  [u"nautical_twilight_begin"]=>
  int(1165894353)
  [u"nautical_twilight_end"]=>
  int(1165937597)
  [u"astronomical_twilight_begin"]=>
  int(1165892570)
  [u"astronomical_twilight_end"]=>
  int(1165939380)
}
Done
