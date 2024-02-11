--TEST--
GH-13344 (finfo::buffer(): Failed identify data 0:(null))
--EXTENSIONS--
fileinfo
--FILE--
<?php
$data = pack('H*','fffe000000099999');
$mime_type = (new finfo(FILEINFO_MIME))->buffer($data);
echo $mime_type;
?>
--EXPECT--
application/octet-stream; charset=utf-32le
