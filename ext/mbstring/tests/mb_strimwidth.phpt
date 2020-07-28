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
  $result = mb_strimwidth($string, $start, $width, '...', $encoding);
  print "Test $index: start=$start width=$width result=[";
  print MBStringChars($result, $encoding);
  print "] length=" . mb_strlen($result, $encoding);
  print " width=" . mb_strwidth($result, $encoding) . "\n";
}

// EUC-JP
$euc_jp = pack('H*', '30313233a4b3a4cecab8bbfacef3a4cfc6fccbdcb8eca4c7a4b9a1a34555432d4a50a4f2bbc8a4c3a4c6a4a4a4dea4b9a1a3c6fccbdcb8eca4cfccccc5ddbdada4a4a1a3');

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

?>
--EXPECT--
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