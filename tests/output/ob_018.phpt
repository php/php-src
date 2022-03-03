--TEST--
output buffering - error message nirvana bug #37714
--EXTENSIONS--
zlib
--ENV--
HTTP_ACCEPT_ENCODING=gzip,deflate
--INI--
display_errors=1
zlib.output_compression=1
--FILE--
<?php
ob_start('ob_gzhandler');
?>
--EXPECTF--
‹%a
