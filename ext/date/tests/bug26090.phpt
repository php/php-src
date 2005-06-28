--TEST--
Bug #26090 (allow colons in time zone offset to strtotime())
--FILE--
<?php
putenv("TZ=America/New_York");
$t = '2003-10-28 10:20:30-0800';
echo date('Y-m-d H:i:s T', strtotime($t)) . "\n";

$t = '2003-10-28 10:20:30-08:00';
echo date('Y-m-d H:i:s T', strtotime($t)) . "\n";
?>
--EXPECT--
2003-10-28 13:20:30 EST
2003-10-28 13:20:30 EST
