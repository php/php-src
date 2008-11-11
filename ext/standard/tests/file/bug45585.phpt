--TEST--
Bug #45585 (fread() return value for EOF inconsistent between PHP 5 and 6)
--FILE--
<?php
$fd = fopen("php://temp","w+");
fwrite($fd, b"foo");
fseek($fd, 0, SEEK_SET);
var_dump($fd, fread($fd, 3), fread($fd, 3));
fclose($fd);
?>
--EXPECTF--
resource(%d) of type (stream)
string(3) "foo"
string(0) ""
