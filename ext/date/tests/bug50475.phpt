--TEST--
Bug #50475 (DateTime::setISODate followed by DateTime::setTime)
--FILE--
<?php
date_default_timezone_set('Asia/Calcutta');

$date = new DateTime('18-01-2009 00:00:00');

$date->setISODate(2009, 6, 1);

var_dump($date->format('Y-m-d H:i:s'));

$date->setTime(8, 0);
var_dump($date->format('Y-m-d H:i:s'));
?>
--EXPECT--
string(19) "2009-02-02 00:00:00"
string(19) "2009-02-02 08:00:00"
