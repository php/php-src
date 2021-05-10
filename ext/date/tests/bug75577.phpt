--TEST--
Bug #75577 (DateTime::createFromFormat does not accept RFC3339_EXTENDED format)
--CREDITS--
Mariano Iglesias <mariano@cricava.com>
--FILE--
<?php
date_default_timezone_set('America/Buenos_Aires');

$date = DateTime::createFromFormat(DateTime::RFC3339_EXTENDED, '2017-06-03T00:08:10.881UTC');
var_dump($date->format(DateTime::RFC3339_EXTENDED));
var_dump($date->format('u'));
var_dump($date->format('v'));

$date = DateTime::createFromFormat(DateTime::RFC3339_EXTENDED, '2009-09-28T09:45:31.918-03:00');
var_dump($date->format(DateTime::RFC3339_EXTENDED));
var_dump($date->format('u'));
var_dump($date->format('v'));
--EXPECT--
string(29) "2017-06-03T00:08:10.881+00:00"
string(6) "881000"
string(3) "881"
string(29) "2009-09-28T09:45:31.918-03:00"
string(6) "918000"
string(3) "918"
