--TEST--
Bug #79200 (Some iconv functions cut Windows-1258)
--EXTENSIONS--
iconv
--FILE--
<?php
var_dump(iconv_mime_decode('=?windows-1258?Q?test=20test?=', 0, 'UTF-8'));
var_dump(iconv_strlen('test test', 'WINDOWS-1258'));
var_dump(iconv_strpos('test test', 'test test', 0, 'WINDOWS-1258'));
var_dump(iconv_substr('test test', 0 , 9, 'WINDOWS-1258'));
?>
--EXPECT--
string(9) "test test"
int(9)
int(0)
string(9) "test test"
