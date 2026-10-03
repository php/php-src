--TEST--
htmlspecialchars() returns the input string when there is nothing to encode
--FILE--
<?php
$s = str_repeat('abc', 10);
$s[0] = 'x'; // not interned
$r = htmlspecialchars($s);
debug_zval_dump($s);
$r = htmlspecialchars("caf\u{E9}");
debug_zval_dump($r);
$s = str_repeat('"it\'s" ', 4); // quotes that are not encoded
$s[0] = '"'; // not interned
$r = htmlspecialchars($s, ENT_NOQUOTES);
debug_zval_dump($s);

// encoding starts after the part that is copied unchanged
foreach ([0, 1, 15, 16, 17, 31, 32, 33] as $n) {
    $p = str_repeat('a', $n);
    echo htmlspecialchars("$p<", ENT_QUOTES), ' ', htmlspecialchars("$p>'", ENT_QUOTES), ' ', htmlspecialchars("$p\"'", ENT_NOQUOTES), ' ', htmlspecialchars("$p\"'", ENT_COMPAT), "\n";
}

// invalid UTF-8 after valid multi-byte characters
$p = str_repeat("\u{E9}\u{20AC}\u{1F600}", 3);
var_dump(htmlspecialchars("$p\xED\xA0\x80<", ENT_QUOTES));
var_dump(htmlspecialchars("$p\xED\xA0\x80<", ENT_QUOTES | ENT_IGNORE));
var_dump(htmlspecialchars("$p\xF4\x90\x80\x80<", ENT_QUOTES | ENT_SUBSTITUTE));

// bytes above 0x7F in other charsets
var_dump(bin2hex(htmlspecialchars(str_repeat("\xE9", 20) . '<', ENT_QUOTES, 'ISO-8859-1')));
var_dump(htmlspecialchars(str_repeat('a', 20) . "\x81<", ENT_QUOTES | ENT_SUBSTITUTE, 'Shift_JIS'));

// double_encode = false
var_dump(htmlspecialchars(str_repeat('a', 20) . '&amp;&foo &#39;', ENT_QUOTES, 'UTF-8', false));
?>
--EXPECT--
string(30) "xbcabcabcabcabcabcabcabcabcabc" refcount(3)
string(5) "café" interned
string(28) ""it's" "it's" "it's" "it's" " refcount(3)
&lt; &gt;&#039; "' &quot;'
a&lt; a&gt;&#039; a"' a&quot;'
aaaaaaaaaaaaaaa&lt; aaaaaaaaaaaaaaa&gt;&#039; aaaaaaaaaaaaaaa"' aaaaaaaaaaaaaaa&quot;'
aaaaaaaaaaaaaaaa&lt; aaaaaaaaaaaaaaaa&gt;&#039; aaaaaaaaaaaaaaaa"' aaaaaaaaaaaaaaaa&quot;'
aaaaaaaaaaaaaaaaa&lt; aaaaaaaaaaaaaaaaa&gt;&#039; aaaaaaaaaaaaaaaaa"' aaaaaaaaaaaaaaaaa&quot;'
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa&lt; aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa&gt;&#039; aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"' aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa&quot;'
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa&lt; aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa&gt;&#039; aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"' aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa&quot;'
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa&lt; aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa&gt;&#039; aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"' aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa&quot;'
string(0) ""
string(31) "é€😀é€😀é€😀&lt;"
string(34) "é€😀é€😀é€😀�&lt;"
string(48) "e9e9e9e9e9e9e9e9e9e9e9e9e9e9e9e9e9e9e9e9266c743b"
string(32) "aaaaaaaaaaaaaaaaaaaa&#xFFFD;&lt;"
string(39) "aaaaaaaaaaaaaaaaaaaa&amp;&amp;foo &#39;"
