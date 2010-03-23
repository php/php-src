--TEST--
Test octdec() - basic function test octdec()
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php

echo "*** Testing octdec() : basic functionality ***\n";

$values = array(01234567,
				0567,
				017777777777,
				020000000000,
				0x1234ABC,
				12345,
				'01234567',
				'0567',
				'017777777777',
				'020000000000',
				'0x1234ABC',
				'12345',
				31101.3,
				31.1013e5,				
				true,
				false,
				null);	

for ($i = 0; $i < count($values); $i++) {
	$res = octdec($values[$i]);
	var_dump($res);
}
?>
===Done===
--EXPECTF--
*** Testing octdec() : basic functionality ***
int(14489)
int(253)
int(36947879)
int(4618484)
int(4104)
int(5349)
int(342391)
int(375)
int(2147483647)
int(2147483648)
int(668)
int(5349)
int(102923)
int(823384)
int(1)
int(0)
int(0)
===Done===
