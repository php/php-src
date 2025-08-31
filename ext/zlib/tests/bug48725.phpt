--TEST--
Bug #48725 (Support for flushing in zlib stream)
--EXTENSIONS--
zlib
--FILE--
<?php
$text = str_repeat('0123456789abcdef', 1000);

$temp = fopen('php://temp', 'r+');
stream_filter_append($temp, 'zlib.deflate', STREAM_FILTER_WRITE);
fwrite($temp, $text);

rewind($temp);

var_dump(bin2hex(stream_get_contents($temp)));
var_dump(ftell($temp));

fclose($temp);
?>
--EXPECT--
string(138) "ecc7c901c0100000b09594bac641d97f840e22f9253c31bdb9d4d6c75cdf3ec1ddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddaffc0f0000ffff"
int(69)
