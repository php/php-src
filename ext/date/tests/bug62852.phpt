--TEST--
Bug #62852 (Unserialize invalid DateTime causes crash), variation 1
--INI--
date.timezone=GMT
--FILE--
<?php
$s1 = 'O:8:"DateTime":3:{s:4:"date";s:20:"10007-06-07 03:51:49";s:13:"timezone_type";i:3;s:8:"timezone";s:3:"UTC";}';

try {
    unserialize( $s1 );
} catch ( Exception $e ) {}
?>
--EXPECTF--
Fatal error: Uncaught Error: Invalid serialization data for DateTime object in %sbug62852.php:%d
Stack trace:
#0 [internal function]: DateTime->__unserialize(Array)
#1 %sbug62852.php(%d): unserialize('O:8:"DateTime":...')
#2 {main}
  thrown in %sbug62852.php on line %d
