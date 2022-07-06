--TEST--
gmp_export() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) echo "skip"; ?>
--FILE--
<?php

// Tests taken from GMPs own test suite.

// format is [output, size, options, expected]
$export = [
    ['0',1,GMP_BIG_ENDIAN,''],
    ['0',2,GMP_BIG_ENDIAN,''],
    ['0',3,GMP_BIG_ENDIAN,''],
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

$passed = true;
foreach ($export as $k => $test) {
    $gmp = gmp_init($test[0], 16);
    $str = gmp_export($gmp, $test[1], $test[2]);
    if (is_string($str)) {
        $result = bin2hex($str);
        if ($result !== $test[3]) {
            echo "$k: '$result' !== '{$test[3]}'\n";
            $passed = false;
        }
    } else {
        $type = gettype($str);
        echo "$k: $type !== '{$test[3]}'\n";
    }
}

var_dump($passed);

// Argument converted from int to GMP
var_dump(bin2hex(gmp_export(0xff)));

// Invalid word sizes
try {
    var_dump(gmp_export(123, -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_export(123, 0));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

// Invalid options
try {
    var_dump(gmp_export(123, 1, GMP_MSW_FIRST | GMP_LSW_FIRST));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_export(123, 1, GMP_BIG_ENDIAN | GMP_LITTLE_ENDIAN));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
bool(true)
string(2) "ff"
gmp_export(): Argument #2 ($word_size) must be greater than or equal to 1
gmp_export(): Argument #2 ($word_size) must be greater than or equal to 1
gmp_export(): Argument #3 ($flags) cannot use multiple word order options
gmp_export(): Argument #3 ($flags) cannot use multiple endian options
