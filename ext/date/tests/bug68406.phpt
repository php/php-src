--TEST--
Bug #68406 calling var_dump on a DateTimeZone object modifies it
--INI--
date.timezone=UTC
--FILE--
<?php

$tz1 = new DateTimeZone('Europe/Berlin');
$tz2 = new DateTimeZone('Europe/Berlin');

$d = new DateTime('2014-12-24 13:00:00', $tz1);
var_dump($d->getTimezone(), $tz2);

if($tz2 == $d->getTimezone()) {
    echo "yes";
}
else {
    echo "no";
}
?>
--EXPECT--
object(DateTimeZone)#4 (2) {
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/Berlin"
}
object(DateTimeZone)#2 (2) {
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/Berlin"
}
yes
