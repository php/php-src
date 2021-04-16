--TEST--
Bug #77660 (Segmentation fault on break 2147483648)
--PLATFORM--
bits: 64
--FILE--
<?php
for(;;) break 2147483648;
?>
--EXPECTF--
Fatal error: Cannot 'break' 2147483648 levels in %sbug77660.php on line %d
