--TEST--
Bug #49536 (mb_detect_encoding() returns incorrect results when strict_mode is turned on)
--EXTENSIONS--
mbstring
--FILE--
<?php
// non-strict mode
var_dump(mb_detect_encoding("A\x81", "SJIS", false));
// strict mode
var_dump(mb_detect_encoding("A\x81", "SJIS", true));
// non-strict mode
var_dump(mb_detect_encoding("\xc0\x00", "UTF-8", false));
// strict mode
var_dump(mb_detect_encoding("\xc0\x00", "UTF-8", true));

// Strict mode with multiple candidate encodings
// This input string is invalid in ALL the candidate encodings:
echo "== INVALID STRING - UTF-8 and SJIS ==\n";
var_dump(mb_detect_encoding("\xFF\xFF", ['SJIS', 'UTF-8'], true));
?>
--EXPECT--
string(4) "SJIS"
bool(false)
string(5) "UTF-8"
bool(false)
== INVALID STRING - UTF-8 and SJIS ==
bool(false)
