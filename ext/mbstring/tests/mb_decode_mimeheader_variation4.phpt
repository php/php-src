--TEST--
Test mb_decode_mimeheader() function: weird variations found by fuzzer
--EXTENSIONS--
mbstring
--FILE--
<?php

// We convert runs of whitespace, including CR, LF, tab, and space, to a single space...
// but ONLY when that run of whitespace does not occur right in the middle between two
// valid MIME encoded words
mb_internal_encoding('UCS-2');
var_dump(bin2hex(mb_decode_mimeheader("2,\rGCG\xb3GS")));

// We DO convert a run of whitespace to a single space at the very beginning of the input string,
// as long as it is followed by a non-whitespace character
mb_internal_encoding('ASCII');
var_dump(bin2hex(mb_decode_mimeheader("\n8i")));

// But not if it is just a CR or LF at the end of the string
mb_internal_encoding('ASCII');
var_dump(bin2hex(mb_decode_mimeheader("\r")));

// Not if it is a run of whitespace going right up to the end of the string
mb_internal_encoding('ASCII');
var_dump(bin2hex(mb_decode_mimeheader("\n ")));

// Handle = which doesn't initiate a valid encoded word
mb_internal_encoding('ASCII');
var_dump(bin2hex(mb_decode_mimeheader(",\x13@=,")));

// Encoded word which should not be accepted
mb_internal_encoding('ASCII');
var_dump(bin2hex(mb_decode_mimeheader("=?I?B??=")));

// Encoded word with invalid charset name, and input string ends with whitespace
// The old implementation of mb_decode_mimeheader would get 'stuck' on the invalid encoding name
// and could never get out of that state; it would not even try to interpret any other encoded words
// up to the end of the input string
mb_internal_encoding('ISO-8859-7');
var_dump(bin2hex(mb_decode_mimeheader("=?=\x20?=?R\xb7=?=\x20?\x8b\x00====?===??=\xc5UC-R\xb7=?=\x20?=?=====?=\x20?======?======?=\x20?======?===??=\xc5UC-KR\xb7=?=\x20?=?===?==?\x0a")));

// While the old implementation would generally trim off whitespace at the end of the input string,
// but there was a bug whereby it would not do this when the whitespace character was the 100th
// byte of an invalid charset name
mb_internal_encoding('UCS-2');
var_dump(bin2hex(mb_decode_mimeheader("=?\xc2\x86tf7,U\x01\x00`@\x00\x04|\xf1D\x18\x00\x00\x00v\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xff\x13f7,U&\x00S\x01\x00\x17,D\xcb\xcb\xcb\xcb\xcb\xcb\xcb\x01\x00\x00\x14\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\xcb\x00\x11\x00\x00\x00\x00\x00\x00\x0a")));

// Empty encoded word
mb_internal_encoding('ASCII');
var_dump(bin2hex(mb_decode_mimeheader("=?us?B?")));

// Encoded word with just one invalid Base64 byte
mb_internal_encoding('ASCII');
var_dump(bin2hex(mb_decode_mimeheader("=?us?B?-")));

// Encoded word with an invalid Base64 byte followed by a valid Base64 byte
mb_internal_encoding('ASCII');
var_dump(bin2hex(mb_decode_mimeheader("=?us?B?-s")));

// Empty encoded word with a ? which looks like it should be terminator, but = is missing
mb_internal_encoding('ASCII');
var_dump(bin2hex(mb_decode_mimeheader("=?us?B??")));

// Encoded word with just one invalid Base64 byte, but this time properly terminated
mb_internal_encoding('ASCII');
var_dump(bin2hex(mb_decode_mimeheader("=?us?B?\x00?=")));

// Invalid encoded word, followed immediately by valid encoded word
mb_internal_encoding('ASCII');
var_dump(bin2hex(mb_decode_mimeheader("=?=?hz?b?")));

// Another example of invalid encoded word followed immediately by valid encoded word
mb_internal_encoding('ASCII');
var_dump(bin2hex(mb_decode_mimeheader("=?==?hz?b?")));

// Yet another example
mb_internal_encoding('ASCII');
var_dump(bin2hex(mb_decode_mimeheader("=?,=?hz?b?")));

// In PHP 8.0-8.1 this would cause a crash
mb_internal_encoding('UUENCODE');
var_dump(bin2hex(mb_decode_mimeheader("")));

// The conversion filter for SJIS-Mobile#SOFTBANK did not work correctly when it was
// passed the last buffer of wchars without passing 'end' flag, then called one more
// time with an empty buffer and 'end' flag to finish up
mb_internal_encoding('SJIS-Mobile#SOFTBANK');
var_dump(bin2hex(mb_decode_mimeheader("6")));

// Same for SJIS-Mobile#KDDI and SJIS-Mobile#DOCOMO
mb_internal_encoding('SJIS-Mobile#KDDI');
var_dump(bin2hex(mb_decode_mimeheader("6")));
mb_internal_encoding('SJIS-Mobile#DOCOMO');
var_dump(bin2hex(mb_decode_mimeheader("6")));

?>
--EXPECT--
string(36) "0032002c0020004700430047003f00470053"
string(6) "203869"
string(0) ""
string(0) ""
string(10) "2c13403d2c"
string(16) "3d3f493f423f3f3d"
string(200) "3d3f3d203f3d3f523f3d3f3d203f3f003d3d3d3d3f3d3d3d3f3f3d3f55432d523f3d3f3d203f3d3f3d3d3d3d3d3f3d203f3d3d3d3d3d3d3f3d3d3d3d3d3d3f3d203f3d3d3d3d3d3d3f3d3d3d3f3f3d3f55432d4b523f3d3f3d203f3d3f3d3d3d3f3d3d3f"
string(400) "003d003f003f003f007400660037002c0055000100000060004000000004007c003f004400180000000000000076003f003f003f003f003f003f003f003f003f003f003f003f001300660037002c0055002600000053000100000017002c0044003f003f003f003f003f003f003f0001000000000014003f003f003f003f003f003f003f003f003f003f003f003f003f003f003f003f003f003f003f003f003f003f003f003f003f003f003f003f003f003f003f003f003f00000011000000000000000000000000"
string(0) ""
string(2) "3f"
string(2) "3f"
string(0) ""
string(2) "3f"
string(4) "3d3f"
string(6) "3d3f3d"
string(6) "3d3f2c"
string(42) "626567696e20303634342066696c656e616d650a20"
string(2) "36"
string(2) "36"
string(2) "36"
