--TEST--
Test octdec() - basic function test octdec()
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
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
    try {
        $res = octdec($values[$i]);
        var_dump($res);
    } catch (ValueError $e) {
        echo 'ValueError: ', $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
ValueError: Invalid characters passed for attempted conversion
int(253)
ValueError: Invalid characters passed for attempted conversion
ValueError: Invalid characters passed for attempted conversion
ValueError: Invalid characters passed for attempted conversion
int(5349)
int(342391)
int(375)
int(2147483647)
float(2147483648)
ValueError: Invalid characters passed for attempted conversion
int(5349)
ValueError: Invalid characters passed for attempted conversion
int(823384)
int(1)
int(0)
