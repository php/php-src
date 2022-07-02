--TEST--
Temporary test of mbstring's UUEncode 'encoding'
--EXTENSIONS--
mbstring
--FILE--
<?php

/* Using mbstring to convert strings from UUEncode has already been deprecated
 * So this test should be removed when the UUEncode 'encoding' is */

function testConversion($uuencode, $raw) {
  $converted = mb_convert_encoding($uuencode, '8bit', 'UUENCODE');
  if ($converted !== $raw)
    die('Expected "' . $uuencode . '" to convert to ' . bin2hex($raw) . '; actually got ' . bin2hex($converted));
  $converted = mb_convert_encoding($raw, 'UUENCODE', '8bit');
  if ($converted !== $uuencode)
    die('Expected ' . bin2hex($raw) . ' to convert to "' . $uuencode . '"; actually got "' . $converted . '"');
}

testConversion('', '');

/* mbstring's uuencode requires the user to strip off the terminating "`\nend\n" */

testConversion("begin 0644 filename\n#0V%T\n", 'Cat');
testConversion("begin 0644 filename\n::'1T<#HO+W=W=RYW:6MI<&5D:6\$N;W)G#0H`\n", "http://www.wikipedia.org\r\n");
testConversion("begin 0644 filename\n#`0(#\n", "\x01\x02\x03");
testConversion("begin 0644 filename\n$`0(#\"@``\n", "\x01\x02\x03\n");

echo "Done!\n";
?>
--EXPECTF--
Deprecated: mb_convert_encoding(): Handling Uuencode via mbstring is deprecated; use convert_uuencode/convert_uudecode instead in %s

Deprecated: mb_convert_encoding(): Handling Uuencode via mbstring is deprecated; use convert_uuencode/convert_uudecode instead in %s

Deprecated: mb_convert_encoding(): Handling Uuencode via mbstring is deprecated; use convert_uuencode/convert_uudecode instead in %s

Deprecated: mb_convert_encoding(): Handling Uuencode via mbstring is deprecated; use convert_uuencode/convert_uudecode instead in %s

Deprecated: mb_convert_encoding(): Handling Uuencode via mbstring is deprecated; use convert_uuencode/convert_uudecode instead in %s
Done!
