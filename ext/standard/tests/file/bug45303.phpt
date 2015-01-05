--TEST--
Bug #45303 (Opening php:// wrapper in append mode results in a warning)
--FILE--
<?php
$fd = fopen("php://stdout","a");
var_dump($fd);
var_dump(fseek($fd, 1024*1024, SEEK_SET));
?>
--EXPECTF--
resource(%d) of type (stream)

Warning: fseek(): stream does not support seeking in %s
int(-1)
