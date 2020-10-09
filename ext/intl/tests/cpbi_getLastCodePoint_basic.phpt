--TEST--
IntlCodepointBreakIterator::getLastCodePoint(): basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");

$text = 'ตัวอย่างข้อความ';

$codepoint_it = IntlBreakIterator::createCodePointInstance();
$codepoint_it->setText($text);

var_dump($codepoint_it->getLastCodePoint());
//first() and last() don't read codepoint and set the last code point var to -1
//The pointer is after the last read codepoint if moving forward and
//before the last read codepoint is moving backwards
$p = $codepoint_it->first();
while ($p != IntlBreakIterator::DONE) {
    $c = $codepoint_it->getLastCodePoint();
    if ($c > 0)
        var_dump(sprintf('U+%04X', $codepoint_it->getLastCodePoint()));
    else
        var_dump($c);
    //it's a post-increment operation as to the codepoint, i.e., it gives the codepoint
    //starting at the initial position and only then moves the pointer forward
    $p = $codepoint_it->next();
}

echo "Now backwards\n";
$p = $codepoint_it->last();
while ($p != IntlBreakIterator::DONE) {
    $c = $codepoint_it->getLastCodePoint();
    if ($c > 0)
        var_dump(sprintf('U+%04X', $codepoint_it->getLastCodePoint()));
    else
        var_dump($c);
    $p = $codepoint_it->previous();
}


?>
--EXPECT--
int(-1)
int(-1)
string(6) "U+0E15"
string(6) "U+0E31"
string(6) "U+0E27"
string(6) "U+0E2D"
string(6) "U+0E22"
string(6) "U+0E48"
string(6) "U+0E32"
string(6) "U+0E07"
string(6) "U+0E02"
string(6) "U+0E49"
string(6) "U+0E2D"
string(6) "U+0E04"
string(6) "U+0E27"
string(6) "U+0E32"
string(6) "U+0E21"
Now backwards
int(-1)
string(6) "U+0E21"
string(6) "U+0E32"
string(6) "U+0E27"
string(6) "U+0E04"
string(6) "U+0E2D"
string(6) "U+0E49"
string(6) "U+0E02"
string(6) "U+0E07"
string(6) "U+0E32"
string(6) "U+0E48"
string(6) "U+0E22"
string(6) "U+0E2D"
string(6) "U+0E27"
string(6) "U+0E31"
string(6) "U+0E15"
