--TEST--
Test the function date_timestamp_set() with first null parameter.
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--INI--
date.timezone = UTC;
date_default_timezone_set("America/Sao_Paulo");
--FILE--
<?php
$dftz021 = date_default_timezone_get(); //UTC

$dtms021 = date_create();

date_timestamp_set(null, 1234567890);
?>
--EXPECTF--
Warning: date_timestamp_set() expects parameter 1 to be DateTime, null given in %s on line %d
