--TEST--
Test hexdec() - basic function test hexdec()
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
echo "*** Testing hexdec() : basic functionality ***\n";

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
                );

foreach($values as $value) {
    echo "\n-- hexdec $value --\n";
    var_dump(hexdec($value));
};

?>
--EXPECTF--
*** Testing hexdec() : basic functionality ***

-- hexdec 1194684 --
int(18433668)

-- hexdec 7904751 --
int(126895953)

-- hexdec 2147483647 --
int(142929835591)

-- hexdec 2147483648 --
int(142929835592)

-- hexdec 0x123abc --
int(1194684)

-- hexdec 0x789DEF --
int(7904751)

-- hexdec 0x7FFFFFFF --
int(2147483647)

-- hexdec 0x80000000 --
int(2147483648)

-- hexdec 0x123XYZABC --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(1194684)

-- hexdec 311015 --
int(3215381)

-- hexdec 311015 --
int(3215381)

-- hexdec 31101.3 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(3215379)

-- hexdec 3110130 --
int(51446064)

-- hexdec 4767 --
int(18279)

-- hexdec 011237 --
int(70199)

-- hexdec 1 --
int(1)

-- hexdec  --
int(0)
