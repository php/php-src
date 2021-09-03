--TEST--
trim(), rtrim() and ltrim() functions
--FILE--
<?php

var_dump('ABC' ===  trim('ABC'));
var_dump('ABC' === ltrim('ABC'));
var_dump('ABC' === rtrim('ABC'));
var_dump('ABC' ===  trim(" \0\t\nABC \0\t\n"));
var_dump("ABC \0\t\n" === ltrim(" \0\t\nABC \0\t\n"));
var_dump(" \0\t\nABC" === rtrim(" \0\t\nABC \0\t\n"));
var_dump(" \0\t\nABC \0\t\n" ===  trim(" \0\t\nABC \0\t\n",''));
var_dump(" \0\t\nABC \0\t\n" === ltrim(" \0\t\nABC \0\t\n",''));
var_dump(" \0\t\nABC \0\t\n" === rtrim(" \0\t\nABC \0\t\n",''));
var_dump("ABC\x50\xC1" === trim("ABC\x50\xC1\x60\x90","\x51..\xC0"));
var_dump("ABC\x50" === trim("ABC\x50\xC1\x60\x90","\x51..\xC1"));
var_dump("ABC" === trim("ABC\x50\xC1\x60\x90","\x50..\xC1"));
var_dump("ABC\x50\xC1" === trim("ABC\x50\xC1\x60\x90","\x51..\xC0"));
var_dump("ABC\x50" === trim("ABC\x50\xC1\x60\x90","\x51..\xC1"));
var_dump("ABC" === trim("ABC\x50\xC1\x60\x90","\x50..\xC1"));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
