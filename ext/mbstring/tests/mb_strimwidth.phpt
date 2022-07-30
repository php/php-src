--TEST--
mb_strimwidth()
--EXTENSIONS--
mbstring
--INI--
output_handler=
--FILE--
<?php

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

function testStrimwidthWithMarker($string, $trimmark, $start, $width, $encoding) {
    $result = mb_strimwidth($string, $start, $width, $trimmark, $encoding);
    print "start=$start width=$width result=[";
    print MBStringChars($result, $encoding);
    print "] length=" . mb_strlen($result, $encoding);
    print " width=" . mb_strwidth($result, $encoding) . "\n";
}

function testStrimwidth($string, $start, $width, $encoding) {
  testStrimwidthWithMarker($string, mb_convert_encoding('...', $encoding, 'ASCII'), $start, $width, $encoding);
}

echo "== UTF-16LE ==\n";

// In UTF-8, this is: 123abc漢字かな
$utf16le = pack('H*', '310032003300610062006300226f575b4b306a30');

print "String length: " . mb_strlen($utf16le, 'UTF-16LE') . "\n";
print "String width: " . mb_strwidth($utf16le, 'UTF-16LE') . "\n";
echo 'Characters: [' . MBStringChars($utf16le, 'UTF-16LE') . "]\n";
echo 'Character widths: [' . MBStringWidths($utf16le, 'UTF-16LE') . "]\n\n";

// Just take the whole string with plenty of room to spare
testStrimwidth($utf16le, 0, 100, 'UTF-16LE');

// OK, now the string will just barely fit in the allowed width...
testStrimwidth($utf16le, 0, 14, 'UTF-16LE');

// Now the last hiragana won't quite fit
testStrimwidth($utf16le, 0, 13, 'UTF-16LE');

// Even a bit tighter
testStrimwidth($utf16le, 0, 12, 'UTF-16LE');

// What if we expect an output width of zero?
// (It will still output the trim marker, pushing the width beyond the stipulated 'maximum')
testStrimwidth($utf16le, 0, 0, 'UTF-16LE');

// Or output width of one?
// (Likewise, it will still output the trim marker)
testStrimwidth($utf16le, 0, 1, 'UTF-16LE');

// OK, let's count back 3 characters from the end of the string, then allow a width of 5
// (Negative starting index)
testStrimwidth($utf16le, -3, 5, 'UTF-16LE');

// Let's also try a negative width
// We'll count back 4 characters, then allow a width of ((4 * 2) - 2) = 6
// Since the output will not reach the END of the string, the trim marker
// will have to be added, and will consume a width of 3
testStrimwidth($utf16le, -4, -2, 'UTF-16LE');

echo "\n== EUC-JP ==\n";

// In UTF-8, this is: 0123この文字列は日本語です。EUC-JPを使っています。日本語は面倒臭い。
$euc_jp = "0123\xa4\xb3\xa4\xce\xca\xb8\xbb\xfa\xce\xf3\xa4\xcf\xc6\xfc\xcb\xdc\xb8\xec\xa4\xc7\xa4\xb9\xa1\xa3EUC-JP\xa4\xf2\xbb\xc8\xa4\xc3\xa4\xc6\xa4\xa4\xa4\xde\xa4\xb9\xa1\xa3\xc6\xfc\xcb\xdc\xb8\xec\xa4\xcf\xcc\xcc\xc5\xdd\xbd\xad\xa4\xa4\xa1\xa3";

print "String length: " . mb_strlen($euc_jp, 'EUC-JP') . "\n";
print "String width: " . mb_strwidth($euc_jp, 'EUC-JP') . "\n";
echo 'Characters: [' . MBStringChars($euc_jp, 'EUC-JP') . "]\n";
echo 'Character widths: [' . MBStringWidths($euc_jp, 'EUC-JP') . "]\n\n";

// Cut down to a width of 15, which is 10 characters in this case.
// Trim marker will be added
testStrimwidth($euc_jp, 0, 15, 'EUC-JP');

// With max width of 100, trim marker will not be added
testStrimwidth($euc_jp, 0, 100, 'EUC-JP');

// Skip 15 characters into string
testStrimwidth($euc_jp, 15, 100, 'EUC-JP');

// Count 30 characters back from end of string, then limit to width of 5
// Since width of trim marker is 3, this will only get a single char from string
testStrimwidth($euc_jp, -30, 5, 'EUC-JP');

// Count 9 characters from start of string. Since string is 39 characters
// long, this will have the same result as the previous test
testStrimwidth($euc_jp, 9, 5, 'EUC-JP');

// Skip 15 characters, which leaves a total width of 42. Then trim string down
// to 5 less than that, which is a width of 37.
testStrimwidth($euc_jp, 15, -5, 'EUC-JP');

// Take the last 30 characters, which have a width of 54. Trim string down to
// 25 less than that, which is 29.
testStrimwidth($euc_jp, -30, -25, 'EUC-JP');

// Skip over 39 characters... but since string is only 39 characters long,
// it takes us to the end of the string, and output is empty
testStrimwidth($euc_jp, 39, 10, 'EUC-JP');

// Take the last 10 characters, which have a width of 20. Trim string down to
// 12 less than that, which is a width of 8.
testStrimwidth($euc_jp, -10, -12, 'EUC-JP');

try {
    var_dump(mb_strimwidth($euc_jp, 0, -100,'...','EUC-JP'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(mb_strimwidth($euc_jp, 100, 10,'...','EUC-JP'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(mb_strimwidth($euc_jp, -100, 10,'...','EUC-JP'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(mb_strimwidth($euc_jp, -10, -21,'...','EUC-JP'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "\n== Other ==\n";

$str = 'abcdefghijklmnop';
for ($from = -5; $from <= 5; $from++) {
    for ($width = -5; $width <= 5; $width++) {
        if ($from < 0 && $width < 0 && $width < $from) {
            // This case is illegal and will throw an exception
            $pass = false;
            try {
                mb_strimwidth($str, $from, $width, '...', 'ASCII');
            } catch (\ValueError $e) {
                $pass = true;
            }
            if (!$pass)
                die("Expected exception to be thrown");
            continue;
        }

        $result = mb_strimwidth($str, $from, $width, '...', 'ASCII');

        if ($from < 0 && $width < 0 && ($width - $from) <= 3) {
            if ($result !== '...')
                die("Output should have just been trim marker. string=" . $str . " result=" . $result . " from=" . $from . " width=" . $width);
            continue;
        }

        if ($width < 0 || $width > 3) {
            if (mb_substr($result, 0, 1, 'ASCII') !== mb_substr($str, $from, 1, 'ASCII'))
                die("Output string did not start at the expected point! string=" . $str . " result=" . $result . " from=" . $from . " width=" . $width);
        }

        if ($width >= 3 && ($from >= 0 || $width <= abs($from))) {
            if (mb_strwidth($result, 'ASCII') !== $width)
                die("Width was different from requested. string=" . $str . " result=" . $result . " from=" . $from . " width=" . $width);
        }

        if ($width < 0) {
            if (mb_substr($result, -3, 3, 'ASCII') !== '...')
                die("Expected trim marker");
            if (mb_substr($result, -4, 1, 'ASCII') !== mb_substr($str, $width-4, 1, 'ASCII'))
                die("Output string did not end at the expected point. string=" . $str . " result=" . $result . " from=" . $from . " width=" . $width);
        }
    }
}


// Regression test found by fuzzer; old implementation would pass input string
// through when requested width=0
testStrimwidth("\x00\x00\x00\x00+\x00\x00\x00\x00k\x00'\x11Yz", 1, 0, 'greek');

// Regression test; new implementation originally had a bug whereby it would
// sometimes not skip over characters at beginning of string when requested to
testStrimwidthWithMarker(str_repeat("a", 268), '', 12, 255, 'ASCII');

// Try invalid string; invalid sequences will be converted to error markers
testStrimwidth("\x80\x80\x80", 0, 10, 'UTF-8');

// Try invalid marker
// It will be directly concatenated onto truncated string without checking for validity
testStrimwidthWithMarker("abcdefghijklmnop", "\x80\x80\x80", 0, 10, 'UTF-8');

// Regression test; old implementation would pass input string through, even when
// it was wider than requested width, if the trim marker string was wider than
// the input string
testStrimwidthWithMarker("abc", "abcdefghijklmnop", 0, 1, 'ASCII');

// Regression test; old implementation did not handle negative 'from' argument
// correctly when portion being skipped over included fullwidth characters
testStrimwidth("日本語abc", -3, 10, 'UTF-8');

// Regression test; old implementation did not handle positive 'from' argument
// combined with negative 'width' argument correctly when portion being skipped
// over included fullwidth characters
testStrimwidth("日本語abcdef", 3, -1, 'UTF-8');

?>
--EXPECT--
== UTF-16LE ==
String length: 10
String width: 14
Characters: [3100 3200 3300 6100 6200 6300 226f 575b 4b30 6a30]
Character widths: [1 1 1 1 1 1 2 2 2 2]

start=0 width=100 result=[3100 3200 3300 6100 6200 6300 226f 575b 4b30 6a30] length=10 width=14
start=0 width=14 result=[3100 3200 3300 6100 6200 6300 226f 575b 4b30 6a30] length=10 width=14
start=0 width=13 result=[3100 3200 3300 6100 6200 6300 226f 575b 2e00 2e00 2e00] length=11 width=13
start=0 width=12 result=[3100 3200 3300 6100 6200 6300 226f 2e00 2e00 2e00] length=10 width=11
start=0 width=0 result=[2e00 2e00 2e00] length=3 width=3
start=0 width=1 result=[2e00 2e00 2e00] length=3 width=3
start=-3 width=5 result=[575b 2e00 2e00 2e00] length=4 width=5
start=-4 width=-2 result=[226f 2e00 2e00 2e00] length=4 width=5

== EUC-JP ==
String length: 39
String width: 68
Characters: [30 31 32 33 a4b3 a4ce cab8 bbfa cef3 a4cf c6fc cbdc b8ec a4c7 a4b9 a1a3 45 55 43 2d 4a 50 a4f2 bbc8 a4c3 a4c6 a4a4 a4de a4b9 a1a3 c6fc cbdc b8ec a4cf cccc c5dd bdad a4a4 a1a3]
Character widths: [1 1 1 1 2 2 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2]

start=0 width=15 result=[30 31 32 33 a4b3 a4ce cab8 bbfa 2e 2e 2e] length=11 width=15
start=0 width=100 result=[30 31 32 33 a4b3 a4ce cab8 bbfa cef3 a4cf c6fc cbdc b8ec a4c7 a4b9 a1a3 45 55 43 2d 4a 50 a4f2 bbc8 a4c3 a4c6 a4a4 a4de a4b9 a1a3 c6fc cbdc b8ec a4cf cccc c5dd bdad a4a4 a1a3] length=39 width=68
start=15 width=100 result=[a1a3 45 55 43 2d 4a 50 a4f2 bbc8 a4c3 a4c6 a4a4 a4de a4b9 a1a3 c6fc cbdc b8ec a4cf cccc c5dd bdad a4a4 a1a3] length=24 width=42
start=-30 width=5 result=[a4cf 2e 2e 2e] length=4 width=5
start=9 width=5 result=[a4cf 2e 2e 2e] length=4 width=5
start=15 width=-5 result=[a1a3 45 55 43 2d 4a 50 a4f2 bbc8 a4c3 a4c6 a4a4 a4de a4b9 a1a3 c6fc cbdc b8ec a4cf cccc 2e 2e 2e] length=23 width=37
start=-30 width=-25 result=[a4cf c6fc cbdc b8ec a4c7 a4b9 a1a3 45 55 43 2d 4a 50 a4f2 bbc8 a4c3 2e 2e 2e] length=19 width=29
start=39 width=10 result=[] length=0 width=0
start=-10 width=-12 result=[a1a3 c6fc 2e 2e 2e] length=5 width=7
mb_strimwidth(): Argument #3 ($width) is out of range
mb_strimwidth(): Argument #2 ($start) is out of range
mb_strimwidth(): Argument #2 ($start) is out of range
mb_strimwidth(): Argument #3 ($width) is out of range

== Other ==
start=1 width=0 result=[2e 2e 2e] length=3 width=3
start=12 width=255 result=[61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61 61] length=255 width=255
start=0 width=10 result=[3f 3f 3f] length=3 width=3
start=0 width=10 result=[61 62 63 64 65 66 67 80 80 80] length=10 width=10
start=0 width=1 result=[61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 70] length=16 width=16
start=-3 width=10 result=[61 62 63] length=3 width=3
start=3 width=-1 result=[61 62 2e 2e 2e] length=5 width=5
