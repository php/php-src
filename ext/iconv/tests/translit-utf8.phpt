--TEST--
Translit UTF-8 quotes
--SKIPIF--
<?php
include('skipif.inc');
( ICONV_IMPL != "libiconv" ) and die("skip"); 
?>
--FILE--
<?php include('translit-utf8.inc'); ?>
--EXPECT--
"Hello"
`Hello´
"Hello"
`Hello´
"Hello"
'Hello'
"Hello"
'Hello'
