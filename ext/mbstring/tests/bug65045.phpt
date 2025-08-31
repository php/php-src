--TEST--
Bug #65045: mb_convert_encoding breaks well-formed character
--EXTENSIONS--
mbstring
--FILE--
<?php

mb_internal_encoding('UTF-8');

$str = "\xF0\xA4\xAD".  "\xF0\xA4\xAD\xA2"."\xF0\xA4\xAD\xA2";
$str2 = "\xF0\xA4\xAD\xA2"."\xF0\xA4\xAD\xA2"."\xF0\xA4\xAD";

mb_substitute_character(0xFFFD);

echo bin2hex(htmlspecialchars_decode(htmlspecialchars($str, ENT_SUBSTITUTE, 'UTF-8'))), "\n";
echo bin2hex(htmlspecialchars_decode(htmlspecialchars($str2, ENT_SUBSTITUTE, 'UTF-8'))), "\n";
echo bin2hex(mb_convert_encoding($str, 'UTF-8', 'UTF-8')), "\n";
echo bin2hex(mb_convert_encoding($str2, 'UTF-8', 'UTF-8')), "\n";

?>
--EXPECT--
efbfbdf0a4ada2f0a4ada2
f0a4ada2f0a4ada2efbfbd
efbfbdf0a4ada2f0a4ada2
f0a4ada2f0a4ada2efbfbd
