--TEST--
Temporary test of mbstring's UUEncode 'encoding'
--EXTENSIONS--
mbstring
--FILE--
<?php
srand(1000); // Make results consistent

/* Using mbstring to convert strings from UUEncode has already been deprecated
 * So this test should be removed when the UUEncode 'encoding' is */
error_reporting(E_ALL & ~E_DEPRECATED);

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

function testRoundTrip($data) {
  $encoded = mb_convert_encoding($data, 'UUENCODE', '8bit');
  $decoded = mb_convert_encoding($encoded, '8bit', 'UUENCODE');
  if ($data !== $decoded)
    die("Round-trip failed! Expected " . bin2hex($data) . " to round-trip; actually got " . bin2hex($decoded));
}

for ($iterations = 0; $iterations < 500; $iterations++) {
  $strlen = rand(1, 300);
  $characters = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
  $randstring = '';
  for ($i = 0; $i < $strlen; $i++) {
      $randstring .= $characters[rand(0, strlen($characters) - 1)];
  }
  testRoundTrip($randstring);
}

echo "Done!\n";
?>
--EXPECTF--
Done!
