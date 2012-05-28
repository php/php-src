--TEST--
bug 62166 test
--FILE--
<?php
date_default_timezone_set('UTC');
var_export(DateTime::createFromFormat("Y-m-d","2012-05-11"));
var_export(new DateTime("2012-05-11"));
var_dump(DateTime::createFromFormat("Y-m-d","2012-05-11")===new DateTime("2012-05-11"));
echo "\n";
echo "Done\n";
?>
--EXPECTF--	
DateTime::__set_state(array(
   'date' => '2012-05-11 09:11:06',
   'timezone_type' => 3,
   'timezone' => 'UTC',
))DateTime::__set_state(array(
   'date' => '2012-05-11 00:00:00',
   'timezone_type' => 3,
   'timezone' => 'UTC',
))bool(false)
Done