--TEST--
Bug #31911 (mb_decode_mimeheader() is case-sensitive to hex escapes)
--EXTENSIONS--
mbstring
--FILE--
<?php
echo mb_decode_mimeheader("Works: =?iso-8859-1?q?=3F=3F=3F?=");
echo "\n";
echo mb_decode_mimeheader("Fails: =?iso-8859-1?q?=3f=3f=3f?=")
?>
--EXPECT--
Works: ???
Fails: ???
