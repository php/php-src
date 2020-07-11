--TEST--
Test hexdec() - basic function test hexdec()
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
$values = array(0x123abc,
                0x789DEF,
                0x7FFFFFFF,
                0x80000000,
                '0x123abc',
                '0x789DEF',
                '0x7FFFFFFF',
                '0x80000000',
                '0x123XYZABC',
                311015,
                '311015',
                31101.3,
                31.1013e5,
                011237,
                '011237',
                true,
                false,
                null);
for ($i = 0; $i < count($values); $i++) {
    $res = hexdec($values[$i]);
    var_dump($res);
}
?>
--EXPECTF--
int(18433668)
int(126895953)
float(142929835591)
float(142929835592)
int(1194684)
int(7904751)
int(2147483647)
float(2147483648)

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(1194684)
int(3215381)
int(3215381)

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(3215379)
int(51446064)
int(18279)
int(70199)
int(1)
int(0)
int(0)
