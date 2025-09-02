--TEST--
Test of oddball text encodings which are not tested elsewhere
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
mb_substitute_character(0x25);

// "7bit". This is not a real text encoding.
var_dump(mb_convert_encoding("ABC", "7bit", "ASCII"));
var_dump(mb_convert_encoding("\x80", "7bit", "ASCII"));
var_dump(mb_convert_encoding("ABC", "8bit", "7bit"));
var_dump(mb_check_encoding(chr(255), '7bit'));
var_dump(mb_convert_encoding("\xAC\xAC", '7bit', 'UHC'));
echo "7bit done\n";

// "8bit"
var_dump(mb_convert_encoding("\x01\x00", "8bit", "UTF-16BE")); // codepoints over 0xFF are illegal for '8-bit'
echo "8bit done\n";

?>
--EXPECT--
string(3) "ABC"
string(1) "%"
string(3) "ABC"
bool(false)
string(1) "%"
7bit done
string(1) "%"
8bit done
