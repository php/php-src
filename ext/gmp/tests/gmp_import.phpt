--TEST--
gmp_import() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) echo "skip"; ?>
--FILE--
<?php

// Tests taken from GMPs own test suite.

// format is [expected, size, options, input]
$import = [
    ['0',1,GMP_BIG_ENDIAN,''],
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

$passed = true;
foreach ($import as $k => $test) {
    $gmp = gmp_import(hex2bin($test[3]), $test[1], $test[2]);
    if ($gmp instanceof GMP) {
        $result = gmp_strval($gmp, 16);
        if ($result !== $test[0]) {
            echo "$k: '$result' !== '{$test[0]}'\n";
            $passed = false;
        }
    } else {
        $type = gettype($gmp);
        echo "$k: $type !== '{$test[0]}'\n";
    }
}

var_dump($passed);

// Invalid word sizes
try {
    var_dump(gmp_import('a', -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_import('a', 0));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

// Invalid data lengths
try {
    var_dump(gmp_import('a', 2));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_import('aa', 3));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_import(str_repeat('a', 100), 64));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

// Invalid options
try {
    var_dump(gmp_import('a', 1, GMP_MSW_FIRST | GMP_LSW_FIRST));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_import('a', 1, GMP_BIG_ENDIAN | GMP_LITTLE_ENDIAN));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
bool(true)
gmp_import(): Argument #2 ($word_size) must be greater than or equal to 1
gmp_import(): Argument #2 ($word_size) must be greater than or equal to 1
gmp_import(): Argument #1 ($data) must be a multiple of argument #2 ($word_size)
gmp_import(): Argument #1 ($data) must be a multiple of argument #2 ($word_size)
gmp_import(): Argument #1 ($data) must be a multiple of argument #2 ($word_size)
gmp_import(): Argument #3 ($flags) cannot use multiple word order options
gmp_import(): Argument #3 ($flags) cannot use multiple endian options
