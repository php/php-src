--TEST--
Test verification and conversion of UCS-2 text
--EXTENSIONS--
mbstring
--FILE--
<?php
include('encoding_tests.inc');
mb_substitute_character(0x25);

// Test "long" illegal character markers
mb_substitute_character("long");

convertInvalidString("\x00\x01\x02\x03", "\x00U\x00+\x001\x000\x002\x000\x003", "UTF-32BE", "UCS-2BE");
convertInvalidString("\x11", "%", "UCS-2BE", "UTF-8");

convertInvalidString("\x00\x01\x02\x03", "U\x00+\x001\x000\x002\x000\x003\x00", "UTF-32BE", "UCS-2LE");
convertInvalidString("\x11", "%", "UCS-2LE", "UTF-8");

echo "Done!";
?>
--EXPECT--
Done!
