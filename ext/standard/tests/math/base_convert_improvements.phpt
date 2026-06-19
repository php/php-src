--TEST--
Test base_convert() - see https://wiki.php.net/rfc/base_convert_improvements
--FILE--
<?php
echo base_convert('FF', 16, 10) . "\n";
echo base_convert('20', 8, 10) . "\n";
echo base_convert('0xFf', 16, 10) . "\n";
echo base_convert('0FF', 16, 10) . "\n";
echo base_convert(" 0xFF\t\n" , 16, 10) . "\n";
echo base_convert(" 0xFF\t\n" , 16, 10) . "\n";
echo base_convert("\r\nFF\t\n" , 16, 10) . "\n";
echo base_convert("0o7" , 8, 10) . "\n";
echo base_convert("0b1010" , 2, 10) . "\n";

echo "=======================================\n";

// These should fail
try {
    echo base_convert('fg', 16, 10);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    echo base_convert('f 0xff ', 16, 10);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    echo base_convert('1xff ', 16, 10);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    echo base_convert(chr(0), 16, 10);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    echo base_convert("0o7" , 9, 10);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    echo base_convert("0 0" , 9, 10) . "\n";
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
255
16
255
255
255
255
255
7
10
=======================================
Invalid characters passed for attempted conversion
Invalid characters passed for attempted conversion
Invalid characters passed for attempted conversion
Invalid characters passed for attempted conversion
Invalid characters passed for attempted conversion
Invalid characters passed for attempted conversion
