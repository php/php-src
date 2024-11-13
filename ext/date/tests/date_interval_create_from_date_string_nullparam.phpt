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
Deprecated: date_interval_create_from_date_string(): Passing null to parameter #1 ($datetime) of type string is deprecated in %s on line %d

Warning: date_interval_create_from_date_string(): Unknown or bad format () at position 0 ( ): Empty string in %s
bool(false)
