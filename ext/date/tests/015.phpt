--TEST--
timezone object reference handling
--INI--
date.timezone=UTC
--FILE--
<?php
$dto = new DateTime();
$tzold = $dto->getTimezone();
var_dump($tzold->getName());
$dto->setTimezone(new DateTimeZone('US/Eastern'));
var_dump($tzold->getName());
var_dump($dto->getTimezone()->getName());
echo "Done\n";
?>
--EXPECTF--
unicode(3) "UTC"
unicode(3) "UTC"
unicode(10) "US/Eastern"
Done
