--TEST--
Bug #71882 (Negative ftruncate() on php://memory exhausts memory)
--FILE--
<?php
$fd = fopen("php://memory", "w+");
var_dump(ftruncate($fd, -1));
?>
==DONE==
--EXPECTF--
Warning: ftruncate(): Negative size is not supported in %s%ebug71882.php on line %d
bool(false)
==DONE==
