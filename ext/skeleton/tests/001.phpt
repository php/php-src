--TEST--
Check for xmlrpci presence
--SKIPIF--
<?php if (!extension_loaded("xmlrpci")) print "skip"; ?>
--FILE--
<?php 
echo "Improved XML-RPC extension is available";
?>
--EXPECT--
Improved XML-RPC extension is available
