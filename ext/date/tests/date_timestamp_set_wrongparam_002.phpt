--TEST--
Check the function date_timestamp_set() with first parameter wrong (integer).
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--INI--
date.timezone = UTC;
date_default_timezone_set("America/Sao_Paulo"); 
--FILE--
<?php
$dftz021 = date_default_timezone_get(); //UTC

date_timestamp_set(987654321, 123456789);
?>
--EXPECTF--
Warning: date_timestamp_set() expects parameter 1 to be DateTime, integer given in %s on line %d