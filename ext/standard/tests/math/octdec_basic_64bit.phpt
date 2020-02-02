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
    try {
        $res = octdec($values[$i]);
    } catch (InvalidArgumentException $e) {
        $res = 'INVALID';
    }
	var_dump($res);
}
?>
--EXPECT--
*** Testing octdec() : basic functionality ***
string(7) "INVALID"
int(253)
string(7) "INVALID"
string(7) "INVALID"
string(7) "INVALID"
int(5349)
int(342391)
int(375)
int(2147483647)
int(2147483648)
string(7) "INVALID"
int(5349)
string(7) "INVALID"
int(823384)
int(1)
int(0)
int(0)
