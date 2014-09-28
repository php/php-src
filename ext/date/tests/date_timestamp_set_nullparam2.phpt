--TEST--
Test the function date_timestamp_set() with second null parameter.
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--INI--
date.timezone = UTC;
date_default_timezone_set("America/Sao_Paulo"); 
--FILE--
<?php
$dftz021 = date_default_timezone_get(); //UTC

$dtms021 = date_create(); 

var_dump(date_timestamp_set($dtms021, null));
?>
--EXPECTF--
object(DateTime)#1 (3) {
  ["date"]=>
  string(26) "1970-01-01 00:00:00.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}