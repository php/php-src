--TEST--
Test date_interval_create_from_date_string() function : nonsense data
--FILE--
<?php
$i = date_interval_create_from_date_string("foobar");
var_dump($i);
?>
--EXPECTF--
Warning: date_interval_create_from_date_string(): Unknown or bad format (foobar) at position 0 (f): The timezone could not be found in the database in %sdate_interval_create_from_date_string_broken.php on line 2
bool(false)
