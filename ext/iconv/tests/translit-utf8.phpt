--TEST--
Translit UTF-8 quotes
--SKIPIF--
<?php
include('skipif.inc');
( ICONV_IMPL != "libiconv" ) and die("skip ICONV_IMPL != \"libiconv\""); 
?>
--INI--
error_reporting=2047
--FILE--
<?php // vim600: syn=php
include('test.inc');
//error_reporting(E_ALL);
$utf = implode('', file('ext/iconv/tests/Quotes.UTF-8'));

print(iconv("UTF-8", "ISO-8859-1//TRANSLIT", $utf));
print(iconv("UTF-8", "ASCII//TRANSLIT", $utf));
?>
--EXPECT--
"Hello"
`Hello´
"Hello"
`Hello´
"Hello"
'Hello'
"Hello"
'Hello'
