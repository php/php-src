--TEST--
Test bindec() - basic function test bindec()
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--INI--
precision=14
--FILE--
<?php
$values = array(111000111,
				011100000,
				1111111111111111111111111111111,
				10000000000000000000000000000000,
				100002001,
				'111000111',
				'011100000',
				'1111111111111111111111111111111',
				'10000000000000000000000000000000',
				'100002001',
				'abcdefg',
				311015,
				31101.3,
				31.1013e5,
				0x111ABC,
				011237,				
				true,
				false,
				null);	

for ($i = 0; $i < count($values); $i++) {
	$res = bindec($values[$i]);
	var_dump($res);
}
?>
--EXPECTF--
int(455)
int(0)
int(32766)
int(5)
int(129)
int(455)
int(224)
int(2147483647)
int(2147483648)
int(129)
int(0)
int(13)
int(13)
int(26)
int(6)
int(0)
int(1)
int(0)
int(0)
