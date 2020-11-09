--TEST--
mb_strimwidth()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
output_handler=
--FILE--
<?php
// TODO: Add more encoding

function MBStringChars($string, $encoding) {
  $chars = mb_str_split($string, 1, $encoding);
  return implode(' ', array_map(function($char) {
    return join(unpack('H*', $char));
  }, $chars));
}

function MBStringWidths($string, $encoding) {
  $chars = mb_str_split($string, 1, $encoding);
  return implode(' ', array_map(function($char) use(&$encoding) {
    return mb_strwidth($char, $encoding);
  }, $chars));
}

function testStrimwidth($index, $string, $start, $width, $encoding) {
  $trimmark = mb_convert_encoding('...', $encoding, 'ASCII');
  $result = mb_strimwidth($string, $start, $width, $trimmark, $encoding);
  print "Test $index: start=$start width=$width result=[";
  print MBStringChars($result, $encoding);
  print "] length=" . mb_strlen($result, $encoding);
  print " width=" . mb_strwidth($result, $encoding) . "\n";
}

// EUC-JP
$euc_jp = pack('H*', '30313233a4b3a4cecab8bbfacef3a4cfc6fccbdcb8eca4c7a4b9a1a34555432d4a50a4f2bbc8a4c3a4c6a4a4a4dea4b9a1a3c6fccbdcb8eca4cfccccc5ddbdada4a4a1a3');

print "** EUC-JP **\n";
print "String width: ".mb_strwidth($euc_jp,'EUC-JP')."\n";
print "String character length: ".mb_strlen($euc_jp,'EUC-JP')."\n";

echo 'Characters: [' . MBStringChars($euc_jp, 'EUC-JP') . "]\n";
echo 'Character widths: [' . MBStringWidths($euc_jp, 'EUC-JP') . "]\n\n";

// Cut down to a width of 15, which is 10 characters in this case.
// Trim marker will be added
testStrimwidth(1, $euc_jp, 0, 15, 'EUC-JP');

// With max width of 100, trim marker will not be added
testStrimwidth(2, $euc_jp, 0, 100, 'EUC-JP');

// Skip 15 characters into string
testStrimwidth(3, $euc_jp, 15, 100, 'EUC-JP');

// Count 30 characters back from end of string, then limit to width of 5
// Since width of trim marker is 3, this will only get a single char from string
testStrimwidth(4, $euc_jp, -30, 5, 'EUC-JP');

// Count 9 characters from start of string. Since string is 39 characters
// long, this will have the same result as test #4
testStrimwidth(5, $euc_jp, 9, 5, 'EUC-JP');

// Skip 15 characters, which leaves a total width of 42. Then trim string down
// to 5 less than that, which is a width of 37.
testStrimwidth(6, $euc_jp, 15, -5, 'EUC-JP');

// Take the last 30 characters, which have a width of 54. Trim string down to
// 25 less than that, which is 29.
testStrimwidth(7, $euc_jp, -30, -25, 'EUC-JP');

// Skip over 100 characters... but since string is only 39 characters long,
// it takes us to the end of the string, and output is empty
testStrimwidth(8, $euc_jp, 100, 10, 'EUC-JP');

// Take the last 10 characters, which have a width of 20. Trim string down to
// 12 less than that, which is a width of 8.
testStrimwidth(9, $euc_jp, -10, -12, 'EUC-JP');

try {
    var_dump(mb_strimwidth($euc_jp, 0, -100,'...','EUC-JP'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(mb_strimwidth($euc_jp, -100, 10,'...','EUC-JP'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "\n";

// UTF-16
// In UTF-8, this is: 123abc漢字かな
$utf16le = pack('H*', '310032003300610062006300226f575b4b306a30');

echo "** UTF-16LE **\n";
print "String width: " . mb_strwidth($utf16le, 'UTF-16LE') . "\n";
print "String character length: " . mb_strlen($utf16le, 'UTF-16LE') . "\n";

echo 'Characters: [' . MBStringChars($utf16le, 'UTF-16LE') . "]\n";
echo 'Character widths: [' . MBStringWidths($utf16le, 'UTF-16LE') . "]\n\n";

// Just take the whole string with plenty of room to spare
testStrimwidth(10, $utf16le, 0, 100, 'UTF-16LE');

// OK, now the string will just barely fit in the allowed width...
testStrimwidth(11, $utf16le, 0, 14, 'UTF-16LE');

// Now the last hiragana won't quite fit
testStrimwidth(12, $utf16le, 0, 13, 'UTF-16LE');

// Even a bit tighter
testStrimwidth(13, $utf16le, 0, 12, 'UTF-16LE');

// What if we expect an output width of zero?
// (It will still output the trim marker, pushing the width beyond the stipulated 'maximum')
testStrimwidth(14, $utf16le, 0, 0, 'UTF-16LE');

// Or output width of one?
// (Likewise, it will still output the trim marker)
testStrimwidth(15, $utf16le, 0, 1, 'UTF-16LE');

// OK, let's count back 3 characters from the end of the string, then allow a width of 5
// (Negative starting index)
testStrimwidth(16, $utf16le, -3, 5, 'UTF-16LE');

// Let's also try a negative width
// We'll count back 4 characters, then allow a width of ((4 * 2) - 2) = 6
// Since the output will not reach the END of the string, the trim marker
// will have to be added, and will consume a width of 3
testStrimwidth(17, $utf16le, -4, -2, 'UTF-16LE');

?>
--EXPECT--
** EUC-JP **
String width: 68
String character length: 39
Characters: [30 31 32 33 a4b3 a4ce cab8 bbfa cef3 a4cf c6fc cbdc b8ec a4c7 a4b9 a1a3 45 55 43 2d 4a 50 a4f2 bbc8 a4c3 a4c6 a4a4 a4de a4b9 a1a3 c6fc cbdc b8ec a4cf cccc c5dd bdad a4a4 a1a3]
Character widths: [1 1 1 1 2 2 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2]

Test 1: start=0 width=15 result=[30 31 32 33 a4b3 a4ce cab8 bbfa 2e 2e 2e] length=11 width=15
Test 2: start=0 width=100 result=[30 31 32 33 a4b3 a4ce cab8 bbfa cef3 a4cf c6fc cbdc b8ec a4c7 a4b9 a1a3 45 55 43 2d 4a 50 a4f2 bbc8 a4c3 a4c6 a4a4 a4de a4b9 a1a3 c6fc cbdc b8ec a4cf cccc c5dd bdad a4a4 a1a3] length=39 width=68
Test 3: start=15 width=100 result=[a1a3 45 55 43 2d 4a 50 a4f2 bbc8 a4c3 a4c6 a4a4 a4de a4b9 a1a3 c6fc cbdc b8ec a4cf cccc c5dd bdad a4a4 a1a3] length=24 width=42
Test 4: start=-30 width=5 result=[a4cf 2e 2e 2e] length=4 width=5
Test 5: start=9 width=5 result=[a4cf 2e 2e 2e] length=4 width=5
Test 6: start=15 width=-5 result=[a1a3 45 55 43 2d 4a 50 a4f2 bbc8 a4c3 a4c6 a4a4 a4de a4b9 a1a3 c6fc cbdc b8ec a4cf cccc 2e 2e 2e] length=23 width=37
Test 7: start=-30 width=-25 result=[a4cf c6fc cbdc b8ec a4c7 a4b9 a1a3 45 55 43 2d 4a 50 a4f2 bbc8 a4c3 2e 2e 2e] length=19 width=29
Test 8: start=100 width=10 result=[] length=0 width=0
Test 9: start=-10 width=-12 result=[a1a3 c6fc 2e 2e 2e] length=5 width=7
mb_strimwidth(): Argument #3 ($width) is out of range
mb_strimwidth(): Argument #2 ($start) is out of range

** UTF-16LE **
String width: 14
String character length: 10
Characters: [3100 3200 3300 6100 6200 6300 226f 575b 4b30 6a30]
Character widths: [1 1 1 1 1 1 2 2 2 2]

Test 10: start=0 width=100 result=[3100 3200 3300 6100 6200 6300 226f 575b 4b30 6a30] length=10 width=14
Test 11: start=0 width=14 result=[3100 3200 3300 6100 6200 6300 226f 575b 4b30 6a30] length=10 width=14
Test 12: start=0 width=13 result=[3100 3200 3300 6100 6200 6300 226f 575b 2e00 2e00 2e00] length=11 width=13
Test 13: start=0 width=12 result=[3100 3200 3300 6100 6200 6300 226f 2e00 2e00 2e00] length=10 width=11
Test 14: start=0 width=0 result=[2e00 2e00 2e00] length=3 width=3
Test 15: start=0 width=1 result=[2e00 2e00 2e00] length=3 width=3
Test 16: start=-3 width=5 result=[575b 2e00 2e00 2e00] length=4 width=5
Test 17: start=-4 width=-2 result=[226f 2e00 2e00 2e00] length=4 width=5
