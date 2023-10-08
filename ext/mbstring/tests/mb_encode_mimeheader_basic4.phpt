--TEST--
Test mb_encode_mimeheader() function : test cases found by fuzzer
--EXTENSIONS--
mbstring
--FILE--
<?php

mb_internal_encoding('UTF-8');

var_dump(mb_encode_mimeheader("", "UTF-8", "Q"));

// Regression test for QPrint-encoding of strings with embedded NUL (zero) bytes
var_dump(mb_encode_mimeheader("abc\x00abc", "UTF-8", "Q"));

// Regression test for input strings which end prematurely
var_dump(mb_encode_mimeheader("\xE2", "UTF-8", "B"));

// Handling of ? signs
var_dump(mb_encode_mimeheader("?", "ASCII", "B"));
var_dump(mb_encode_mimeheader("?", "ASCII", "Q"));

// Handling of = signs
var_dump(mb_encode_mimeheader("=", "ASCII", "B"));
var_dump(mb_encode_mimeheader("=", "ASCII", "Q"));

// Handling of underscores
var_dump(mb_encode_mimeheader("_", "ASCII", "B"));
var_dump(mb_encode_mimeheader("_", "ASCII", "Q"));

// Handling of 0x7F (DEL)
var_dump(mb_encode_mimeheader("\x7f", "ASCII", "B", ""));

// Handling of leading spaces
var_dump(mb_encode_mimeheader(" ", "ASCII", "B"));
var_dump(mb_encode_mimeheader(" ", "ASCII", "Q"));
var_dump(mb_encode_mimeheader("   ", "ASCII", "B"));
var_dump(mb_encode_mimeheader("   ", "ASCII", "Q"));

// Try multiple spaces after a word
var_dump(mb_encode_mimeheader("ab  ab  ", "ASCII", "B"));
var_dump(mb_encode_mimeheader("ab  ab  ", "ASCII", "Q"));

// Trailing spaces
var_dump(mb_encode_mimeheader("` ", "HZ", "B", ""));
var_dump(mb_encode_mimeheader("S ", "ASCII", "Q", "", 73));

// Regression test: extra spaces should not be added at beginning of ASCII string
// when entire input is one ASCII 'word' and high indent value makes us consider
// adding a line feed
var_dump(mb_encode_mimeheader("S4", "ASCII", "B", "\n", 73));
var_dump(mb_encode_mimeheader("S4", "ASCII", "Q", "\n", 73));

// Regression test: converting UTF-8 to UCS-4 and then QPrint-encoding makes string
// take a vastly larger number of bytes; make sure we don't overrun max line length
var_dump(mb_encode_mimeheader("24\x0a", "UCS-4", "Q", "", 29));

// Regression test: include space after ASCII word when switching to Base64 encoding
var_dump(mb_encode_mimeheader("o\x20\x00", "ASCII", "B"));

// Regression test for buffer overrun while performing Base64-encoding
var_dump(mb_encode_mimeheader("\x00\x11\x01\x00\x00\x00\x00\x00\x00\x00", "UCS-4", "B"));

// Regression test for incorrect calculation of when to stop generating output
var_dump(mb_encode_mimeheader("\x01\x00\xcb\xcb\xcb\xcb\xcb\xcb=\xcb\xcb\xcb=?\x01\x00a\x00\x00\xcb\xcb\xcb=?\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb?4?4\xcb\xcb\xcb\xcb\xcb=?\x01\x00\x00\x00\x01\x00\x00\x06\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb=?\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb?4\xcb\xcb\xcb\xcb\xcb?4", "UCS-2", "B", ""));

// 'Line feed' string is truncated to no more than 8 bytes long
$linefeed = "=aaaaaa=?";
var_dump(mb_encode_mimeheader("?", "ASCII", "Q", "=aaaaaa=?", 52));
var_dump($linefeed); // Make sure 'line feed' string was not modified

// Regression test: must take ASCII characters already output at beginning of line
// into account when calculating how many QPrint-encoded characters we can output
// without overrunning max line length
var_dump(mb_encode_mimeheader(",\x20o\x00\x01\x00\x00(", "JIS", "Q", "", 40));

// Make sure we maintain legacy behavior when linefeed string contains NUL (zero) bytes
// (We treat the linefeed string as being truncated at that point)
// The reason is because in the original implementation, the linefeed string was a
// null-terminated C string, so including NUL bytes would have the side effect of
// causing only part of the linefeed string to be used
var_dump(mb_encode_mimeheader("\xff", "ASCII", "Q", "\x00", 54));

// Regression test: After we see a non-ASCII character and switch into Base64/QPrint encoding mode,
// we may need to emit a linefeed before we start the next MIME encoded word
// If so, properly record where the line start position is so we can correctly calculate
// how much output can fit on the line
var_dump(mb_encode_mimeheader("\xff~H~\xe0\xea\x00\x00\xff\xff\xff\xff\xff>\x00\x00\x00\x00", "HZ", "Q", "", 71));

// ASCII strings with no spaces should pass through unchanged
var_dump(mb_encode_mimeheader("yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyBIG5", "BIG-5", "B"));

// Regression test: After decoding part of a line as ASCII, before we switch into Base64/QPrint encoding mode,
// refill our buffer of wchars so we don't hit the end of the buffer in the middle of a line
var_dump(mb_encode_mimeheader("\x20\x20\x20\x202\x20\x20\x20sssssssssssssssssssssssssss\x20\x20\x20\x20W\x20\x20\x20\x20\x20\x20W\x20\x20\x20\x20\xb9S\x01\x00\xf0`\x00\x00\x20\x20\x20\x20mSCII\xee\x20\x20\x20\x20mSCII\xeeI\xee", "ArmSCII-8", "B", ""));

// Regression test: Input string with a huge number of spaces
var_dump(mb_encode_mimeheader("\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x00", "CP936", "Q", ""));

// Regression test: Long string, all ASCII, but with spaces at the beginning
var_dump(mb_encode_mimeheader("\x20\x201111111111111111111111111111111111111111111111111111111111111111111111111", "ASCII", "Q", ""));

// Only a single character in input, but when we convert it to outcode and then
// transfer-encode it, it takes too many bytes to fit on a single line
// Legacy implementation would always include at least one wchar in each encoded word;
// imitate the same behavior
var_dump(mb_encode_mimeheader("\xe7\xad\xb5", "HZ", "Q", "", 44));

// Regression test: Exploring corner cases of when legacy implementation would output plain ASCII
// with no transfer encoding, and when it would transfer-encode
var_dump(mb_encode_mimeheader("2\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20!3", "GB18030", "Q", ""));
var_dump(mb_encode_mimeheader("\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20!3\x20", "GB18030", "Q", ""));

// Change in behavior: The old implementation would output the following string as plain ASCII,
// but the new one transfer-encodes it
// In the general case, matching the old implementation's decision to transfer-encode or not
// perfectly would require allocating potentially unbounded scratch memory (up to the size of
// the input string), but we aim to only use a constant amount of temporarily allocated memory
var_dump(mb_encode_mimeheader("2\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20!3", "GB18030", "Q", ""));

echo "Done";
?>
--EXPECT--
string(0) ""
string(21) "=?UTF-8?Q?abc=00abc?="
string(16) "=?UTF-8?B?Pw==?="
string(19) "=?US-ASCII?B?Pw==?="
string(18) "=?US-ASCII?Q?=3F?="
string(19) "=?US-ASCII?B?PQ==?="
string(18) "=?US-ASCII?Q?=3D?="
string(19) "=?US-ASCII?B?Xw==?="
string(18) "=?US-ASCII?Q?=5F?="
string(19) "=?US-ASCII?B?fw==?="
string(1) " "
string(1) " "
string(3) "   "
string(3) "   "
string(8) "ab  ab  "
string(8) "ab  ab  "
string(1) "`"
string(1) "S"
string(2) "S4"
string(2) "S4"
string(61) "=?UCS-4?Q?=00=00=00=32=00=00=00=34?= =?UCS-4?Q?=00=00=00=0A?="
string(21) "o =?US-ASCII?B?AA==?="
string(68) "=?UCS-4?B?AAAAAAAAABEAAAABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA==?="
string(271) "=?UCS-2?B?AAEAAAA/AD8APwA/AD8APwA9AD8APwA/AD0APwABAAAAYQAAAAAAPwA/AD8=?= =?UCS-2?B?AD0APwA/AD8APwA/AD8APwA/AD8APwA/ADQAPwA0AD8APwA/AD8APwA9AD8=?= =?UCS-2?B?AAEAAAAAAAAAAQAAAAAABgA/AD8APwA/AD8APwA/AD8APwA9AD8APwA/AD8=?= =?UCS-2?B?AD8APwA/AD8APwA/AD8ANAA/AD8APwA/AD8APwA0?="
string(27) "=aaaaaa= =?US-ASCII?Q?=3F?="
string(9) "=aaaaaa=?"
string(55) ", =?ISO-2022-JP?Q?o=00=01=00=00?= =?ISO-2022-JP?Q?=28?="
string(19) " =?US-ASCII?Q?=3F?="
string(76) " =?HZ-GB-2312?Q?=3F=7E=7EH=7E=7E=3F=3F=00=00=3F=3F=3F=3F=3F=3E=00=00=00=00?="
string(75) "yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyBIG5"
string(108) "    2   sssssssssssssssssssssssssss    W      W =?ArmSCII-8?B?ICAgP1MBAD9gAAAgICAgbVNDSUk/ICAgIG1TQ0lJP0k/?="
string(294) "=?CP936?Q?=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20?= =?CP936?Q?=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20?= =?CP936?Q?=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20?= =?CP936?Q?=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=00?="
string(75) "  1111111111111111111111111111111111111111111111111111111111111111111111111"
string(33) "=?HZ-GB-2312?Q?=7E=7Bs=5B=7E=7D?="
string(77) "2                                                                          !3"
string(282) "=?GB18030?Q?=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20?= =?GB18030?Q?=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20?= =?GB18030?Q?=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20?= =?GB18030?Q?=20=20=20=20=20=20=20=20=20=20=20=20!=33=20?="
string(296) "2 =?GB18030?Q?=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20?= =?GB18030?Q?=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20?= =?GB18030?Q?=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20?= =?GB18030?Q?=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20=20!=33?="
Done
