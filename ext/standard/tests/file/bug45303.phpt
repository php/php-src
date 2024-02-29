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

Warning: fseek(Resource id #%d, 1048576, 0): Stream does not support seeking in %s on line %d
int(-1)
