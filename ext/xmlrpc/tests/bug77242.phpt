--TEST--
Bug #77242 (heap out of bounds read in xmlrpc_decode())
--SKIPIF--
<?php if (!extension_loaded("xmlrpc")) print "skip"; ?>
--FILE--
<?php
var_dump(xmlrpc_decode(base64_decode("PD94bWwgdmVyc2lvbmVuY29kaW5nPSJJU084ODU5NyKkpKSkpKSkpKSkpKSkpKSkpKSkpKSk")));
?>
--EXPECT--
NULL