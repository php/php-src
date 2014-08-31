--TEST--
Check gmp_import and gmp_export behave as intended
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

// Tests taken from GMPs own test suite.

$import = [
    ['0',0,1,GMP_ORDER_MSW_FIRST | GMP_ENDIAN_BIG,''],
    ['0',1,0,GMP_ORDER_MSW_FIRST | GMP_ENDIAN_BIG,''],
    ['12345678',4,1,GMP_ORDER_MSW_FIRST | GMP_ENDIAN_BIG,'12345678'],
    ['12345678',1,4,GMP_ORDER_MSW_FIRST | GMP_ENDIAN_BIG,'12345678'],
    ['12345678',1,4,GMP_ORDER_LSW_FIRST | GMP_ENDIAN_BIG,'12345678'],
    ['12345678',4,1,GMP_ORDER_LSW_FIRST | GMP_ENDIAN_LITTLE,'78563412'],
    ['12345678',1,4,0,'78563412'], // options are defaults
    ['12345678',1,4,GMP_ORDER_LSW_FIRST | GMP_ENDIAN_LITTLE,'78563412'],
    ['123456789abc',3,2,GMP_ORDER_MSW_FIRST | GMP_ENDIAN_BIG,'123456789abc'],
    ['123456789abc',3,2,GMP_ORDER_LSW_FIRST | GMP_ENDIAN_BIG,'9abc56781234'],
    ['123456789abc',3,2,0,'34127856bc9a'], // options are defaults
    ['123456789abc',3,2,GMP_ORDER_LSW_FIRST | GMP_ENDIAN_LITTLE,'bc9a78563412'],
    ['112233445566778899aabbcc',3,4,GMP_ORDER_MSW_FIRST | GMP_ENDIAN_BIG,'112233445566778899aabbcc'],
    ['112233445566778899aabbcc',3,4,GMP_ORDER_LSW_FIRST | GMP_ENDIAN_BIG,'99aabbcc5566778811223344'],
    ['112233445566778899aabbcc',3,4,0,'4433221188776655ccbbaa99'], // options are defaults
    ['112233445566778899aabbcc',3,4,GMP_ORDER_LSW_FIRST | GMP_ENDIAN_LITTLE,'ccbbaa998877665544332211'],
    ['100120023003400450056006700780089009a00ab00bc00c',3,8,GMP_ORDER_MSW_FIRST | GMP_ENDIAN_BIG,'100120023003400450056006700780089009a00ab00bc00c'],
    ['100120023003400450056006700780089009a00ab00bc00c',3,8,GMP_ORDER_LSW_FIRST | GMP_ENDIAN_BIG,'9009a00ab00bc00c50056006700780081001200230034004'],
    ['100120023003400450056006700780089009a00ab00bc00c',3,8,0,'044003300220011008800770066005500cc00bb00aa00990'], // options are defaults
    ['100120023003400450056006700780089009a00ab00bc00c',3,8,GMP_ORDER_LSW_FIRST | GMP_ENDIAN_LITTLE,'0cc00bb00aa0099008800770066005500440033002200110']
];

$export = [
    ['0',0,1,GMP_ORDER_MSW_FIRST | GMP_ENDIAN_BIG,''],
    ['0',0,2,GMP_ORDER_MSW_FIRST | GMP_ENDIAN_BIG,''],
    ['0',0,3,GMP_ORDER_MSW_FIRST | GMP_ENDIAN_BIG,''],
    ['12345678',4,1,GMP_ORDER_MSW_FIRST | GMP_ENDIAN_BIG,'12345678'],
    ['12345678',1,4,GMP_ORDER_MSW_FIRST | GMP_ENDIAN_BIG,'12345678'],
    ['12345678',1,4,GMP_ORDER_LSW_FIRST | GMP_ENDIAN_BIG,'12345678'],
    ['12345678',4,1,GMP_ORDER_LSW_FIRST | GMP_ENDIAN_LITTLE,'78563412'],
    ['12345678',1,4,0,'78563412'], // options are defaults
    ['12345678',1,4,GMP_ORDER_LSW_FIRST | GMP_ENDIAN_LITTLE,'78563412'],
    ['123456789ABC',3,2,GMP_ORDER_MSW_FIRST | GMP_ENDIAN_BIG,'123456789abc'],
    ['123456789ABC',3,2,GMP_ORDER_LSW_FIRST | GMP_ENDIAN_BIG,'9abc56781234'],
    ['123456789ABC',3,2,0,'34127856bc9a'], // options are defaults
    ['123456789ABC',3,2,GMP_ORDER_LSW_FIRST | GMP_ENDIAN_LITTLE,'bc9a78563412'],
    ['112233445566778899AABBCC',3,4,GMP_ORDER_MSW_FIRST | GMP_ENDIAN_BIG,'112233445566778899aabbcc'],
    ['112233445566778899AABBCC',3,4,GMP_ORDER_LSW_FIRST | GMP_ENDIAN_BIG,'99aabbcc5566778811223344'],
    ['112233445566778899AABBCC',3,4,0,'4433221188776655ccbbaa99'], // options are defaults
    ['112233445566778899AABBCC',3,4,GMP_ORDER_LSW_FIRST | GMP_ENDIAN_LITTLE,'ccbbaa998877665544332211'],
    ['100120023003400450056006700780089009A00AB00BC00C',3,8,GMP_ORDER_MSW_FIRST | GMP_ENDIAN_BIG,'100120023003400450056006700780089009a00ab00bc00c'],
    ['100120023003400450056006700780089009A00AB00BC00C',3,8,GMP_ORDER_LSW_FIRST | GMP_ENDIAN_BIG,'9009a00ab00bc00c50056006700780081001200230034004'],
    ['100120023003400450056006700780089009A00AB00BC00C',3,8,0,'044003300220011008800770066005500cc00bb00aa00990'], // options are defaults
    ['100120023003400450056006700780089009A00AB00BC00C',3,8,GMP_ORDER_LSW_FIRST | GMP_ENDIAN_LITTLE,'0cc00bb00aa0099008800770066005500440033002200110']
];

print "Import:\n";
$passed = true;
foreach ($import as $k => $test) {
    // count * size
    $data = substr(hex2bin($test[4]), 0, $test[1] * $test[2]);
    // data, size, options
    $gmp = gmp_import($data, $test[2], $test[3]);
    print "$k\n";
    if (!is_null($gmp)) {
        $result = gmp_strval($gmp, 16);
        if ($result !== $test[0]) {
            print "$k: '$result' !== '{$test[0]}'\n";
            $passed = false;
        }
    }
}

var_dump($passed);

print "Export:\n";
$passed = true;
foreach ($export as $k => $test) {
    $gmp = gmp_init($test[0], 16);
    // gmpumber, size, options
    $str = gmp_export($gmp, $test[2], $test[3]);
    // count * size
    print "$k\n";
    if (is_string($str)) {
        $result = bin2hex(substr($str, 0, $test[1] * $test[2]));
        if ($result !== $test[4]) {
            print "$k: '$result' !== '{$test[4]}'\n";
            $passed = false;
        }
    }
}

var_dump($passed);

print "bad sizes\n";
var_dump(gmp_import('a', -1));
var_dump(gmp_import('a', 0));

print "good sizes\n";
gmp_import('a', 1);

print "bad sizes for data length\n";
var_dump(gmp_import('a', 2));
var_dump(gmp_import('aa', 3));
var_dump(gmp_import(str_repeat('a', 100), 64));
var_dump(gmp_import('', 1));

--EXPECTF--
Import:

Warning: gmp_import(): Not enough input, need 1, have 0 in %s on line %d
0

Warning: gmp_import(): Bad word size: 0 (should be at least 1 byte) in %s on line %d
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
bool(true)
Export:
0
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
bool(true)
bad sizes

Warning: gmp_import(): Bad word size: -1 (should be at least 1 byte) in %s on line %d
NULL

Warning: gmp_import(): Bad word size: 0 (should be at least 1 byte) in %s on line %d
NULL
good sizes
bad sizes for data length

Warning: gmp_import(): Not enough input, need 2, have 1 in %s on line %d
NULL

Warning: gmp_import(): Not enough input, need 3, have 2 in %s on line %d
NULL

Warning: gmp_import(): Not enough input, need 64, have 36 in %s on line %d
NULL

Warning: gmp_import(): Not enough input, need 1, have 0 in %s on line %d
NULL
