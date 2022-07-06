--TEST--
Test date_interval_create_from_date_string() function : null parameter
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--FILE--
<?php
$i = date_interval_create_from_date_string(null);
var_dump($i);
?>
--EXPECTF--
Warning: date_interval_create_from_date_string(): Unknown or bad format () at position 0 ( ): Empty string in %sdate_interval_create_from_date_string_nullparam.php on line 2
bool(false)
