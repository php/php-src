--TEST--
IntlCodepointBreakIterator's part iterator
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");

$text = 'ตัวอย่างข้อความ';

$it = IntlBreakIterator::createCodePointInstance()->getPartsIterator();
$it->getBreakIterator()->setText($text);

foreach ($it as $k => $v) {
    echo "$k. $v (" . sprintf("U+%04X", $it->getBreakIterator()->getLastCodePoint()) .
        ") at {$it->getBreakIterator()->current()}\r\n";
}

?>
--EXPECT--
0. ต (U+0E15) at 3
1. ั (U+0E31) at 6
2. ว (U+0E27) at 9
3. อ (U+0E2D) at 12
4. ย (U+0E22) at 15
5. ่ (U+0E48) at 18
6. า (U+0E32) at 21
7. ง (U+0E07) at 24
8. ข (U+0E02) at 27
9. ้ (U+0E49) at 30
10. อ (U+0E2D) at 33
11. ค (U+0E04) at 36
12. ว (U+0E27) at 39
13. า (U+0E32) at 42
14. ม (U+0E21) at 45
