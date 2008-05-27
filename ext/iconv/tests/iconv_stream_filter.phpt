--TEST--
iconv stream filter
--SKIPIF--
<?php extension_loaded('iconv') or die('skip iconv extension is not available'); ?>
--INI--
iconv.internal_charset=iso-8859-1
--FILE--
<?php
$fp = fopen(dirname(__FILE__).'/iconv_stream_filter.txt', 'rb');
var_dump(bin2hex(fread($fp, 10)));
var_dump(bin2hex(fread($fp, 5)));
var_dump(bin2hex(fread($fp, 1)));
fclose($fp);

$fp = fopen(dirname(__FILE__).'/iconv_stream_filter.txt', 'rb');
stream_filter_append($fp, 'convert.iconv.ISO-2022-JP/EUC-JP');
var_dump(bin2hex(fread($fp, 10)));
var_dump(bin2hex(fread($fp, 5)));
var_dump(bin2hex(fread($fp, 1)));
fclose($fp);

$fp = fopen(dirname(__FILE__).'/iconv_stream_filter.txt', 'rb');
stream_filter_append($fp, 'string.rot13'); // this will make conversion fail.
stream_filter_append($fp, 'convert.iconv.ISO-2022-JP/EUC-JP');
var_dump(bin2hex(fread($fp, 10)));
var_dump(bin2hex(fread($fp, 5)));
var_dump(bin2hex(fread($fp, 1)));
fclose($fp);
?>
--EXPECTF--
unicode(20) "1b244224332473244b24"
unicode(10) "41244f1b28"
unicode(2) "42"
unicode(20) "a4b3a4f3a4cba4c1a4cf"
unicode(10) "69636f6e76"
unicode(2) "0a"

Warning: fread(): iconv stream filter ("ISO-2022-JP"=>"EUC-JP"): invalid multibyte sequence in %s on line %d
unicode(0) ""
unicode(0) ""
unicode(0) ""
