--TEST--
Test str_icontains() function
--FILE--
<?php
var_dump(str_icontains("test string", "test"));
var_dump(str_icontains("test string", "string"));
var_dump(str_icontains("test string", "strin"));
var_dump(str_icontains("test string", "t s"));
var_dump(str_icontains("test string", "g"));
var_dump(str_icontains("te".chr(0)."st", chr(0)));
var_dump(str_icontains("tEst", "test"));
var_dump(str_icontains("teSt", "test"));
var_dump(str_icontains("TEST", "test"));
var_dump(str_icontains("test", "TEST"));
var_dump(str_icontains("TESST", "TEST"));
var_dump(str_icontains("", ""));
var_dump(str_icontains("a", ""));
var_dump(str_icontains("", "a"));
var_dump(str_icontains("\\\\a", "\\a"));
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
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
