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
                );

for ($i = 0; $i < count($values); $i++) {
    $res = octdec($values[$i]);
    var_dump($res);
}
?>
--EXPECTF--
*** Testing octdec() : basic functionality ***

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(14489)
int(253)

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(36947879)

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(4618484)

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(4104)
int(5349)
int(342391)
int(375)
int(2147483647)
int(2147483648)

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(668)
int(5349)

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(102923)
int(823384)
int(1)
int(0)
