--TEST--
Bug #43008 (php://filter uris ignore url encoded filternames and can't handle slashes)
--SKIPIF--
<?php
if (!extension_loaded("iconv")) die("skip iconv extension not available");
?>
--INI--
allow_url_fopen=1
--FILE--
<?php
$url = b""
	. b"php://filter/read="
	. urlencode(b"convert.iconv.ISO-8859-15/UTF-8")
	. b'|' . urlencode(b"string.rot13")
	. b'|' . urlencode(b"string.rot13")
	. b'|' . urlencode(b"convert.iconv.UTF-8/ISO-8859-15")
	. b"/resource=data://text/plain,foob%E2r";
var_dump(urlencode(file_get_contents($url)));
?>
--EXPECTF--
string(8) "foob%E2r"
