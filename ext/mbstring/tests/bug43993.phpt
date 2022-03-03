--TEST--
Bug #43993 (mb_substr_count() behaves differently to substr_count() with overlapping needles)
--EXTENSIONS--
mbstring
--FILE--
<?php
var_dump(mb_substr_count("abcabcabca", "abcabc"));
var_dump(mb_substr_count("abcabcabca", "abc"));
var_dump(mb_substr_count("abcabcabca", "cab"));
var_dump(mb_substr_count("abcabcabca", "bca"));
var_dump(mb_substr_count("ababababab", "ba"));
var_dump(mb_substr_count("ababababab", "ab"));
var_dump(mb_substr_count("ababababab", "bc"));
var_dump(mb_substr_count("aaaaaaaaaa", "a"));
var_dump(mb_substr_count("aaaaaaaaaa", "b"));
?>
--EXPECT--
int(1)
int(3)
int(2)
int(3)
int(4)
int(5)
int(0)
int(10)
int(0)
