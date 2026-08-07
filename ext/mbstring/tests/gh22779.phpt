--TEST--
GH-22779: mb_strrpos() wrong result for a negative offset in a non-UTF-8 encoding
--EXTENSIONS--
mbstring
--FILE--
<?php
/* mb_strrpos() with a negative offset must return the correct position in non-UTF-8 encodings. */
$haystack = "\xA9\xA9X";
$needle = "\xA9";
foreach ([-1, -2, -3] as $offset) {
    var_dump(mb_strrpos($haystack, $needle, $offset, 'ISO-8859-1'));
}
var_dump(mb_strrpos("X\xA9", "\xA9", -1, 'ISO-8859-1'));
var_dump(mb_strrpos("\x95Z\x95Z", "\x95", -1, 'Windows-1252'));
var_dump(mb_strrpos("\x95Z\x95Z", "\x95", -2, 'Windows-1252'));
// Two-byte needle in a single-byte encoding.
var_dump(mb_strrpos("\xA9\xB0\xA9\xB0Z", "\xA9\xB0", -2, 'ISO-8859-1'));
// Multibyte: "ああA" in Shift_JIS, needle "あ" (\x82\xA0).
var_dump(mb_strrpos("\x82\xA0\x82\xA0A", "\x82\xA0", -2, 'SJIS'));
var_dump(mb_strrpos("\x82\xA0\x82\xA0A", "\x82\xA0", -3, 'SJIS'));
// UTF-16: an ASCII needle miscounts the other way.
var_dump(mb_strrpos("\x00A\x00X\x00A", "\x00A", -2, 'UTF-16BE'));
?>
--EXPECT--
int(1)
int(1)
int(0)
int(1)
int(2)
int(2)
int(2)
int(1)
int(0)
int(0)
