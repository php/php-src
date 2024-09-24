--TEST--
mb_levenshtein() function test
--FILE--
<?php

echo '--- Equal ---' . \PHP_EOL;
var_dump(mb_levenshtein('12345', '12345'));

echo '--- First string empty ---' . \PHP_EOL;
var_dump(mb_levenshtein('', 'xyz'));
echo '--- Second string empty ---' . \PHP_EOL;
var_dump(mb_levenshtein('xyz', ''));
echo '--- Both empty ---' . \PHP_EOL;
var_dump(mb_levenshtein('', ''));
var_dump(mb_levenshtein('', '', 10, 10, 10));

echo '--- 1 character ---' . \PHP_EOL;
var_dump(mb_levenshtein('1', '2'));
echo '--- 2 character swapped ---' . \PHP_EOL;
var_dump(mb_levenshtein('12', '21'));

echo '--- Inexpensive deletion ---' . \PHP_EOL;
var_dump(mb_levenshtein('2121', '11', 2));
echo '--- Expensive deletion ---' . \PHP_EOL;
var_dump(mb_levenshtein('2121', '11', 2, 1, 5));

echo '--- Inexpensive insertion ---' . \PHP_EOL;
var_dump(mb_levenshtein('11', '2121'));
echo '--- Expensive insertion ---' . \PHP_EOL;
var_dump(mb_levenshtein('11', '2121', 5));

echo '--- Expensive replacement ---' . \PHP_EOL;
var_dump(mb_levenshtein('111', '121', 2, 3, 2));
echo '--- Very expensive replacement ---' . \PHP_EOL;
var_dump(mb_levenshtein('111', '121', 2, 9, 2));

echo '--- 128 codepoints over ---' . \PHP_EOL;
var_dump(mb_levenshtein(str_repeat("a", 128) . "abc", str_repeat("a", 128) . "aaa"));
echo '--- 128 codepoints over only $string1 ---' . \PHP_EOL;
var_dump(mb_levenshtein(str_repeat("a", 128) . "abc", "aaa"));
echo '--- 128 codepoints over only $string2 ---' . \PHP_EOL;
var_dump(mb_levenshtein("abc", str_repeat("a", 128) . "aaa"));
echo '--- 128 codepoints over Hiragana ---' . \PHP_EOL;
var_dump(mb_levenshtein(str_repeat("あ", 128) . "あああ", str_repeat("あ", 128) . "あいう"));

echo '--- 128 codepoints over Hiragana in Shift_JIS ---' . \PHP_EOL;
$hiragana_a = mb_convert_encoding("あ", "SJIS", "UTF-8");
$hiragana_aiu = mb_convert_encoding("あいう", "SJIS", "UTF-8");
var_dump(mb_levenshtein(str_repeat($hiragana_a, 128 + 3), str_repeat($hiragana_a, 128) . $hiragana_aiu, encoding: "SJIS"));
?>
--EXPECT--
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
--- 128 codepoints over ---
int(2)
--- 128 codepoints over only $string1 ---
int(128)
--- 128 codepoints over only $string2 ---
int(130)
--- 128 codepoints over Hiragana ---
int(2)
--- 128 codepoints over Hiragana in Shift_JIS ---
int(2)
