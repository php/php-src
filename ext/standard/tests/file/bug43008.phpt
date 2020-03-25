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
$url = ""
    . "php://filter/read="
    . urlencode("convert.iconv.ISO-8859-15/UTF-8")
    . '|' . urlencode("string.rot13")
    . '|' . urlencode("string.rot13")
    . '|' . urlencode("convert.iconv.UTF-8/ISO-8859-15")
    . "/resource=data://text/plain,foob%E2r";
var_dump(urlencode(file_get_contents($url)));
?>
--EXPECTF--
string(8) "foob%E2r"
