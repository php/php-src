--TEST--
hex2bin() function
--FILE--
<?php
$hex = bin2hex(hex2bin('12345'));
echo "$hex\n";
?>
--EXPECT--
012345
