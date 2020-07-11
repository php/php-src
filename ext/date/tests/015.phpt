--TEST--
timezone object reference handling
--FILE--
<?php
date_default_timezone_set('UTC');
$dto = new DateTime();
$tzold = $dto->getTimezone();
var_dump($tzold->getName());
$dto->setTimezone(new DateTimeZone('US/Eastern'));
var_dump($tzold->getName());
var_dump($dto->getTimezone()->getName());
unset($dto);
var_dump($tzold->getName());
echo "Done\n";
?>
--EXPECT--
string(3) "UTC"
string(3) "UTC"
string(10) "US/Eastern"
string(3) "UTC"
Done
