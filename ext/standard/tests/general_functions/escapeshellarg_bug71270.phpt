--TEST--
Test escapeshellarg() allowed argument length
--FILE--
<?php
ini_set('memory_limit', -1);
$var_2  = str_repeat('A', 1024*1024*64);
escapeshellarg($var_2);

?>
===DONE===
--EXPECTF--
Fatal error: escapeshellarg(): Argument exceeds the allowed length of %d bytes in %s on line %d
