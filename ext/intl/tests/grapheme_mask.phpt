--TEST--
grapheme_mask() - Basic functionality
--SKIPIF--
<?php
if (!extension_loaded('intl')) die('skip intl extension not enabled');
?>
--FILE--
<?php
var_dump(grapheme_mask("Hello World", "X"));
var_dump(grapheme_mask("Hello World", "X", 3));
var_dump(grapheme_mask("Hello World", "X", 3, 2));
var_dump(grapheme_mask("Hello World", "X", -5, 3));
var_dump(grapheme_mask("Hello World", "X", 0, 0));
var_dump(grapheme_mask("", "X"));
var_dump(grapheme_mask("Hello World", "👍"));

try {
    var_dump(grapheme_mask("Hello World", "ab"));
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

// Test with emoji sequence (should work - single grapheme cluster)
var_dump(grapheme_mask("Hello", "👨‍👩‍👧‍👦"));

// Test with combining characters (should work - single grapheme cluster)
var_dump(grapheme_mask("Hello", "c\u0301")); // c with acute accent

// Test with ZWJ sequence (should work - single grapheme cluster)
var_dump(grapheme_mask("Hello", "👨‍💻"));
?>
--EXPECT--
string(11) "XXXXX XXXXX"
string(11) "HelXX XXXXX"
string(11) "HelXXo XXXXX"
string(11) "Hello WXXld"
string(11) "Hello World"
string(0) ""
string(11) "👍👍👍👍👍 👍👍👍👍👍"
grapheme_mask(): Argument #2 ($mask_char) must be exactly one grapheme cluster
string(5) "👨‍👩‍👧‍👦👨‍👩‍👧‍👦👨‍👩‍👧‍👦👨‍👩‍👧‍👦👨‍👩‍👧‍👦"
string(5) "ććććć"
string(5) "👨‍💻👨‍💻👨‍💻👨‍💻👨‍💻"
