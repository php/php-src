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
echo base_convert('fg', 16, 10);
echo base_convert('f 0xff ', 16, 10);
echo base_convert('1xff ', 16, 10);
echo base_convert(chr(0), 16, 10);
echo base_convert("0o7" , 9, 10) . "\n";

?>
--EXPECTF--
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
Deprecated: Invalid characters passed for attempted conversion, these have been ignored in /tmp/test.php on line 18
15
Deprecated: Invalid characters passed for attempted conversion, these have been ignored in /tmp/test.php on line 19
61695
Deprecated: Invalid characters passed for attempted conversion, these have been ignored in /tmp/test.php on line 20
511
Deprecated: Invalid characters passed for attempted conversion, these have been ignored in /tmp/test.php on line 21
0
Deprecated: Invalid characters passed for attempted conversion, these have been ignored in /tmp/test.php on line 22
7
