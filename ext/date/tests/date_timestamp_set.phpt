--TEST--
Test the basics to function date_timestamp_set().
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--INI--
date.timezone = UTC;
date_default_timezone_set("America/Sao_Paulo"); 
--FILE--
<?php
$dftz021 = date_default_timezone_get(); //UTC

$dtms021 = date_create(); 

date_timestamp_set($dtms021, 1234567890); 

var_dump(date_format($dtms021, 'B => (U) => T Y-M-d H:i:s'));
?>
--EXPECTF--
string(47) "021 => (1234567890) => UTC 2009-Feb-13 23:31:30"