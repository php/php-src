--TEST--
Translit UTF-8 quotes
--EXTENSIONS--
iconv
--SKIPIF--
<?php
( ICONV_IMPL != "libiconv" ) and die("skip ICONV_IMPL != \"libiconv\"");
?>
--INI--
error_reporting=2047
--FILE--
<?php
//error_reporting(E_ALL);
$utf = implode('', file(__DIR__.'/Quotes.UTF-8.data'));

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
