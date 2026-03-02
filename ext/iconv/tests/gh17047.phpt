--TEST--
GH-17047 (UAF on iconv filter failure)
--EXTENSIONS--
iconv
--FILE--
<?php
$stream = fopen('php://temp', 'w+');
stream_filter_append($stream, 'convert.iconv.UTF-16BE.UTF-8');
stream_filter_append($stream, 'convert.iconv.UTF-16BE.UTF-16BE');
fputs($stream, 'test');
rewind($stream);
var_dump(stream_get_contents($stream));
fclose($stream);
?>
--EXPECTF--
Warning: stream_get_contents(): iconv stream filter ("UTF-16BE"=>"UTF-16BE"): invalid multibyte sequence in %s on line %d
string(0) ""
