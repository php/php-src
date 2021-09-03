--TEST--
iconv stream filter
--EXTENSIONS--
iconv
--INI--
iconv.internal_charset=iso-8859-1
--FILE--
<?php
$fp = fopen(__DIR__.'/iconv_stream_filter.txt', 'rb');
var_dump(bin2hex(fread($fp, 10)));
var_dump(bin2hex(fread($fp, 5)));
var_dump(bin2hex(fread($fp, 1)));
fclose($fp);

$fp = fopen(__DIR__.'/iconv_stream_filter.txt', 'rb');
stream_filter_append($fp, 'convert.iconv.ISO-2022-JP/EUC-JP');
var_dump(bin2hex(fread($fp, 10)));
var_dump(bin2hex(fread($fp, 5)));
var_dump(bin2hex(fread($fp, 1)));
fclose($fp);

$fp = fopen(__DIR__.'/iconv_stream_filter.txt', 'rb');
stream_filter_append($fp, 'string.rot13'); // this will make conversion fail.
stream_filter_append($fp, 'convert.iconv.ISO-2022-JP/EUC-JP');
var_dump(bin2hex(@fread($fp, 10)) != "a4b3a4f3a4cba4c1a4cf");
var_dump(bin2hex(fread($fp, 5)) != "69636f6e76");
var_dump(bin2hex(fread($fp, 1)) != "0a");
fclose($fp);
?>
--EXPECT--
string(20) "1b244224332473244b24"
string(10) "41244f1b28"
string(2) "42"
string(20) "a4b3a4f3a4cba4c1a4cf"
string(10) "69636f6e76"
string(2) "0a"
bool(true)
bool(true)
bool(true)
