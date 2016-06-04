--TEST--
Test escapeshellcmd() allowed argument length
--FILE--
<?php
ini_set('memory_limit', -1);
$var_2  = str_repeat('A', 1024*1024*64);
escapeshellcmd($var_2);

?>
===DONE===
--EXPECTF--
Fatal error: escapeshellcmd(): Command exceeds the allowed length of %d bytes in %s on line %d
