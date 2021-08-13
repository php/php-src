--TEST--
Bug #80913 (DateTime(Immutable)::sub around DST yield incorrect time)
--FILE--
<?php
$date = new DateTime('2021-03-28 03:00:00', new DateTimeZone('Europe/Amsterdam'));
$_30mbefore = (clone $date)->sub(new DateInterval('PT30M'));
$_30mafter = (clone $date)->add(new DateInterval('PT30M'));

var_dump($date->format(DATE_ATOM));
var_dump($_30mbefore->format(DATE_ATOM));
var_dump($_30mafter->format(DATE_ATOM));
?>
--EXPECT--
string(25) "2021-03-28T03:00:00+02:00"
string(25) "2021-03-28T01:30:00+01:00"
string(25) "2021-03-28T03:30:00+02:00"
