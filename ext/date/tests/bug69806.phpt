--TEST--
Bug #69806
--FILE--
<?php
ini_set('date.timezone', 'America/New_York');

echo date('Y-m-d H:i:s', 2377224000)."\n";
?>
--EXPECT--
2045-05-01 00:00:00
