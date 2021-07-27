--TEST--
Test verification and conversion of UCS-4 text
--EXTENSIONS--
mbstring
--FILE--
<?php
include('encoding_tests.inc');

// Test "long" illegal character markers
mb_substitute_character("long");
convertInvalidString("\x6F\x00\x00\x00", "U+6F000000", "UCS-4BE", "UTF-8");
convertInvalidString("\x70\x00\x00\x00", "?+0", "UCS-4BE", "UTF-8");
convertInvalidString("\x78\x00\x00\x01", "BAD+1", "UCS-4BE", "UTF-8");
convertInvalidString("\x80\x01\x02\x03", "BAD+10203", "UCS-4BE", "UTF-8");
convertInvalidString("\x00\x01\x02", "BAD+10200", "UCS-4BE", "UTF-8");

convertInvalidString("\x00\x00\x00\x6F", "U+6F000000", "UCS-4LE", "UTF-8");
convertInvalidString("\x00\x00\x00\x70", "?+0", "UCS-4LE", "UTF-8");
convertInvalidString("\x01\x00\x00\x78", "BAD+1", "UCS-4LE", "UTF-8");
convertInvalidString("\x02\x01\x00", "BAD+102", "UCS-4LE", "UTF-8");

echo "Done!";
?>
--EXPECT--
Done!
