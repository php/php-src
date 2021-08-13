--TEST--
Exhaustive test of verification and conversion of HZ text
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
include('encoding_tests.inc');
srand(1000); // Make results consistent
mb_substitute_character(0x25); // '%'

for ($i = 0; $i < 0x80; $i++) {
    if ($i != 0x7E) // ~ is special and will be tested separately
        testValidString(chr($i), chr($i), 'ASCII', 'HZ');
}
echo "Tested ASCII -> HZ\n";

for ($i = 0; $i < 0x80; $i++) {
    if ($i != 0x7E)
        testValidString(chr($i), chr($i), 'HZ', 'ASCII');
}
echo "Tested HZ -> ASCII\n";

for ($i = 0x80; $i < 0xFF; $i++) {
    testInvalidString(chr($i), '%', 'HZ', 'ASCII');
}
echo "Tested non-ASCII bytes in ASCII mode\n";

testValidString('~~', '~', 'HZ', 'ASCII');
testValidString("~\n", '', 'HZ', 'ASCII', false);
testValidString('~{~}', '', 'HZ', 'ASCII', false);
testValidString("~{~\n~}", '', 'HZ', 'ASCII', false);
echo "Tested valid ~ escapes\n";

for ($i = 0; $i < 0xFF; $i++) {
    if ($i != 0x0A) {
        // Try invalid ~ escapes both in ASCII and GB modes
        if ($i != 0x7E && $i != 0x7B) // not {
            testInvalidString("~" . chr($i), '%', 'HZ', 'ASCII');
        if ($i != 0x7D) // not }
            testInvalidString("~{~" . chr($i) . "~}", '%', 'HZ', 'ASCII');
    }
}
echo "Tested all invalid ~ escapes\n";

readConversionTable(__DIR__ . '/data/GB2312.txt', $toUnicode, $fromUnicode);

findInvalidChars($toUnicode, $invalid, $truncated);

// Two characters in ISO-2022-CN convert to Unicode 0x2225
$irreversible = ["\x21\x2C" => true];

// Test all good GB2312 characters within ~{ ~} escapes
$goodChars = array_keys($toUnicode);
shuffle($goodChars);
while (!empty($goodChars)) {
    $reversible = true;
    $length = 1; //min(rand(5,10), count($goodChars));
    $fromString = $toString = '';
    while ($length--) {
        $goodChar = array_pop($goodChars);
        $fromString .= $goodChar;
        $toString .= $toUnicode[$goodChar];
        if (isset($irreversible[$goodChar]))
          $reversible = false;
    }

    testValidString('~{' . $fromString . '~}', $toString, 'HZ', 'UTF-16BE', $reversible);
}

// Test all invalid GB2312 characters within ~{ ~} escapes
// However, don't test escape sequences; we will do those separately below
unset($invalid["~"]);
$badChars = array_keys($invalid);
$goodChars = array();
while (!empty($badChars)) {
    if (empty($goodChars)) {
        $goodChars = array_keys($toUnicode);
        shuffle($goodChars);
    }
    $goodChar   = array_pop($goodChars);
    $fromString = array_pop($badChars) . $goodChar;
    $toString   = "\x00%" . $toUnicode[$goodChar];

    testInvalidString('~{' . $fromString . '~}', $toString, 'HZ', 'UTF-16BE');
}

$truncatedChars = array_keys($truncated);
foreach ($truncatedChars as $truncatedChar) {
    testInvalidString('~{' . $truncatedChar, "\x00%", 'HZ', 'UTF-16BE');
}

echo "Tested HZ -> UTF-16BE (for all GB2312 characters)\n";

findInvalidChars($fromUnicode, $invalid, $unused, array_fill_keys(range(0,0xFF), 2));

// Although they do not appear in the Unicode -> GB2312 map, ASCII characters *are*
// valid to convert to HZ
for ($i = 0; $i <= 0x7F; $i++)
    unset($invalid["\x00" . chr($i)]);

$badChars = array_keys($invalid);
$goodChars = array();
while (!empty($badChars)) {
    if (empty($goodChars)) {
        $goodChars = array_keys($fromUnicode);
        shuffle($goodChars);
    }
    $goodChar   = array_pop($goodChars);
    $fromString = array_pop($badChars) . $goodChar;
    $toString   = "%~{" . $fromUnicode[$goodChar] . "~}";

    convertInvalidString($fromString, $toString, 'UTF-16BE', 'HZ');
}

echo "Tested UTF-16BE -> HZ (for all GB2312 characters)\n";

?>
--EXPECT--
Tested ASCII -> HZ
Tested HZ -> ASCII
Tested non-ASCII bytes in ASCII mode
Tested valid ~ escapes
Tested all invalid ~ escapes
Tested HZ -> UTF-16BE (for all GB2312 characters)
Tested UTF-16BE -> HZ (for all GB2312 characters)
