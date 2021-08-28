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
echo "7bit done\n";

// "8bit"
var_dump(mb_convert_encoding("\x01\x00", "8bit", "UTF-16BE")); // codepoints over 0xFF are illegal for '8-bit'
echo "8bit done\n";


// UCS-2
echo bin2hex(mb_convert_encoding("\xFF\xFE\x00\x30", "UTF-16BE", "UCS-2")), "\n";
echo bin2hex(mb_convert_encoding("\xFE\xFF\x30\x00", "UTF-16BE", "UCS-2")), "\n";
echo bin2hex(mb_convert_encoding("\x00\x30", "UTF-16BE", "UCS-2LE")), "\n";
echo "UCS-2 done\n";

?>
--EXPECT--
string(3) "ABC"
string(1) "%"
string(3) "ABC"
7bit done
string(1) "%"
8bit done
3000
3000
3000
UCS-2 done
