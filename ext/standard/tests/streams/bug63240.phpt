--TEST--
Bug #63240: stream_get_line() return contains delimiter string
--FILE--
<?php
$fd = fopen('php://temp', 'r+');
$delimiter = 'MM';
$str = str_repeat('.', 8191) . $delimiter . "rest";
fwrite($fd, $str);
rewind($fd);
$line = stream_get_line($fd, 9000, $delimiter);
var_dump(strlen($line));
$line = stream_get_line($fd, 9000, $delimiter);
var_dump($line);
?>
--EXPECT--
int(8191)
string(4) "rest"
