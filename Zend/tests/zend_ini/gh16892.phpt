--TEST--
GH-16892 (ini_parse_quantity() fails to parse inputs starting with 0x0b)
--FILE--
<?php
echo ini_parse_quantity('0x0b'), "\n";
echo ini_parse_quantity('0xb'), "\n";
echo ini_parse_quantity('-0x0B'), "\n";
echo ini_parse_quantity('-0xB'), "\n";
echo ini_parse_quantity('0x0beef'), "\n";
echo ini_parse_quantity('0xbeef'), "\n";
echo ini_parse_quantity('-0x0BEEF'), "\n";
echo ini_parse_quantity('-0xBEEF'), "\n";
?>
--EXPECT--
11
11
-11
-11
48879
48879
-48879
-48879
