--TEST--
Test bindec() - basic function test bindec()
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--INI--
precision=14
--FILE--
<?php
$values = array(111000111,
                011100000,
                1111111111111111111111111111111,
                10000000000000000000000000000000,
                100002001,
                '111000111',
                '011100000',
                '1111111111111111111111111111111',
                '10000000000000000000000000000000',
                '100002001',
                'abcdefg',
                311015,
                31101.3,
                31.1013e5,
                0x111ABC,
                011237,
                true,
                false,
                );

for ($i = 0; $i < count($values); $i++) {
    try {
        $res = bindec($values[$i]);
        var_dump($res);
    } catch (ValueError $e) {
        echo 'ValueError: ', $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
int(455)
ValueError: bindec(): Argument #1 ($binary_string) has invalid characters for attempted conversion
ValueError: bindec(): Argument #1 ($binary_string) has invalid characters for attempted conversion
ValueError: bindec(): Argument #1 ($binary_string) has invalid characters for attempted conversion
ValueError: bindec(): Argument #1 ($binary_string) has invalid characters for attempted conversion
int(455)
int(224)
int(2147483647)
int(2147483648)
ValueError: bindec(): Argument #1 ($binary_string) has invalid characters for attempted conversion
ValueError: bindec(): Argument #1 ($binary_string) has invalid characters for attempted conversion
ValueError: bindec(): Argument #1 ($binary_string) has invalid characters for attempted conversion
ValueError: bindec(): Argument #1 ($binary_string) has invalid characters for attempted conversion
ValueError: bindec(): Argument #1 ($binary_string) has invalid characters for attempted conversion
ValueError: bindec(): Argument #1 ($binary_string) has invalid characters for attempted conversion
ValueError: bindec(): Argument #1 ($binary_string) has invalid characters for attempted conversion
int(1)
int(0)
