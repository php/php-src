--TEST--
Bug #28599 (strtotime fails with zero base time)
--FILE--
<?php
date_default_timezone_set("Europe/Amsterdam");
print gmdate("d-m-Y H:i:s", strtotime("+30 minutes", 1100535573));
?>
--EXPECT--
15-11-2004 16:49:33
