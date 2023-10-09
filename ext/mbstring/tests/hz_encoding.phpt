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
testValidString('~~?', '~?', 'HZ', 'ASCII');
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

// Regression tests
if (mb_convert_encoding("\x7A\xFA\x00\x00", 'HZ', 'UTF-16BE') !== "~{\x73\x43~}\x00")
    die("Bad");
if (mb_convert_encoding("~", 'UTF-16BE', 'HZ') !== "")
    die("Bad");
// There had once been a bug whereby the output buffer would be overrun by one byte.
// It was found by fuzzing. Reproducing it required a string which was long enough
// and had a GB2312 character at the end.
$str = "\xD9\x96C\xA7\x1B\xF6\xD8\x86\x94\xB0\xA0\xE1\x9D\x8C\xF8G\xBBMk\xD2Y\tt\xF1\x96d\x17JA\xF9\xF8\xCF\xDC\xFE\x8E\x0E\xC1\x84\xDA\xDBM\xC1\x87\x1AZ\xD5\xA6)\xFF%2\\\xCC\x02\x16]Y\xF0\x00\xEA\xE8{)\x81\xD5VQZ\x12\xB5\xBC\x9A\x91\xA0x\x02\xBA\xF6c\xACo\x9BH\xB7qx\xF5\x0F\t\x15\xDByx\xBA[\xC9\xE8r\xCD*:\xBF\x10P\xF1>Q\x07\xEE\xE5\x80\xAD\xB9\xA2\x9B\xF6\xE1,\x82\xC6q\x94E\xD4\x0B\xC6\xBCQe=\xC3\xE0\xC8\xE0R\x97\x14q\x0C\x1A\x7F\xE1\xC4\xB8U\x8A\x86\x93\xB6/\x84\x95\x06\x91W\xB2\xB6\x1F!\t X\x1A\xD5\xD6\xDA<\x81ib\x9A\x1B3\xD3\xB7:\xE2QS\xD0\x91\x99[K\xF2E\xBBjoh_5\x15 \xA4\xCC\xB0\x7F\x06\xB3,\xB3\xA7u\xB9\x82\x00\xE2f$\x1C\x84NsP\xFAiPB{\x8D\xBA\xB3[\x88\xA9\xB1\xA2r\x86\xFF<\xFD\xFB\xF8\xD6\xABq\x00z\xFA\x87\x8C_\xD9N\xF2\xFA\xEA\xEA\xAA\xD7\xFA\xA2\xD4\x85/\xFC\xE1}\xF7\x9C\x86\xDD\x12@\xC3\xDA\nC\x1Di\xA9\xB0\xC3\xB3\x04\xB2\x1A\x07BA\x02\xED\x11\xA4\xDAz\x96\xB5\xD0!p\xE2\xAD\xEDI\xEF\xF7\\\x05d.p\x07\xC4\x8B\x952\xCDz\x90\x8C\xA6U\xDB\xC7\xF4\x94\xE9\x16X\xF1\xCC\xB13\x07a9\x86]\xF9k\xA9\x87E\xCB\x89\x9Fd\x0E\x81m\xC6c\xDA\x9C\xE9\xAF\x80.\xFAq\xD9\xAAd\x1DB\x1F\x854\xE8\x82v)A\xF3\xB4\x1D\xE5\xF0\xFFu\x0E\x0C\xC4q\xF0\xE7\xB4p\x86\xE6]9\xD9\xA5O\xBAw\x1B\x8D&]\x9D\xE2\x0F\xD2\xD5\x13AY#\x81\x90\xB2\xE8\xDA\xD2\xFC>\xA0\x9A\xBD\x0B\xCC\x08>\x1E\xD1\xFEgr1'$\xEE\xA2!\x8A\xBB>\x11j#Pz_!?\xA8\x15\xCF\xCB\x84\x86\xC1\xF78:\xDA\xBCE\xA7\x02SO\x8B\x81>\x96\xBD\xFD2\x84\xC5\xFC\x19\xE5\xF4\xEFp\xF08K\xBB\xAE-[}\xE1\xDB\x8A%6\xC7\xC9";
if (substr(mb_convert_encoding($str, 'HZ', 'CP51932'), -4) !== "\x45\x49~}")
    die("Bad");

// Test "long" illegal character markers
mb_substitute_character("long");
convertInvalidString("~A", "%", "HZ", "UTF-8");
convertInvalidString("\x80", "%", "HZ", "UTF-8");
convertInvalidString("~{\x22\x21", "%", "HZ", "UTF-8");

echo "Done!\n";
?>
--EXPECT--
Tested ASCII -> HZ
Tested HZ -> ASCII
Tested non-ASCII bytes in ASCII mode
Tested valid ~ escapes
Tested all invalid ~ escapes
Tested HZ -> UTF-16BE (for all GB2312 characters)
Tested UTF-16BE -> HZ (for all GB2312 characters)
Done!
