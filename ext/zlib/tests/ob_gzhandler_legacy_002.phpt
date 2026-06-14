--TEST--
ob_gzhandler legacy
--EXTENSIONS--
zlib
--CGI--
--INI--
zlib.output_compression=0
--ENV--
HTTP_ACCEPT_ENCODING=
--POST--
dummy=42
--FILE--
<?php
if (false !== ob_gzhandler("", PHP_OUTPUT_HANDLER_START)) {
    ini_set("zlib.output_compression", 0);
    ob_start("ob_gzhandler");
}
echo "hi\n";
?>
--EXPECT--
hi
--EXPECTHEADERS--
