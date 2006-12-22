--TEST--
date_isodate_set() tests
--INI--
date.timezone=UTC
--FILE--
<?php

$dto = date_create("2006-12-12");
var_dump(date_isodate_set($dto, 2006, 2, 15));
var_dump($dto->format("Y/m/d H:i:s"));
var_dump(date_isodate_set($dto, 2006));
var_dump($dto->format("Y/m/d H:i:s"));
var_dump(date_isodate_set($dto, 2006, 5));
var_dump($dto->format("Y/m/d H:i:s"));
var_dump(date_isodate_set($dto, 2006, 100, 15));
var_dump($dto->format("Y/m/d H:i:s"));
var_dump(date_isodate_set($dto, 2006, 100, 15, 10));
var_dump($dto->format("Y/m/d H:i:s"));

echo "Done\n";
?>
--EXPECTF--
NULL
string(19) "2006/01/23 00:00:00"

Warning: date_isodate_set() expects at least 3 parameters, 2 given in %s on line %d
bool(false)
string(19) "2006/01/23 00:00:00"
NULL
string(19) "2006/01/30 00:00:00"
NULL
string(19) "2007/12/10 00:00:00"

Warning: date_isodate_set() expects at most 4 parameters, 5 given in %s on line %d
bool(false)
string(19) "2007/12/10 00:00:00"
Done
