--TEST--
Test mb_decode_mimeheader() function: use of underscores in QPrint-encoded data
--EXTENSIONS--
mbstring
--FILE--
<?php

// RFC 2047 says that in a QPrint-encoded MIME encoded word, underscores should be converted to spaces
var_dump(mb_decode_mimeheader("=?UTF-8?Q?abc?="));
var_dump(mb_decode_mimeheader("=?UTF-8?Q?abc_def?="));
var_dump(mb_decode_mimeheader("_=?UTF-8?Q?abc_def?=_"));
var_dump(mb_decode_mimeheader("=?UTF-8?Q?__=E6=B1=89=E5=AD=97__?="));

// This is how underscores should be encoded in MIME encoded words with QPrint
var_dump(mb_decode_mimeheader("=?UTF-8?Q?=5F?="));

?>
--EXPECT--
string(3) "abc"
string(7) "abc def"
string(9) "_abc def_"
string(10) "  汉字  "
string(1) "_"
