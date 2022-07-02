--TEST--
Bug #51269 (zlib.output_compression Overwrites Vary Header)
--INI--
zlib.output_compression=1
--ENV--
HTTP_ACCEPT_ENCODING=gzip
--EXTENSIONS--
zlib
--FILE--
<?php
header('Vary: Cookie');
echo 'foo';
?>
--EXPECTF--
%a
--EXPECTHEADERS--
Vary: Cookie
Content-Encoding: gzip
Vary: Accept-Encoding
