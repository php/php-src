--TEST--
Check for xslt presence
--SKIPIF--
<?php if (!extension_loaded("xslt")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php 
echo "xslt extension is available";
?>
--EXPECT--
xslt extension is available