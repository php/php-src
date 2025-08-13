--TEST--
grapheme_levenshtein() function test
--EXTENSIONS--
intl
--FILE--
<?php
echo '--- Equal ---' . \PHP_EOL;
var_dump(grapheme_levenshtein('12345', '12345'));

echo '--- First string empty ---' . \PHP_EOL;
var_dump(grapheme_levenshtein('', 'xyz'));
echo '--- Second string empty ---' . \PHP_EOL;
var_dump(grapheme_levenshtein('xyz', ''));
echo '--- Both empty ---' . \PHP_EOL;
var_dump(grapheme_levenshtein('', ''));
var_dump(grapheme_levenshtein('', '', 10, 10, 10));

echo '--- 1 character ---' . \PHP_EOL;
var_dump(grapheme_levenshtein('1', '2'));
echo '--- 2 character swapped ---' . \PHP_EOL;
var_dump(grapheme_levenshtein('12', '21'));

echo '--- Inexpensive deletion ---' . \PHP_EOL;
var_dump(grapheme_levenshtein('2121', '11', 2));
echo '--- Expensive deletion ---' . \PHP_EOL;
var_dump(grapheme_levenshtein('2121', '11', 2, 1, 5));

//
echo '--- Inexpensive insertion ---' . \PHP_EOL;
var_dump(grapheme_levenshtein('11', '2121'));
echo '--- Expensive insertion ---' . \PHP_EOL;
var_dump(grapheme_levenshtein('11', '2121', 5));

echo '--- Expensive replacement ---' . \PHP_EOL;
var_dump(grapheme_levenshtein('111', '121', 2, 3, 2));
echo '--- Very expensive replacement ---' . \PHP_EOL;
var_dump(grapheme_levenshtein('111', '121', 2, 9, 2));

echo '--- 128 codepoints ---' . \PHP_EOL;
var_dump(grapheme_levenshtein(str_repeat("a", 128), str_repeat("a", 125) . "abc"));
echo '--- 128 codepoints over ---' . \PHP_EOL;
var_dump(grapheme_levenshtein(str_repeat("a", 128) . "abc", str_repeat("a", 128) . "aaa"));
var_dump(grapheme_levenshtein(str_repeat("a", 256) . "abc", "aaa"));
echo '--- 128 codepoints over only $string1 ---' . \PHP_EOL;
var_dump(grapheme_levenshtein(str_repeat("a", 128) . "abc", "aaa"));
echo '--- 128 codepoints over only $string2 ---' . \PHP_EOL;
var_dump(grapheme_levenshtein("abc", str_repeat("a", 128) . "aaa"));
echo '--- 128 codepoints over Hiragana ---' . \PHP_EOL;
var_dump(grapheme_levenshtein(str_repeat("あ", 128) . "あああ", str_repeat("あ", 128) . "あいう"));

echo '--- Variable selector ---' . \PHP_EOL;
$ka = "カ́";
var_dump(grapheme_levenshtein("カ", $ka));
// variable $nabe and $nabe_E0100 is seems nothing different.
// However, $nabe_E0100 is variable selector in U+908A U+E0100.
// So grapheme_levenshtein result is maybe 0.
$nabe = '邊';
$nabe_E0100 = "邊󠄀";
var_dump(grapheme_levenshtein($nabe, $nabe_E0100));

// variable $nabe and $nabe_E0101 is different because that is IVS.
// $nabe_E0101 is variable selector in U+908A U+E0101.
// grapheme_levenshtein can catches different only match strength is u-ks-identic for locale.
// So result is expect to 1.
$nabe = '邊';
$nabe_E0101 = "\u{908A}\u{E0101}";
var_dump(grapheme_levenshtein($nabe, $nabe_E0101, locale: "ja_JP-u-ks-identic"));

// combining character
var_dump(grapheme_levenshtein("\u{0065}\u{0301}", "\u{00e9}"));

// Corner case
echo '--- Corner case ---' . PHP_EOL;
try {
	grapheme_levenshtein($nabe, $nabe_E0100, 0, 1, 1);
} catch (ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	grapheme_levenshtein($nabe, $nabe_E0100, 1, 0, 1);
} catch (ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	grapheme_levenshtein($nabe, $nabe_E0100, 1, 1, 0);
} catch (ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

echo "--- Invalid locales ---\n";
var_dump(grapheme_levenshtein("abc", "ａｂｃ", locale: "defaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
var_dump(intl_get_error_code());
var_dump(intl_get_error_message());

?>
--EXPECTF--
--- Equal ---
int(0)
--- First string empty ---
int(3)
--- Second string empty ---
int(3)
--- Both empty ---
int(0)
int(0)
--- 1 character ---
int(1)
--- 2 character swapped ---
int(2)
--- Inexpensive deletion ---
int(2)
--- Expensive deletion ---
int(10)
--- Inexpensive insertion ---
int(2)
--- Expensive insertion ---
int(10)
--- Expensive replacement ---
int(3)
--- Very expensive replacement ---
int(4)
--- 128 codepoints ---
int(2)
--- 128 codepoints over ---
int(2)
int(256)
--- 128 codepoints over only $string1 ---
int(128)
--- 128 codepoints over only $string2 ---
int(130)
--- 128 codepoints over Hiragana ---
int(2)
--- Variable selector ---
int(1)
int(0)
int(1)
int(0)
--- Corner case ---
grapheme_levenshtein(): Argument #3 ($insertion_cost) must be greater than 0 and less than or equal to %d
grapheme_levenshtein(): Argument #4 ($replacement_cost) must be greater than 0 and less than or equal to %d
grapheme_levenshtein(): Argument #5 ($deletion_cost) must be greater than 0 and less than or equal to %d
--- Invalid locales ---
bool(false)
int(%d)
string(68) "grapheme_levenshtein(): Error on ucol_open: U_ILLEGAL_ARGUMENT_ERROR"
