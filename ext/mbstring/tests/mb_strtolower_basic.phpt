--TEST--
Test mb_strtolower() function : basic functionality
--EXTENSIONS--
mbstring
--FILE--
<?php
/*
 * Test basic functionality of mb_strtolower
 */

echo "*** Testing mb_strtolower() : basic functionality***\n";

$ascii_lower = 'abcdefghijklmnopqrstuvwxyz';
$ascii_upper = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';
$greek_lower = base64_decode('zrHOss6zzrTOtc62zrfOuM65zrrOu868zr3Ovs6/z4DPgc+Dz4TPhc+Gz4fPiM+J');
$greek_upper = base64_decode('zpHOks6TzpTOlc6WzpfOmM6ZzprOm86czp3Ons6fzqDOoc6jzqTOpc6mzqfOqM6p');

echo "\n-- ASCII String --\n";
$ascii = mb_strtolower($ascii_upper);
var_dump($ascii);

if($ascii == $ascii_lower) {
    echo "Correctly converted\n";
} else {
    echo "Incorrectly converted\n";
}

echo "\n-- Multibyte String --\n";
$mb = mb_strtolower($greek_upper, 'UTF-8');
var_dump(base64_encode($mb));

if ($mb == $greek_lower) {
    echo "Correctly converted\n";
} else {
    echo "Incorrectly converted\n";
}

echo "\n-- Greek letter sigma --\n";
var_dump(mb_strtolower("Σ", 'UTF-8'));
var_dump(mb_strtolower("aΣ", 'UTF-8'));
var_dump(mb_strtolower("aΣb", 'UTF-8'));
var_dump(mb_strtolower("aΣ b", 'UTF-8'));
var_dump(mb_strtolower(" ΣΣΣΣ ", 'UTF-8'));

// Apostrophe, full stop, colon, etc. are "case-ignorable"
// When checking whether capital sigma is at the end of a word or not, we skip over
// any number of case-ignorable characters, both when scanning back and when scanning forward
var_dump(mb_strtolower("'Σ", 'UTF-8'));
var_dump(mb_strtolower("ab'Σ", 'UTF-8'));
var_dump(mb_strtolower("Σ'", 'UTF-8'));
var_dump(mb_strtolower("Σ'a", 'UTF-8'));
var_dump(mb_strtolower("a'Σ'a", 'UTF-8'));

// We scan back by at least 63 characters when necessary,
// but there is no guarantee that we will scan back further than that
var_dump(mb_strtolower('a' . str_repeat('.', 63) . "Σ", 'UTF-8'));
var_dump(mb_strtolower('a' . str_repeat('.', 64) . "Σ", 'UTF-8')); // Context-sensitive casing doesn't work here!

// When scanning forward to confirm if capital sigma is at the end of a word or not,
// there is no limit as to how far we will scan
var_dump(mb_strtolower("abcΣ" . str_repeat('.', 64) . ' abc', 'UTF-8'));
var_dump(mb_strtolower("abcΣ" . str_repeat('.', 64) . 'a abc', 'UTF-8'));
var_dump(mb_strtolower("abcΣ" . str_repeat('.', 256) . ' abc', 'UTF-8'));

echo "Done";
?>
--EXPECT--
*** Testing mb_strtolower() : basic functionality***

-- ASCII String --
string(26) "abcdefghijklmnopqrstuvwxyz"
Correctly converted

-- Multibyte String --
string(64) "zrHOss6zzrTOtc62zrfOuM65zrrOu868zr3Ovs6/z4DPgc+Dz4TPhc+Gz4fPiM+J"
Correctly converted

-- Greek letter sigma --
string(2) "σ"
string(3) "aς"
string(4) "aσb"
string(5) "aς b"
string(10) " σσσς "
string(3) "'σ"
string(5) "ab'ς"
string(3) "σ'"
string(4) "σ'a"
string(6) "a'σ'a"
string(66) "a...............................................................ς"
string(67) "a................................................................σ"
string(73) "abcς................................................................ abc"
string(74) "abcσ................................................................a abc"
string(265) "abcς................................................................................................................................................................................................................................................................ abc"
Done
