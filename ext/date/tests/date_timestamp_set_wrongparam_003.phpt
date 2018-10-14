--TEST--
Check the function date_timestamp_set() with second parameter wrong (array).
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--INI--
date.timezone = UTC;
date_default_timezone_set("America/Sao_Paulo");
--FILE--
<?php
$dftz021 = date_default_timezone_get(); //UTC

$dtms021 = new DateTime();

$wrong_parameter = array();

date_timestamp_set($dtms021, $wrong_parameter);
?>
--EXPECTF--
Warning: date_timestamp_set() expects parameter 2 to be integer, array given in %s on line %d
