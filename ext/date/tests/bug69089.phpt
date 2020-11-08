--TEST--
Bug #69089 (Add support for RFC3339 extended to DateTime::format)
--CREDITS--
Mariano Iglesias <mariano@cricava.com>
--FILE--
<?php
date_default_timezone_set('America/Buenos_Aires');

$date = new DateTime('2009-09-28 09:45:31.918312');

var_dump($date->format(DateTime::RFC3339_EXTENDED));
var_dump($date->format('u'));
var_dump($date->format('v'));
?>
--EXPECT--
string(29) "2009-09-28T09:45:31.918-03:00"
string(6) "918312"
string(3) "918"
