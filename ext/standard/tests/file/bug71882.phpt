--TEST--
Bug #71882 (Negative ftruncate() on php://memory exhausts memory)
--FILE--
<?php
$fd = fopen("php://memory", "w+");
var_dump(ftruncate($fd, -1));
?>
==DONE==
--EXPECTF--
bool(false)
==DONE==
