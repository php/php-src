--TEST--
GH-25832: mb_encode_mimeheader() rejects null for $charset and $transfer_encoding
--EXTENSIONS--
mbstring
--FILE--
<?php
mb_internal_encoding('UTF-8');
mb_language('neutral');
var_dump(mb_encode_mimeheader('abc é', null));
var_dump(mb_encode_mimeheader('abc é', null, null));
var_dump(mb_encode_mimeheader('abc é', 'UTF-8', null));
mb_language('English');
var_dump(mb_encode_mimeheader('abc é', null, null));
?>
--EXPECT--
string(20) "abc =?UTF-8?B?w6k=?="
string(20) "abc =?UTF-8?B?w6k=?="
string(20) "abc =?UTF-8?B?w6k=?="
string(24) "abc =?ISO-8859-1?Q?=E9?="
