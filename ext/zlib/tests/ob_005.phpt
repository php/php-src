--TEST--
ob_gzhandler
--EXTENSIONS--
zlib
--CGI-
--INI--
zlib.output_compression=0
--ENV--
HTTP_ACCEPT_ENCODING=gzip
--POST--
dummy=42
--FILE--
<?php
ob_start("ob_gzhandler");
ini_set("zlib.output_compression", 0);
echo "hi\n";
?>
--EXPECT--
hi
--EXPECTHEADERS--
