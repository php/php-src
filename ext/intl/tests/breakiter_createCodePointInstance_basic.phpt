--TEST--
IntlBreakIterator::createCodePointInstance(): basic test
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");

$text = 'ตัวอย่างข้อความ';

$codepoint_it = IntlBreakIterator::createCodePointInstance();
var_dump(get_class($codepoint_it));
$codepoint_it->setText($text);

print_r(iterator_to_array($codepoint_it));

?>
--EXPECT--
string(26) "IntlCodePointBreakIterator"
Array
(
    [0] => 0
    [1] => 3
    [2] => 6
    [3] => 9
    [4] => 12
    [5] => 15
    [6] => 18
    [7] => 21
    [8] => 24
    [9] => 27
    [10] => 30
    [11] => 33
    [12] => 36
    [13] => 39
    [14] => 42
    [15] => 45
)
