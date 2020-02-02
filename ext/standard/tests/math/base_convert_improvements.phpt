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

echo "=======================================";

// These should fail
echo base_convert_with_catch('fg', 16, 10);
echo base_convert_with_catch('f 0xff ', 16, 10);
echo base_convert_with_catch('1xff ', 16, 10);
echo base_convert_with_catch(chr(0), 16, 10);
echo base_convert_with_catch("0o7" , 9, 10);
echo base_convert_with_catch("0 0" , 9, 10) . "\n";

function base_convert_with_catch($original, $fromBase, $toBase) {
    try {
        return base_convert($original, $fromBase, $toBase);
    } catch (InvalidArgumentException $e) {
        return "Invalid\n";
    }
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
=======================================Invalid
Invalid
Invalid
Invalid
Invalid
Invalid
