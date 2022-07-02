--TEST--
Valid Unicode escape sequences
--FILE--
<?php

var_dump("\u{61}"); // ASCII "a" - characters below U+007F just encode as ASCII, as it's UTF-8
var_dump("\u{FF}"); // y with diaeresis
var_dump("\u{ff}"); // case-insensitive
var_dump("\u{2603}"); // Unicode snowman
var_dump("\u{1F602}"); // FACE WITH TEARS OF JOY emoji
var_dump("\u{0000001F602}"); // Leading zeroes permitted
?>
--EXPECT--
string(1) "a"
string(2) "ÿ"
string(2) "ÿ"
string(3) "☃"
string(4) "😂"
string(4) "😂"
