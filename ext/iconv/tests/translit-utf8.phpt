--TEST--
Translit UTF-8 quotes
--SKIPIF--
<?php include('skipif.inc'); ?>
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
