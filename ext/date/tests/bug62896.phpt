--TEST--
Bug #62896 Unixtimestamp may take on local times DST flag (this test will only be valid during CEST)
--INI--
date.timezone=Europe/Berlin
--FILE--
<?php
  $tz = new DateTimeZone('Europe/Berlin');

  echo "FROM TIMESTAMP, NO TZ:\n";

  $date = new DateTime('@'.strtotime('2012-08-22 00:00:00 CEST'));
  echo $date->format('Y-m-d H:i:s T').' (offset '.$date->getOffset().")\n";

  $date->modify('+0 days');
  echo $date->format('Y-m-d H:i:s T').' (offset '.$date->getOffset().")\n";

  echo "FROM TIMESTAMP, WITH TZ:\n";

  $date = new DateTime('@'.strtotime('2012-08-22 00:00:00 CEST'));
  $date->setTimezone($tz);
  echo $date->format('Y-m-d H:i:s T').' (offset '.$date->getOffset().")\n";

  $date->modify('+0 days');
  echo $date->format('Y-m-d H:i:s T').' (offset '.$date->getOffset().")\n";

  echo "FROM STRING:\n";

  $date = new DateTime('2012-08-22 00:00:00 CEST', $tz);
  echo $date->format('Y-m-d H:i:s T').' (offset '.$date->getOffset().")\n";

  $date->modify('+0 days');
  echo $date->format('Y-m-d H:i:s T').' (offset '.$date->getOffset().")\n";
?>
--EXPECT--
FROM TIMESTAMP, NO TZ:
2012-08-21 22:00:00 GMT+0000 (offset 0)
2012-08-21 22:00:00 GMT+0000 (offset 0)
FROM TIMESTAMP, WITH TZ:
2012-08-22 00:00:00 CEST (offset 7200)
2012-08-22 00:00:00 CEST (offset 7200)
FROM STRING:
2012-08-22 00:00:00 CEST (offset 7200)
2012-08-22 00:00:00 CEST (offset 7200)
