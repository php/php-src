--TEST--
Check gmp_import and gmp_export behave as intended
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

// Tests taken from GMPs own test suite.

// format is [expected, size, options, input]
$import = [
    ['0',1,GMP_BIG_ENDIAN,''],
    ['0',0,GMP_BIG_ENDIAN,''],
    ['12345678',1,GMP_BIG_ENDIAN,'12345678'],
    ['12345678',4,GMP_BIG_ENDIAN,'12345678'],
    ['12345678',4,GMP_LSW_FIRST | GMP_BIG_ENDIAN,'12345678'],
    ['12345678',1,GMP_LSW_FIRST | GMP_LITTLE_ENDIAN,'78563412'],
    ['12345678',4,GMP_LITTLE_ENDIAN,'78563412'],
    ['12345678',4,GMP_LSW_FIRST | GMP_LITTLE_ENDIAN,'78563412'],
    ['123456789abc',2,GMP_BIG_ENDIAN,'123456789abc'],
    ['123456789abc',2,GMP_LSW_FIRST | GMP_BIG_ENDIAN,'9abc56781234'],
    ['123456789abc',2,GMP_LITTLE_ENDIAN,'34127856bc9a'],
    ['123456789abc',2,GMP_LSW_FIRST | GMP_LITTLE_ENDIAN,'bc9a78563412'],
    ['112233445566778899aabbcc',4,GMP_BIG_ENDIAN,'112233445566778899aabbcc'],
    ['112233445566778899aabbcc',4,GMP_LSW_FIRST | GMP_BIG_ENDIAN,'99aabbcc5566778811223344'],
    ['112233445566778899aabbcc',4,GMP_LITTLE_ENDIAN,'4433221188776655ccbbaa99'],
    ['112233445566778899aabbcc',4,GMP_LSW_FIRST | GMP_LITTLE_ENDIAN,'ccbbaa998877665544332211'],
    ['100120023003400450056006700780089009a00ab00bc00c',8,GMP_BIG_ENDIAN,'100120023003400450056006700780089009a00ab00bc00c'],
    ['100120023003400450056006700780089009a00ab00bc00c',8,GMP_LSW_FIRST | GMP_BIG_ENDIAN,'9009a00ab00bc00c50056006700780081001200230034004'],
    ['100120023003400450056006700780089009a00ab00bc00c',8,GMP_LITTLE_ENDIAN,'044003300220011008800770066005500cc00bb00aa00990'],
    ['100120023003400450056006700780089009a00ab00bc00c',8,GMP_LSW_FIRST | GMP_LITTLE_ENDIAN,'0cc00bb00aa0099008800770066005500440033002200110']
];

// format is [output, size, options, expected]
$export = [
    ['0',0,GMP_BIG_ENDIAN,''],
    ['0',1,GMP_BIG_ENDIAN,"00"],
    ['0',2,GMP_BIG_ENDIAN,"0000"],
    ['0',3,GMP_BIG_ENDIAN,"000000"],
    ['12345678',1,GMP_BIG_ENDIAN,'12345678'],
    ['12345678',4,GMP_BIG_ENDIAN,'12345678'],
    ['12345678',4,GMP_LSW_FIRST | GMP_BIG_ENDIAN,'12345678'],
    ['12345678',1,GMP_LSW_FIRST | GMP_LITTLE_ENDIAN,'78563412'],
    ['12345678',4,GMP_LITTLE_ENDIAN,'78563412'],
    ['12345678',4,GMP_LSW_FIRST | GMP_LITTLE_ENDIAN,'78563412'],
    ['123456789ABC',2,GMP_BIG_ENDIAN,'123456789abc'],
    ['123456789ABC',2,GMP_LSW_FIRST | GMP_BIG_ENDIAN,'9abc56781234'],
    ['123456789ABC',2,GMP_LITTLE_ENDIAN,'34127856bc9a'],
    ['123456789ABC',2,GMP_LSW_FIRST | GMP_LITTLE_ENDIAN,'bc9a78563412'],
    ['112233445566778899AABBCC',4,GMP_BIG_ENDIAN,'112233445566778899aabbcc'],
    ['112233445566778899AABBCC',4,GMP_LSW_FIRST | GMP_BIG_ENDIAN,'99aabbcc5566778811223344'],
    ['112233445566778899AABBCC',4,GMP_LITTLE_ENDIAN,'4433221188776655ccbbaa99'],
    ['112233445566778899AABBCC',4,GMP_LSW_FIRST | GMP_LITTLE_ENDIAN,'ccbbaa998877665544332211'],
    ['100120023003400450056006700780089009A00AB00BC00C',8,GMP_BIG_ENDIAN,'100120023003400450056006700780089009a00ab00bc00c'],
    ['100120023003400450056006700780089009A00AB00BC00C',8,GMP_LSW_FIRST | GMP_BIG_ENDIAN,'9009a00ab00bc00c50056006700780081001200230034004'],
    ['100120023003400450056006700780089009A00AB00BC00C',8,GMP_LITTLE_ENDIAN,'044003300220011008800770066005500cc00bb00aa00990'],
    ['100120023003400450056006700780089009A00AB00BC00C',8,GMP_LSW_FIRST | GMP_LITTLE_ENDIAN,'0cc00bb00aa0099008800770066005500440033002200110']
];

print "Import:\n";
$passed = true;
foreach ($import as $k => $test) {
    $gmp = gmp_import(hex2bin($test[3]), $test[1], $test[2]);
    print "$k\n";
    if (!is_null($gmp)) {
        $result = gmp_strval($gmp, 16);
        if ($result !== $test[0]) {
            print "$k: '$result' !== '{$test[0]}'\n";
            $passed = false;
        }
    }
    else {
        $type = gettype($gmp);
        print "$k: $type !== '{$test[0]}'\n";
    }
}

var_dump($passed);

print "Export:\n";
$passed = true;
foreach ($export as $k => $test) {
    $gmp = gmp_init($test[0], 16);
    $str = gmp_export($gmp, $test[1], $test[2]);
    print "$k\n";
    if (is_string($str)) {
        $result = bin2hex($str);
        if ($result !== $test[3]) {
            print "$k: '$result' !== '{$test[3]}'\n";
            $passed = false;
        }
    }
    else {
        $type = gettype($str);
        print "$k: $type !== '{$test[3]}'\n";
    }
}

var_dump($passed);

print "bad sizes\n";
var_dump(gmp_import('a', -1));

print "good sizes\n";
gmp_import('a', 1);

print "bad sizes for data length\n";
var_dump(gmp_import('a', 2));
var_dump(gmp_import('aa', 3));
var_dump(gmp_import(str_repeat('a', 100), 64));

print "error for bad options\n";
var_dump(gmp_import('a', 1, GMP_MSW_FIRST | GMP_LSW_FIRST));
var_dump(gmp_import('a', 1, GMP_BIG_ENDIAN | GMP_LITTLE_ENDIAN));

--EXPECTF--
Import:
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
21
bool(true)
bad sizes

Warning: gmp_import(): Bad word size: -1 (cannot be negative) in %s on line %d
NULL
good sizes
bad sizes for data length

Warning: gmp_import(): Input length must be a multiple of word size in %s on line %d
NULL

Warning: gmp_import(): Input length must be a multiple of word size in %s on line %d
NULL

Warning: gmp_import(): Input length must be a multiple of word size in %s on line %d
NULL
error for bad options

Warning: gmp_import(): Invalid options: Conflicting word orders in %s on line %d
NULL

Warning: gmp_import(): Invalid options: Conflicting word endianness in %s on line %d
NULL
