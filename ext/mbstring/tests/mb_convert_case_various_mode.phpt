--TEST--
Calling mb_convert_case() with an invalid casing mode
--EXTENSIONS--
mbstring
--FILE--
<?php

var_dump(mb_convert_case('foo BAR Spaß', MB_CASE_UPPER));
var_dump(mb_convert_case('foo BAR Spaß', MB_CASE_LOWER));
var_dump(mb_convert_case('foo BAR Spaß', MB_CASE_TITLE));
var_dump(mb_convert_case('foo BAR Spaß', MB_CASE_FOLD));
var_dump(mb_convert_case('foo BAR Spaß', MB_CASE_UPPER_SIMPLE));
var_dump(mb_convert_case('foo BAR Spaß', MB_CASE_LOWER_SIMPLE));
var_dump(mb_convert_case('foo BAR Spaß', MB_CASE_TITLE_SIMPLE));
var_dump(mb_convert_case('foo BAR Spaß', MB_CASE_FOLD_SIMPLE));

// Invalid mode
try {
    var_dump(mb_convert_case('foo BAR Spaß', 100));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "\n-- Greek letter sigma --\n";
var_dump(mb_convert_case("Σ", MB_CASE_TITLE, 'UTF-8'));
var_dump(mb_convert_case("aΣ", MB_CASE_TITLE, 'UTF-8'));
var_dump(mb_convert_case("aΣb", MB_CASE_TITLE, 'UTF-8'));
var_dump(mb_convert_case("aΣ b", MB_CASE_TITLE, 'UTF-8'));
var_dump(mb_convert_case(" ΣΣΣΣ ", MB_CASE_TITLE, 'UTF-8'));

// Apostrophe, full stop, colon, etc. are "case-ignorable"
// When checking whether capital sigma is at the end of a word or not, we skip over
// any number of case-ignorable characters, both when scanning back and when scanning forward
var_dump(mb_convert_case("'Σ", MB_CASE_TITLE, 'UTF-8'));
var_dump(mb_convert_case("ab'Σ", MB_CASE_TITLE, 'UTF-8'));
var_dump(mb_convert_case("Σ'", MB_CASE_TITLE, 'UTF-8'));
var_dump(mb_convert_case("Σ'a", MB_CASE_TITLE, 'UTF-8'));
var_dump(mb_convert_case("a'Σ'a", MB_CASE_TITLE, 'UTF-8'));

// We scan back by at least 63 characters when necessary,
// but there is no guarantee that we will scan back further than that
var_dump(mb_convert_case('a' . str_repeat('.', 63) . "Σ", MB_CASE_TITLE, 'UTF-8'));
var_dump(mb_convert_case('a' . str_repeat('.', 64) . "Σ", MB_CASE_TITLE, 'UTF-8')); // Context-sensitive casing doesn't work here!

// When scanning forward to confirm if capital sigma is at the end of a word or not,
// there is no limit as to how far we will scan
var_dump(mb_convert_case("abcΣ" . str_repeat('.', 64) . ' abc', MB_CASE_TITLE, 'UTF-8'));
var_dump(mb_convert_case("abcΣ" . str_repeat('.', 64) . 'a abc', MB_CASE_TITLE, 'UTF-8'));
var_dump(mb_convert_case("abcΣ" . str_repeat('.', 256) . ' abc', MB_CASE_TITLE, 'UTF-8'));

/* Regression test for new implementation;
 * When converting a codepoint, if we overwrite it with the converted version before
 * checking whether we should shift in/out of 'title mode', then the conversion will be incorrect */
var_dump(bin2hex(mb_convert_case("\x01I\x01,", MB_CASE_TITLE, 'UCS-2BE')));
var_dump(bin2hex(mb_convert_case("\x01I\x01,", MB_CASE_TITLE_SIMPLE, 'UCS-2BE')));

?>
--EXPECT--
string(13) "FOO BAR SPASS"
string(13) "foo bar spaß"
string(13) "Foo Bar Spaß"
string(13) "foo bar spass"
string(13) "FOO BAR SPAß"
string(13) "foo bar spaß"
string(13) "Foo Bar Spaß"
string(13) "foo bar spaß"
mb_convert_case(): Argument #2 ($mode) must be one of the MB_CASE_* constants

-- Greek letter sigma --
string(2) "Σ"
string(3) "Aς"
string(4) "Aσb"
string(5) "Aς B"
string(10) " Σσσς "
string(3) "'Σ"
string(5) "Ab'ς"
string(3) "Σ'"
string(4) "Σ'a"
string(6) "A'σ'a"
string(66) "A...............................................................ς"
string(67) "A................................................................σ"
string(73) "Abcς................................................................ Abc"
string(74) "Abcσ................................................................a Abc"
string(265) "Abcς................................................................................................................................................................................................................................................................ Abc"
string(12) "02bc004e012d"
string(8) "0149012d"
