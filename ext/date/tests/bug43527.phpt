--TEST--
Bug #43527 (DateTime created from a timestamp reports environment timezone)
--FILE--
<?php
date_default_timezone_set("Etc/GMT+1");
$datetime = new DateTime('Fri, 07 Dec 2007 19:05:14 +1000');
echo $datetime->getTimezone()->getName(), "\n";
?>
--EXPECT--
+10:00
