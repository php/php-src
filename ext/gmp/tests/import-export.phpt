--TEST--
Check gmp_import and gmp_export behave as intended
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

// Tests taken from GMPs own test suite.

$import = [
    ['0',0,1,1,1,0,''],
    ['0',1,1,0,1,0,''],
    ['12345678',4,1,1,1,0,'12345678'],
    ['12345678',1,1,4,1,0,'12345678'],
    ['12345678',1,-1,4,1,0,'12345678'],
    ['12345678',4,-1,1,-1,0,'78563412'],
    ['12345678',1,1,4,-1,0,'78563412'],
    ['12345678',1,-1,4,-1,0,'78563412'],
    ['123456789abc',3,1,2,1,0,'123456789abc'],
    ['123456789abc',3,-1,2,1,0,'9abc56781234'],
    ['123456789abc',3,1,2,-1,0,'34127856bc9a'],
    ['123456789abc',3,-1,2,-1,0,'bc9a78563412'],
    ['112233445566778899aabbcc',3,1,4,1,0,'112233445566778899aabbcc'],
    ['112233445566778899aabbcc',3,-1,4,1,0,'99aabbcc5566778811223344'],
    ['112233445566778899aabbcc',3,1,4,-1,0,'4433221188776655ccbbaa99'],
    ['112233445566778899aabbcc',3,-1,4,-1,0,'ccbbaa998877665544332211'],
    ['100120023003400450056006700780089009a00ab00bc00c',3,1,8,1,0,'100120023003400450056006700780089009a00ab00bc00c'],
    ['100120023003400450056006700780089009a00ab00bc00c',3,-1,8,1,0,'9009a00ab00bc00c50056006700780081001200230034004'],
    ['100120023003400450056006700780089009a00ab00bc00c',3,1,8,-1,0,'044003300220011008800770066005500cc00bb00aa00990'],
    ['100120023003400450056006700780089009a00ab00bc00c',3,-1,8,-1,0,'0cc00bb00aa0099008800770066005500440033002200110']
];

$export = [
    ['0',0,1,1,1,0,''],
    ['0',0,1,2,1,0,''],
    ['0',0,1,3,1,0,''],
    ['12345678',4,1,1,1,0,'12345678'],
    ['12345678',1,1,4,1,0,'12345678'],
    ['12345678',1,-1,4,1,0,'12345678'],
    ['12345678',4,-1,1,-1,0,'78563412'],
    ['12345678',1,1,4,-1,0,'78563412'],
    ['12345678',1,-1,4,-1,0,'78563412'],
    ['123456789ABC',3,1,2,1,0,'123456789abc'],
    ['123456789ABC',3,-1,2,1,0,'9abc56781234'],
    ['123456789ABC',3,1,2,-1,0,'34127856bc9a'],
    ['123456789ABC',3,-1,2,-1,0,'bc9a78563412'],
    ['112233445566778899AABBCC',3,1,4,1,0,'112233445566778899aabbcc'],
    ['112233445566778899AABBCC',3,-1,4,1,0,'99aabbcc5566778811223344'],
    ['112233445566778899AABBCC',3,1,4,-1,0,'4433221188776655ccbbaa99'],
    ['112233445566778899AABBCC',3,-1,4,-1,0,'ccbbaa998877665544332211'],
    ['100120023003400450056006700780089009A00AB00BC00C',3,1,8,1,0,'100120023003400450056006700780089009a00ab00bc00c'],
    ['100120023003400450056006700780089009A00AB00BC00C',3,-1,8,1,0,'9009a00ab00bc00c50056006700780081001200230034004'],
    ['100120023003400450056006700780089009A00AB00BC00C',3,1,8,-1,0,'044003300220011008800770066005500cc00bb00aa00990'],
    ['100120023003400450056006700780089009A00AB00BC00C',3,-1,8,-1,0,'0cc00bb00aa0099008800770066005500440033002200110']
];

print "Import:\n";
$passed = true;
foreach ($import as $k => $test) {
    // count * size
    $data = substr(hex2bin($test[6]), 0, $test[1] * $test[3]);
    // data, order, size, endian
    $gmp = gmp_import($data, $test[2], $test[3], $test[4]);
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
    // gmpumber, order, size, endian
    $str = gmp_export($gmp, $test[2], $test[3], $test[4]);
    // count * size
    print "$k\n";
    if (is_string($str)) {
        $result = bin2hex(substr($str, 0, $test[1] * $test[3]));
        if ($result !== $test[6]) {
            print "$k: '$result' !== '{$test[6]}'\n";
            $passed = false;
        }
    }
}

var_dump($passed);

print "bad orders\n";
var_dump(gmp_import('a', 2, 1, 1));
var_dump(gmp_import('a', 0, 1, 1));
var_dump(gmp_import('a', -2, 1, 1));

print "good orders\n";
gmp_import('a', -1, 1, 1);
gmp_import('a', 1, 1, 1);

print "bad sizes\n";
var_dump(gmp_import('a', -1, -1, 1));
var_dump(gmp_import('a', -1, 0, 1));

print "good sizes\n";
gmp_import('a', -1, 1, 1);

print "bad endians\n";
var_dump(gmp_import('a', -1, 1, 2));
var_dump(gmp_import('a', -1, 1, -2));

print "good endians\n";
gmp_import('a', -1, 1, 1);
gmp_import('a', -1, 1, 0);
gmp_import('a', -1, 1, -1);

print "bad sizes for data length\n";
var_dump(gmp_import('a', -1, 2, 1));
var_dump(gmp_import('aa', -1, 3, 1));
var_dump(gmp_import(str_repeat('a', 100), -1, 64, 1));
var_dump(gmp_import('', -1, 1, 1));

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
bad orders

Warning: gmp_import(): Bad order: 2 (should be 1 for most significant word first, or -1 for least significant first) in %s on line %d
NULL

Warning: gmp_import(): Bad order: 0 (should be 1 for most significant word first, or -1 for least significant first) in %s on line %d
NULL

Warning: gmp_import(): Bad order: -2 (should be 1 for most significant word first, or -1 for least significant first) in %s on line %d
NULL
good orders
bad sizes

Warning: gmp_import(): Bad word size: -1 (should be at least 1 byte) in %s on line %d
NULL

Warning: gmp_import(): Bad word size: 0 (should be at least 1 byte) in %s on line %d
NULL
good sizes
bad endians

Warning: gmp_import(): Bad endian: 2 (should be 1 for most significant byte first, -1 for least significant first or 0 for native endianness) in %s on line %d
NULL

Warning: gmp_import(): Bad endian: -2 (should be 1 for most significant byte first, -1 for least significant first or 0 for native endianness) in %s on line %d
NULL
good endians
bad sizes for data length

Warning: gmp_import(): Not enough input, need 2, have 1 in %s on line %d
NULL

Warning: gmp_import(): Not enough input, need 3, have 2 in %s on line %d
NULL

Warning: gmp_import(): Not enough input, need 64, have 36 in %s on line %d
NULL

Warning: gmp_import(): Not enough input, need 1, have 0 in %s on line %d
NULL
