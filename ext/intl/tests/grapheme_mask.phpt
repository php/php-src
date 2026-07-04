--TEST--
grapheme_mask() basic test
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip intl extension not loaded'; ?>
--FILE--
<?php

// Test 1: Basic ASCII string masking
var_dump(grapheme_mask("Hello World", "eo", 0, 0, "X"));

// Test 2: Unicode string masking (Persian with a mask character)
var_dump(grapheme_mask("سلام دنیا", "ا", 0, 0, "*"));

// Test 3: With start and length parameters
var_dump(grapheme_mask("abcdefghij", "aeiou", 1, 5, "#"));

// Test 4: Default mask character (*)
var_dump(grapheme_mask("test", "t"));

// Test 5: Mask character more than one grapheme → should return false
var_dump(grapheme_mask("hello", "l", 0, 0, "ab"));

// Test 6: Empty maskChars → nothing masked
var_dump(grapheme_mask("hello", ""));

// Test 7: Multibyte mask character (e.g., Persian character as mask)
var_dump(grapheme_mask("a b c", " ", 0, 0, "🌟"));

?>
--EXPECT--
string(11) "HXllo World"
string(13) "س*لم دنی*"
string(10) "abcde#ghij"
string(4) "*es*"
bool(false)
string(5) "hello"
string(5) "a🌟b🌟c"
