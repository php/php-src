--TEST--
Bug #62097: fix for bug #54547 is wrong for 32-bit machines
--SKIPIF--
<?php
if (PHP_INT_MAX !== 2147483647)
	die('skip for system with 32-bit wide longs only');
--FILE--
<?php
var_dump("02147483647" == "2147483647",
		 "02147483648" == "2147483648",
		 "09007199254740991" == "9007199254740991",
		 "09007199254740992" == "9007199254740992");
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
