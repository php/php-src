--TEST--
trim() function
--POST--
--GET--
--FILE--
<?php

$tests = <<<TESTS
'abc' ===  trim('abc')
'abc' === ltrim('abc')
'abc' === rtrim('abc')
'abc' ===  trim(" \0\t\nabc \0\t\n")
"abc \0\t\n" === ltrim(" \0\t\nabc \0\t\n")
" \0\t\nabc" === rtrim(" \0\t\nabc \0\t\n")
" \0\t\nabc \0\t\n" ===  trim(" \0\t\nabc \0\t\n",'')
" \0\t\nabc \0\t\n" === ltrim(" \0\t\nabc \0\t\n",'')
" \0\t\nabc \0\t\n" === rtrim(" \0\t\nabc \0\t\n",'')
"abc\x50\xC1" === trim("abc\x50\xC1\x60\x90","\x51..\xC0")
"abc\x50" === trim("abc\x50\xC1\x60\x90","\x51..\xC1")
"abc" === trim("abc\x50\xC1\x60\x90","\x50..\xC1")
"abc\x50\xC1" === trim("abc\x50\xC1\x60\x90","\x51..\xC0")
"abc\x50" === trim("abc\x50\xC1\x60\x90","\x51..\xC1")
"abc" === trim("abc\x50\xC1\x60\x90","\x50..\xC1")
TESTS;

include('../../../../tests/quicktester.inc');

--EXPECT--
OK
