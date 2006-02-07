--TEST--
Bug #36306 crc32() 64bit
--FILE--
<?php
echo crc32("platform independant") . "\n";
?>
--EXPECT--
-858128794
