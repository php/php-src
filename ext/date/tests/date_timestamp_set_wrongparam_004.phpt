--TEST--
Check the function date_timestamp_set() with 3 parameters.
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--INI--
date.timezone = UTC;
date_default_timezone_set("America/Sao_Paulo");
--FILE--
<?php
$dftz021 = date_default_timezone_get(); //UTC

$dtms021 = new DateTime();

date_timestamp_set($dtms021, 123456789, 'error');
?>
--EXPECTF--
Warning: date_timestamp_set() expects exactly 2 parameters, 3 given in %s on line %d
