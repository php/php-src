--TEST--
Translit UTF-8 quotes
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php include('003.inc'); ?>
--EXPECT--
"Hello"
`Hello´
"Hello"
`Hello´
"Hello"
'Hello'
"Hello"
'Hello'
