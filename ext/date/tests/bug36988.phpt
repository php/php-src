--TEST--
Bug #36988 (mktime freezes on long numbers)
--INI--
date.timezone=GMT
--FILE--
<?php
$start = microtime(true);
$a = mktime(1, 1, 1, 1, 1, 11111111111);
echo (microtime(true) - $start) < 1 ? "smaller than one second" : "more than a second";
?>
--EXPECT--
smaller than one second
