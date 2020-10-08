--TEST--
Bug #80203 (imap_mime_header_decode() is not binary safe)
--SKIPIF--
<?php
if (!extension_loaded("imap")) die("skip imap extension not available");
?>
--FILE--
<?php
var_dump(bin2hex(imap_mime_header_decode("=?UTF-8?Q?foo=00bar?=")[0]->text));
var_dump(bin2hex(imap_mime_header_decode("=?UTF-8?B?Zm9vAGJhcg==?=")[0]->text));
?>
--EXPECT--
string(14) "666f6f00626172"
string(14) "666f6f00626172"
